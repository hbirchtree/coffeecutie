#include "hit_model.h"

#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <bit>
#include <map>

namespace poc {
namespace {

/* One surface's vertex loop, walked like load_collision_debug does */
template<typename Edges, typename Verts>
void surface_loop(
    Edges const&       edges,
    Verts const&       verts,
    std::int32_t       si,
    std::int32_t       first,
    std::vector<vec3>& out)
{
    out.clear();
    std::int32_t ed    = first;
    std::uint32_t guard = 0;
    do
    {
        if(ed < 0 || static_cast<size_t>(ed) >= edges.size())
            break;
        auto const&   edge = edges[ed];
        std::int32_t  vi;
        if(edge.left_surface == si)
        {
            vi = edge.start_vertex;
            ed = edge.forward_edge;
        } else
        {
            vi = edge.end_vertex;
            ed = edge.reverse_edge;
        }
        if(vi >= 0 && static_cast<size_t>(vi) < verts.size())
            out.push_back(verts[vi].point);
    } while(ed != first && ++guard < 64);
}

/* The raycast resolves a BSP plane, not a surface. Surfaces lying on that
 * plane are tested for containing the hit point; the one that does names the
 * material. */
std::int16_t surface_material(
    blam::collision::model_bsp const& bsp,
    blam::map_ptr const&              magic,
    std::int32_t                plane,
    vec3 const&                 point,
    std::int16_t                fallback)
{
    auto surfs_r  = bsp.surfaces.data(magic);
    auto edges_r  = bsp.edges.data(magic);
    auto verts_r  = bsp.vertices.data(magic);
    auto planes_r = bsp.planes.data(magic);
    if(surfs_r.has_error() || edges_r.has_error() || verts_r.has_error() ||
       planes_r.has_error())
        return fallback;
    auto surfs  = surfs_r.value();
    auto edges  = edges_r.value();
    auto verts  = verts_r.value();
    auto planes = planes_r.value();

    std::vector<vec3> loop;
    std::int16_t      on_plane = -1;
    for(std::int32_t si = 0; static_cast<size_t>(si) < surfs.size(); si++)
    {
        auto const&  s  = surfs[si];
        std::int32_t pi = s.plane & 0x7fffffff;
        if(pi != plane || static_cast<size_t>(pi) >= planes.size())
            continue;
        if(on_plane < 0)
            on_plane = s.material;
        surface_loop(edges, verts, si, s.first_edge, loop);
        if(loop.size() < 3)
            continue;
        vec3 const n    = planes[pi].plane;
        int        sign = 0;
        bool       in   = true;
        for(size_t i = 0; i < loop.size() && in; i++)
        {
            vec3 const& a = loop[i];
            vec3 const& b = loop[(i + 1) % loop.size()];
            float const d = glm::dot(glm::cross(b - a, point - a), n);
            if(std::abs(d) < 1e-6f)
                continue;
            int const sd = d > 0.f ? 1 : -1;
            if(sign == 0)
                sign = sd;
            else if(sd != sign)
                in = false;
        }
        if(in)
            return s.material;
    }
    return on_plane >= 0 ? on_plane : fallback;
}

/* Segment vs sphere, conservative */
bool segment_near_sphere(vec3 const& a, vec3 const& b, vec3 const& c, float r)
{
    vec3 const  ab = b - a;
    float const l2 = glm::dot(ab, ab);
    float       t  = l2 > 0.f ? glm::dot(c - a, ab) / l2 : 0.f;
    t              = std::clamp(t, 0.f, 1.f);
    vec3 const  p  = a + ab * t;
    return glm::dot(p - c, p - c) <= r * r;
}

float read_f32(std::uint32_t v)
{
    return std::bit_cast<float>(v);
}

} // namespace

mat4 HitModel::root(BipedPose const& pose)
{
    return glm::rotate(
        glm::translate(mat4(1.f), pose.pos), pose.yaw, vec3(0.f, 0.f, 1.f));
}

std::optional<HitModel::Hit> HitModel::raycast(
    BipedPose const& pose, vec3 const& start, vec3 const& end) const
{
    mat4 const root_m = root(pose);
    mat4 const inv_root = glm::inverse(root_m);
    vec3 const ls       = vec3(inv_root * glm::vec4(start, 1.f));
    vec3 const le       = vec3(inv_root * glm::vec4(end, 1.f));
    if(!segment_near_sphere(ls, le, bound_center, bound_radius))
        return std::nullopt;

    std::optional<Hit> best;
    for(size_t ni = 0; ni < nodes.size(); ni++)
    {
        auto const& node = nodes[ni];
        if(node.bsps.empty())
            continue;
        vec3 const ns = vec3(node.inv_bind * glm::vec4(ls, 1.f));
        vec3 const ne = vec3(node.inv_bind * glm::vec4(le, 1.f));
        if(!segment_near_sphere(ns, ne, node.center, node.radius))
            continue;
        for(auto const* bsp : node.bsps)
        {
            auto hit = bsp->raycast(ns, ne, magic);
            if(!hit || (best && hit->t >= best->t))
                continue;
            vec3 const local = ns + (ne - ns) * hit->t;
            mat4 const to_world = root_m * node.bind;
            best = Hit{
                .t      = hit->t,
                .node   = static_cast<std::int16_t>(ni),
                .material =
                    surface_material(*bsp, magic, hit->plane, local, node.material),
                .normal = glm::normalize(vec3(to_world * glm::vec4(hit->normal, 0.f))),
                .local  = local,
            };
        }
    }
    return best;
}

vec3 HitModel::node_to_world(
    BipedPose const& pose, std::int16_t node, vec3 local) const
{
    if(node < 0 || static_cast<size_t>(node) >= nodes.size())
        return pose.pos;
    return vec3(root(pose) * nodes[node].bind * glm::vec4(local, 1.f));
}

void HitModel::finalize()
{
    std::vector<vec3> verts_cache;
    bound_center = vec3(0.f);
    bound_radius = 0.f;
    std::vector<std::pair<vec3, float>> spheres;
    float head_weight = 0.f, body_weight = 0.f;
    head_center = body_center = vec3(0.f);

    for(auto& node : nodes)
    {
        vec3                        lo(1e9f), hi(-1e9f);
        std::map<std::int16_t, int> mat_count;
        for(auto const* bsp : node.bsps)
        {
            if(auto v = bsp->vertices.data(magic); v.has_value())
                for(auto const& vert : v.value())
                {
                    lo = glm::min(lo, vert.point);
                    hi = glm::max(hi, vert.point);
                }
            if(auto s = bsp->surfaces.data(magic); s.has_value())
                for(auto const& surf : s.value())
                    mat_count[surf.material]++;
        }
        if(node.bsps.empty() || lo.x > hi.x)
            continue;
        node.lo     = lo;
        node.hi     = hi;
        node.center = (lo + hi) * 0.5f;
        node.radius = glm::length(hi - lo) * 0.5f + 0.01f;
        int most    = 0;
        for(auto [m, c] : mat_count)
            if(c > most)
            {
                most          = c;
                node.material = m;
            }

        vec3 const  c = vec3(node.bind * glm::vec4(node.center, 1.f));
        float const w = node.radius * node.radius * node.radius;
        spheres.emplace_back(c, node.radius);
        auto const* mat = material(node.material);
        if(mat && mat->head)
        {
            head_center += c * w;
            head_weight += w;
        } else
        {
            body_center += c * w;
            body_weight += w;
        }
    }

    if(spheres.empty())
        return;
    vec3 lo(1e9f), hi(-1e9f);
    for(auto const& [c, r] : spheres)
    {
        lo = glm::min(lo, c - r);
        hi = glm::max(hi, c + r);
    }
    bound_center = (lo + hi) * 0.5f;
    for(auto const& [c, r] : spheres)
        bound_radius =
            std::max(bound_radius, glm::length(c - bound_center) + r);
    if(head_weight > 0.f)
        head_center /= head_weight;
    if(body_weight > 0.f)
        body_center /= body_weight;
}

void HitModel::dump() const
{
    fmt::print(
        "hit model: {} nodes, {} materials, body {} shield {}\n",
        nodes.size(),
        materials.size(),
        max_body,
        max_shield);
    for(size_t i = 0; i < materials.size(); i++)
    {
        auto const& m = materials[i];
        fmt::print(
            "  material[{}] {:<16} head={} shield_mult={:.2f} body_mult={:.2f}\n",
            i,
            m.name,
            m.head,
            m.shield_mult,
            m.body_mult);
    }
    for(size_t i = 0; i < nodes.size(); i++)
    {
        auto const& n = nodes[i];
        vec3 const  c = vec3(n.bind * glm::vec4(n.center, 1.f));
        fmt::print(
            "  node[{:2}] {:<22} parent={:<3} bsps={} material={:<3} "
            "center=({:.3f}, {:.3f}, {:.3f}) r={:.3f}\n",
            i,
            n.name,
            n.parent,
            n.bsps.size(),
            n.material,
            c.x,
            c.y,
            c.z,
            n.radius);
    }
    fmt::print(
        "  bound=({:.3f}, {:.3f}, {:.3f}) r={:.3f} head=({:.3f}, {:.3f}, "
        "{:.3f}) body=({:.3f}, {:.3f}, {:.3f})\n",
        bound_center.x,
        bound_center.y,
        bound_center.z,
        bound_radius,
        head_center.x,
        head_center.y,
        head_center.z,
        body_center.x,
        body_center.y,
        body_center.z);
}

std::optional<HitModel> build_hit_model(
    blam::coll::header const& coll,
    blam::mod2::bone const*   bones,
    size_t                    bone_count,
    blam::map_ptr const&      magic)
{
    HitModel model;
    model.magic = magic;

    /* Model-wide vitality: maximum_body_vitality right after the indirect
     * damage material, maximum_shield_vitality at header offset 0xCC. */
    float const body   = read_f32(coll.body_and_shield[0]);
    float const shield = read_f32(coll.body_and_shield[(0xCC - 8) / 4]);
    if(body > 0.f && body < 10000.f)
        model.max_body = body;
    if(shield >= 0.f && shield < 10000.f)
        model.max_shield = shield;

    if(auto mats = coll.materials.data(magic); mats.has_value())
        for(auto const& m : mats.value())
            model.materials.push_back(HitMaterial{
                .name        = std::string(m.name.str()),
                .head        = (m.flags & blam::coll::material::head) != 0,
                .shield_mult = m.shield_damage_multiplier,
                .body_mult   = m.body_damage_multiplier,
            });

    /* Bind pose, same convention as caching.cpp's inv_bind */
    std::vector<mat4> world_bind(bone_count, mat4(1.f));
    for(size_t i = 0; i < bone_count; i++)
    {
        auto const& b     = bones[i];
        mat4 const  local = glm::translate(mat4(1.f), vec3(b.translation)) *
                           glm::mat4_cast(glm::conjugate(b.rotation));
        if(b.parent != blam::mod2::bone::invalid_bone && b.parent < i)
            world_bind[i] = world_bind[b.parent] * local;
        else
            world_bind[i] = local;
    }

    auto nodes = coll.nodes.data(magic);
    if(nodes.has_error())
        return std::nullopt;
    for(auto const& n : nodes.value())
    {
        HitNode node;
        node.name   = std::string(n.name.str());
        node.parent = n.parent;
        for(size_t i = 0; i < bone_count; i++)
            if(bones[i].name.str() == n.name.str())
            {
                node.bind = world_bind[i];
                break;
            }
        node.inv_bind = glm::inverse(node.bind);
        if(auto bsps = n.bsps.data(magic); bsps.has_value())
            for(auto const& b : bsps.value())
                node.bsps.push_back(&b);
        model.nodes.push_back(std::move(node));
    }
    model.finalize();
    return model;
}

std::optional<vec3> Terrain::ground(vec3 const& from, float depth) const
{
    vec3 const end = from - vec3(0.f, 0.f, depth);
    auto       hit = raycast(from, end);
    if(!hit)
        return std::nullopt;
    return from + (end - from) * hit->t;
}

} // namespace poc
