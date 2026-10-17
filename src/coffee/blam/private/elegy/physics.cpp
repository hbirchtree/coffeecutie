#include <blam/elegy/physics.h>

#include <cmath>

namespace blam::elegy::phys {

namespace {

struct walker_t
{
    tag_magic const&           magic;
    Span<sphere_t const>       spheres;
    Span<multi_sphere_t const> multi_spheres;
    Span<pill_t const>         pills;
    Span<box_t const>          boxes;
    Span<triangle_t const>     triangles;
    Span<polyhedron_t const>   polyhedra;
    Span<four_vectors_t const> four_vectors;
    Span<list_t const>         lists;
    Span<list_shape_t const>   list_shapes;
    Span<mopp_t const>         mopps;
    /* Where each polyhedron's and list's run starts */
    std::vector<size_t> polyhedron_first;
    std::vector<size_t> list_first;
    std::vector<hull_t> out;

    walker_t(physics_model_t const& model, tag_magic const& magic_)
        : magic(magic_)
    {
        auto get = [this](auto const& block) {
            using T = typename std::decay_t<decltype(block)>::value_type;
            return block.data(magic).value_or(Span<T const>());
        };
        spheres       = get(model.spheres);
        multi_spheres = get(model.multi_spheres);
        pills         = get(model.pills);
        boxes         = get(model.boxes);
        triangles     = get(model.triangles);
        polyhedra     = get(model.polyhedra);
        four_vectors  = get(model.four_vectors);
        lists         = get(model.lists);
        list_shapes   = get(model.list_shapes);
        mopps         = get(model.mopps);
        size_t next   = 0;
        for(auto const& poly : polyhedra)
        {
            polyhedron_first.push_back(next);
            next += static_cast<size_t>(std::max(poly.four_vectors_size.value(), 0));
        }
        next = 0;
        for(auto const& list : lists)
        {
            list_first.push_back(next);
            next += static_cast<size_t>(std::max(list.child_count.value(), 0));
        }
    }

    hull_t& start(shape_base_t const& base)
    {
        out.push_back(hull_t{
            .material    = base.material,
            .friction    = base.friction,
            .restitution = base.restitution,
            .mass        = base.mass,
            .radius      = base.radius,
        });
        return out.back();
    }

    /* Points on a sphere, enough for a hull to stand in for it */
    static void sphere_points(std::vector<Vecf3>& points, Vecf3 const& center, f32 radius)
    {
        constexpr int rings = 4, segments = 8;
        points.push_back(center + Vecf3(0, 0, radius));
        points.push_back(center - Vecf3(0, 0, radius));
        for(int r = 1; r < rings; r++)
        {
            f32 const theta = glm::pi<f32>() * r / rings;
            for(int s = 0; s < segments; s++)
            {
                f32 const phi = glm::two_pi<f32>() * s / segments;
                points.push_back(
                    center + radius * Vecf3(std::sin(theta) * std::cos(phi),
                                            std::sin(theta) * std::sin(phi),
                                            std::cos(theta)));
            }
        }
    }

    void walk(i16 type, i16 index, u32 depth = 0)
    {
        if(index < 0 || depth > 8)
            return;
        auto const i = static_cast<size_t>(index);
        switch(static_cast<shape_type_t>(type))
        {
        case shape_type_t::sphere:
            if(i < spheres.size())
            {
                auto const& s = spheres[i];
                sphere_points(start(s.base).points, Vecf3(s.translation.value()), s.base.radius);
            }
            break;
        case shape_type_t::multi_sphere:
            if(i < multi_spheres.size())
            {
                auto const& s     = multi_spheres[i];
                auto&       hull  = start(s.base);
                /* The radius field holds the sphere count here */
                i32 const   count = std::min<i32>(
                    std::bit_cast<be<i32>>(s.base.radius).value(), s.spheres.size());
                for(i32 k = 0; k < count; k++)
                {
                    Vecf4 const sphere = s.spheres[static_cast<size_t>(k)];
                    sphere_points(hull.points, Vecf3(sphere), sphere.w);
                }
            }
            break;
        case shape_type_t::pill:
            if(i < pills.size())
            {
                auto const& p    = pills[i];
                auto&       hull = start(p.base);
                sphere_points(hull.points, Vecf3(p.bottom.value()), p.base.radius);
                sphere_points(hull.points, Vecf3(p.top.value()), p.base.radius);
            }
            break;
        case shape_type_t::box:
            if(i < boxes.size())
            {
                auto const& b    = boxes[i];
                auto&       hull = start(b.base);
                /* The convex radius is a shell around the half extents */
                Vecf3 const half = Vecf3(b.half_extents.value()) + Vecf3(b.base.radius.value());
                Vecf3 const ri   = Vecf3(b.rotation[0].value());
                Vecf3 const rj   = Vecf3(b.rotation[1].value());
                Vecf3 const rk   = Vecf3(b.rotation[2].value());
                Vecf3 const t    = Vecf3(b.translation.value());
                for(int c = 0; c < 8; c++)
                {
                    Vecf3 const local{
                        c & 1 ? half.x : -half.x, c & 2 ? half.y : -half.y, c & 4 ? half.z : -half.z};
                    hull.points.push_back(t + ri * local.x + rj * local.y + rk * local.z);
                }
            }
            break;
        case shape_type_t::triangle:
            if(i < triangles.size())
            {
                auto const& tri  = triangles[i];
                auto&       hull = start(tri.base);
                Vecf3 const extrusion = Vecf3(tri.extrusion.value());
                for(auto const& p : tri.points)
                {
                    hull.points.push_back(Vecf3(p.value()));
                    hull.points.push_back(Vecf3(p.value()) + extrusion);
                }
            }
            break;
        case shape_type_t::polyhedron:
            if(i < polyhedra.size())
            {
                auto const&  poly  = polyhedra[i];
                auto&        hull  = start(poly.base);
                size_t const first = polyhedron_first[i];
                i32 const    count = poly.vertex_count;
                for(i32 k = 0; k < count; k++)
                {
                    size_t const fv = first + static_cast<size_t>(k) / 4;
                    if(fv >= four_vectors.size())
                        break;
                    auto const   lane = static_cast<glm::length_t>(k % 4);
                    auto const&  f    = four_vectors[fv];
                    hull.points.push_back(
                        {f.x.value()[lane], f.y.value()[lane], f.z.value()[lane]});
                }
            }
            break;
        case shape_type_t::list:
            if(i < lists.size())
            {
                size_t const first = list_first[i];
                size_t const count = static_cast<size_t>(std::max(lists[i].child_count.value(), 0));
                for(size_t k = first; k < first + count && k < list_shapes.size(); k++)
                    walk(list_shapes[k].type, list_shapes[k].index, depth + 1);
            }
            break;
        case shape_type_t::mopp:
            if(i < mopps.size())
                walk(mopps[i].child_type, mopps[i].child_index, depth + 1);
            break;
        default:
            break;
        }
    }
};

} // namespace

std::vector<hull_t> hulls_of(
    physics_model_t const& model, tag_magic const& magic, rigid_body_t const& body)
{
    walker_t walker(model, magic);
    walker.walk(body.shape_type, body.shape_index);
    std::erase_if(walker.out, [](hull_t const& hull) { return hull.points.empty(); });
    return std::move(walker.out);
}

} // namespace blam::elegy::phys
