#include "ai/nav_mesh.h"

#include <blam/volta/blam_bsp_structures.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <map>
#include <queue>

using namespace typing::vector_types;
using libc_types::f32;
using libc_types::i32;
using libc_types::i64;
using libc_types::u32;

namespace {

/* The edges round a surface, in walking order; false if the data loops */
template<typename Visit>
bool walk_edges(
    Span<blam::collision::surface const> surfaces,
    Span<blam::collision::edge const>    edges,
    i32                                  si,
    Visit&&                              visit)
{
    i32 const first = surfaces[si].first_edge;
    i32       e     = first;
    for(u32 n = 0; n < 32; n++)
    {
        if(e < 0 || static_cast<size_t>(e) >= edges.size())
            return false;
        auto const& edge = edges[e];
        visit(e, edge);
        e = edge.left_surface == si ? edge.forward_edge : edge.reverse_edge;
        if(e == first)
            return true;
    }
    return false;
}

Vecf3 vertex_of(Span<blam::collision::vertex const> vertices, i32 i)
{
    return i >= 0 && static_cast<size_t>(i) < vertices.size() ? vertices[i].point : Vecf3{};
}

/* The side of a portal end: left of the way across, or right */
bool left_of(Vecf3 const& from, Vecf3 const& to, Vecf3 const& point)
{
    Vecf2 const dir{to.x - from.x, to.y - from.y};
    Vecf2 const off{point.x - from.x, point.y - from.y};
    return dir.x * off.y - dir.y * off.x >= 0.f;
}

f32 cross2(Vecf3 const& a, Vecf3 const& b, Vecf3 const& c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

} // namespace

void NavMesh::clear()
{
    nodes.clear();
    vertices.clear();
    links.clear();
    m_grid.clear();
}

void NavMesh::build(BSPItem const& item)
{
    clear();
    auto const& surfaces = item.coll_surfaces;
    auto const& edges    = item.coll_edges;
    auto const& verts    = item.coll_vertices;
    if(surfaces.empty() || edges.empty() || verts.empty())
        return;

    /* Every surface's polygon, and which are floors */
    struct polygon_t
    {
        std::vector<i32> vertex;
        std::vector<i32> edge;
        Vecf3            normal{};
        Vecf3            center{};
        f32              z_min{0.f}, z_max{0.f};
    };
    std::vector<polygon_t> polygons(surfaces.size());
    std::vector<i32>       node_of(surfaces.size(), -1);
    for(u32 si = 0; si < surfaces.size(); si++)
    {
        auto& poly = polygons[si];
        bool  ok   = walk_edges(surfaces, edges, static_cast<i32>(si), [&](i32 e, blam::collision::edge const& edge) {
            poly.edge.push_back(e);
            poly.vertex.push_back(edge.left_surface == static_cast<i32>(si) ? edge.start_vertex : edge.end_vertex);
        });
        if(!ok || poly.vertex.size() < 3)
        {
            poly.vertex.clear();
            continue;
        }
        Vecf3 normal{};
        Vecf3 center{};
        poly.z_min = std::numeric_limits<f32>::max();
        poly.z_max = std::numeric_limits<f32>::lowest();
        for(size_t i = 0; i < poly.vertex.size(); i++)
        {
            Vecf3 const a = vertex_of(verts, poly.vertex[i]);
            Vecf3 const b = vertex_of(verts, poly.vertex[(i + 1) % poly.vertex.size()]);
            normal.x += (a.y - b.y) * (a.z + b.z);
            normal.y += (a.z - b.z) * (a.x + b.x);
            normal.z += (a.x - b.x) * (a.y + b.y);
            center += a;
            poly.z_min = std::min(poly.z_min, a.z);
            poly.z_max = std::max(poly.z_max, a.z);
        }
        f32 const len = glm::length(normal);
        if(len < 1e-8f)
        {
            poly.vertex.clear();
            continue;
        }
        poly.normal = normal / len;
        poly.center = center / static_cast<f32>(poly.vertex.size());
        if(poly.normal.z < min_normal_z)
            continue;
        node_of[si] = static_cast<i32>(nodes.size());
        nodes.push_back(node_t{
            .center       = poly.center,
            .normal       = poly.normal,
            .d            = glm::dot(poly.normal, poly.center),
            .first_vertex = static_cast<u32>(vertices.size()),
            .vertex_count = static_cast<u32>(poly.vertex.size()),
        });
        for(i32 v : poly.vertex)
            vertices.push_back(vertex_of(verts, v));
    }

    /* Links, gathered per node then laid out flat */
    std::vector<std::vector<link_t>> per_node(nodes.size());
    auto const link = [&](i32 from_surface, i32 to_surface, Vecf3 const& a, Vecf3 const& b) {
        i32 const from = node_of[from_surface], to = node_of[to_surface];
        if(from < 0 || to < 0 || from == to)
            return;
        auto& out = per_node[from];
        if(std::any_of(out.begin(), out.end(), [to](link_t const& l) { return l.to == static_cast<u32>(to); }))
            return;
        bool const a_left = left_of(nodes[from].center, nodes[to].center, a);
        out.push_back(link_t{.to = static_cast<u32>(to), .left = a_left ? a : b, .right = a_left ? b : a});
    };
    for(auto const& edge : edges)
    {
        if(edge.left_surface < 0 || edge.right_surface < 0 ||
           static_cast<size_t>(edge.left_surface) >= surfaces.size() ||
           static_cast<size_t>(edge.right_surface) >= surfaces.size())
            continue;
        Vecf3 const a = vertex_of(verts, edge.start_vertex), b = vertex_of(verts, edge.end_vertex);
        link(edge.left_surface, edge.right_surface, a, b);
        link(edge.right_surface, edge.left_surface, a, b);
    }
    /* A low riser joins the floors on either side of it */
    for(u32 si = 0; si < surfaces.size(); si++)
    {
        auto const& poly = polygons[si];
        if(node_of[si] >= 0 || poly.vertex.empty() || std::abs(poly.normal.z) > .5f ||
           poly.z_max - poly.z_min > step_height)
            continue;
        struct side_t
        {
            i32   surface;
            Vecf3 a, b;
        };
        std::vector<side_t> floors;
        for(i32 e : poly.edge)
        {
            auto const& edge  = edges[e];
            i32 const   other = edge.left_surface == static_cast<i32>(si) ? edge.right_surface : edge.left_surface;
            if(other >= 0 && static_cast<size_t>(other) < surfaces.size() && node_of[other] >= 0)
                floors.push_back({other, vertex_of(verts, edge.start_vertex), vertex_of(verts, edge.end_vertex)});
        }
        for(auto const& from : floors)
            for(auto const& to : floors)
                if(from.surface != to.surface)
                    link(from.surface, to.surface, to.a, to.b);
    }
    for(u32 n = 0; n < nodes.size(); n++)
    {
        nodes[n].first_link = static_cast<u32>(links.size());
        nodes[n].link_count = static_cast<u32>(per_node[n].size());
        links.insert(links.end(), per_node[n].begin(), per_node[n].end());
    }

    /* The grid, by each node's footprint */
    for(u32 n = 0; n < nodes.size(); n++)
    {
        auto const& node = nodes[n];
        Vecf2       lo{std::numeric_limits<f32>::max()}, hi{std::numeric_limits<f32>::lowest()};
        for(u32 i = 0; i < node.vertex_count; i++)
        {
            auto const& v = vertices[node.first_vertex + i];
            lo            = glm::min(lo, Vecf2{v.x, v.y});
            hi            = glm::max(hi, Vecf2{v.x, v.y});
        }
        for(f32 y = std::floor(lo.y / cell_size); y <= std::floor(hi.y / cell_size); y++)
            for(f32 x = std::floor(lo.x / cell_size); x <= std::floor(hi.x / cell_size); x++)
                m_grid[cell_of(x * cell_size, y * cell_size)].push_back(n);
    }
}

i64 NavMesh::cell_of(f32 x, f32 y)
{
    i64 const cx = static_cast<i64>(std::floor(x / cell_size));
    i64 const cy = static_cast<i64>(std::floor(y / cell_size));
    return (cx << 32) ^ (cy & 0xffffffff);
}

std::vector<u32> const* NavMesh::cell(i64 key) const
{
    auto it = m_grid.find(key);
    return it != m_grid.end() ? &it->second : nullptr;
}

bool NavMesh::contains(node_t const& node, Vecf2 const& at) const
{
    bool inside = false;
    for(u32 i = 0, j = node.vertex_count - 1; i < node.vertex_count; j = i++)
    {
        auto const& a = vertices[node.first_vertex + i];
        auto const& b = vertices[node.first_vertex + j];
        if((a.y > at.y) != (b.y > at.y) && at.x < (b.x - a.x) * (at.y - a.y) / (b.y - a.y) + a.x)
            inside = !inside;
    }
    return inside;
}

f32 NavMesh::height_at(node_t const& node, Vecf2 const& at) const
{
    return (node.d - node.normal.x * at.x - node.normal.y * at.y) / node.normal.z;
}

std::optional<u32> NavMesh::locate(Vecf3 const& feet) const
{
    auto const*        here = cell(cell_of(feet.x, feet.y));
    if(!here)
        return std::nullopt;
    Vecf2 const        at{feet.x, feet.y};
    std::optional<u32> best;
    f32                nearest = std::numeric_limits<f32>::max();
    for(u32 n : *here)
    {
        auto const& node = nodes[n];
        if(!contains(node, at))
            continue;
        f32 const dz = feet.z - height_at(node, at);
        if(dz < -feet_below || dz > feet_above)
            continue;
        if(std::abs(dz) < nearest)
        {
            nearest = std::abs(dz);
            best    = n;
        }
    }
    return best;
}

std::vector<Vecf3> NavMesh::route(Vecf3 const& from, Vecf3 const& to, u32 max_expansions) const
{
    auto const start = locate(from);
    auto const goal  = locate(to);
    if(!start || !goal)
        return {};
    if(*start == *goal)
        return {to};

    /* A* over the nodes, each reached at the middle of the edge crossed */
    struct open_t
    {
        f32 f;
        u32 node;
        bool operator>(open_t const& o) const
        {
            return f > o.f;
        }
    };
    std::priority_queue<open_t, std::vector<open_t>, std::greater<>> open;
    std::unordered_map<u32, f32>                                     cost;
    std::unordered_map<u32, Vecf3>                                   at;
    std::unordered_map<u32, std::pair<u32, u32>>                     came; /* node: link into it, from */
    cost[*start] = 0.f;
    at[*start]   = from;
    open.push({glm::distance(from, to), *start});
    bool found = false;
    for(u32 expanded = 0; !open.empty() && expanded < max_expansions; expanded++)
    {
        auto const [f, node] = open.top();
        open.pop();
        if(node == *goal)
        {
            found = true;
            break;
        }
        auto const& n = nodes[node];
        for(u32 l = n.first_link; l < n.first_link + n.link_count; l++)
        {
            auto const& link = links[l];
            Vecf3 const mid  = (link.left + link.right) * .5f;
            f32 const   g    = cost[node] + glm::distance(at[node], mid);
            if(auto it = cost.find(link.to); it != cost.end() && it->second <= g)
                continue;
            cost[link.to] = g;
            at[link.to]   = mid;
            came[link.to] = {l, node};
            open.push({g + glm::distance(mid, to), link.to});
        }
    }
    if(!found)
        return {};

    /* The edges crossed, start to goal, pulled in from their ends by a
     * body's width */
    std::vector<std::pair<Vecf3, Vecf3>> portals;
    for(u32 node = *goal; node != *start;)
    {
        auto const& [l, previous] = came.at(node);
        auto const& link          = links[l];
        Vecf3 left = link.left, right = link.right;
        f32 const   width = glm::distance(left, right);
        if(width > body_radius * 2.f)
        {
            Vecf3 const in = (right - left) / width * body_radius;
            left += in;
            right -= in;
        } else
            left = right = (left + right) * .5f;
        portals.emplace_back(left, right);
        node = previous;
    }
    std::reverse(portals.begin(), portals.end());
    portals.emplace_back(to, to);

    /* The funnel: corners where the way bends round a portal's end */
    std::vector<Vecf3> out;
    Vecf3              apex = from, left = from, right = from;
    size_t             apex_i = 0, left_i = 0, right_i = 0;
    for(size_t i = 0; i < portals.size(); i++)
    {
        auto const& [pl, pr] = portals[i];
        if(cross2(apex, right, pr) <= 0.f)
        {
            if(glm::distance(Vecf2(apex), Vecf2(right)) < 1e-4f || cross2(apex, left, pr) > 0.f)
            {
                right   = pr;
                right_i = i;
            } else
            {
                out.push_back(left);
                apex = left;
                apex_i = left_i;
                left = right = apex;
                left_i = right_i = apex_i;
                i                = apex_i;
                continue;
            }
        }
        if(cross2(apex, left, pl) >= 0.f)
        {
            if(glm::distance(Vecf2(apex), Vecf2(left)) < 1e-4f || cross2(apex, right, pl) < 0.f)
            {
                left   = pl;
                left_i = i;
            } else
            {
                out.push_back(right);
                apex = right;
                apex_i = right_i;
                left = right = apex;
                left_i = right_i = apex_i;
                i                = apex_i;
                continue;
            }
        }
    }
    if(out.empty() || glm::distance(out.back(), to) > 1e-3f)
        out.push_back(to);
    return out;
}
