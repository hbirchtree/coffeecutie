#pragma once

/* Decoded dumps of Halo 2 tags, the dimeter counterpart of tag-inspect:
 * answer format questions from the tag data instead of from a frame. */

#include <blam/dimeter/bitm.h>
#include <blam/dimeter/geometry.h>
#include <blam/dimeter/lightmap.h>
#include <blam/dimeter/map.h>
#include <blam/dimeter/material.h>
#include <blam/dimeter/scenario.h>
#include <blam/dimeter/shader.h>

#include <fmt_extensions/format.h>
#include <peripherals/stl/enumerate.h>

#include <cmath>
#include <string>
#include <string_view>

namespace dimeter_info {

namespace h2  = blam::dimeter;
namespace geo = blam::dimeter::geo;
using blam::from_le;
using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using libc_types::u8;

template<typename V>
inline std::string tag_label(
    h2::map_container<V> const& map, h2::datum_index id)
{
    auto const* tag = map.tag_at(id);
    if(!tag)
        return id.valid() ? fmt::format("<bad tag {:#x}>", from_le(id.raw))
                          : "<none>";
    return fmt::format("{} [{}]", map.name_of(*tag), tag->tag_class.str());
}

template<typename V>
inline std::string tag_label(
    h2::map_container<V> const& map, h2::tag_reference<> const& ref)
{
    return tag_label(map, ref.tag_id);
}

/*! Groups inherit: scen/bloc/vehi/... all derive from obje */
template<typename V>
inline bool derives_from(
    h2::map_container<V> const& map,
    h2::tag_class const&        tag_class,
    std::string_view            base)
{
    if(tag_class.str() == base)
        return true;
    for(auto const& group : map.groups())
        if(group.group == tag_class)
            return group.parent.str() == base ||
                   group.grandparent.str() == base;
    return false;
}

inline const char* raw_file_name(h2::raw_ref ref)
{
    switch(ref.file())
    {
    case h2::raw_file_t::local:
        return "local";
    case h2::raw_file_t::mainmenu:
        return "mainmenu";
    case h2::raw_file_t::shared:
        return "shared";
    case h2::raw_file_t::single_player_shared:
        return "sp_shared";
    }
    return "?";
}

template<typename V>
inline void dump_bitm(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* bitm = tag.template data<h2::bitm::header_t>(map.magic());
    if(!bitm)
        return;
    fmt::print(
        "  type={} format={} usage={} flags={:#x} mipmaps={} sprites={}\n",
        from_le(bitm->type),
        from_le(bitm->format),
        from_le(bitm->usage),
        from_le(bitm->flags),
        from_le(bitm->mipmap_count),
        from_le(bitm->sprite_budget_count));
    auto sequences = bitm->sequences.data(map.magic());
    if(!sequences.has_error())
        for(auto const& seq : sequences.value())
            fmt::print(
                "  sequence '{}' first={} count={}\n",
                seq.name.str(),
                from_le(seq.first_bitmap),
                from_le(seq.bitmap_count));
    auto images = bitm->images.data(map.magic());
    if(images.has_error())
        return;
    for(auto const& [i, img] : stl_types::enumerate(images.value()))
    {
        fmt::print(
            "  image {}: {}x{}x{} type={} format={} flags={:#x} mipmaps={}\n",
            i,
            from_le(img.width),
            from_le(img.height),
            img.depth,
            static_cast<u16>(from_le(img.type)),
            static_cast<u16>(from_le(img.format)),
            from_le(static_cast<u16>(img.flags)),
            from_le(img.mipmaps));
        for(int lod = 0; lod < 3; lod++)
            if(img.lod_offset[lod].valid())
                fmt::print(
                    "    lod{} {}+{:#x} {} bytes\n",
                    lod + 1,
                    raw_file_name(img.lod_offset[lod]),
                    img.lod_offset[lod].offset(),
                    from_le(img.lod_size[lod]));
        if(from_le(img.lowres_size))
            fmt::print(
                "    lowres header+{:#x} {} bytes format={}\n",
                from_le(img.lowres_offset),
                from_le(img.lowres_size),
                static_cast<u16>(from_le(img.lowres_format)));
    }
}

template<typename V>
inline void dump_stem(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* stem = tag.template data<h2::stem::header_t>(map.magic());
    if(!stem)
        return;
    auto props = stem->properties.data(map.magic());
    if(props.has_error())
        return;
    for(auto const& tp : props.value())
    {
        auto passes = tp.passes.data(map.magic());
        auto impls  = tp.implementations.data(map.magic());
        auto remaps = tp.remappings.data(map.magic());
        if(passes.has_error() || impls.has_error() || remaps.has_error())
            continue;
        fmt::print(
            "  {} passes, {} implementations, {} remappings\n",
            passes.value().size(),
            impls.value().size(),
            remaps.value().size());
        for(auto const& pass : passes.value())
        {
            auto range = pass.implementations();
            fmt::print(
                "  pass {} impl {}+{}{}\n",
                tag_label(map, pass.pass_id()),
                range.index(),
                range.count(),
                h2::passes::find([&] {
                    auto const* spas = map.tag_at(pass.pass_id());
                    auto        name = spas ? map.name_of(*spas) : std::string_view{};
                    if(auto slash = name.find_last_of('\\'); slash != name.npos)
                        name = name.substr(slash + 1);
                    return name;
                }())
                    ? ""
                    : " (no pass definition)");
            for(u32 i = 0; i < range.count(); i++)
            {
                size_t ii = static_cast<size_t>(range.index()) + i;
                if(ii >= impls.value().size())
                    break;
                auto const& impl = impls.value()[ii];
                auto        show = [&](const char* what, h2::stem::block_range_t r) {
                    fmt::print("      {} ->", what);
                    for(u32 k = 0; k < r.count(); k++)
                    {
                        size_t ri = static_cast<size_t>(r.index()) + k;
                        if(ri >= remaps.value().size())
                            break;
                        fmt::print(" {}", remaps.value()[ri].block_index);
                    }
                    fmt::print("\n");
                };
                fmt::print("    implementation {}:\n", ii);
                show("bitmaps", impl.bitmaps);
                show("pixel constants", impl.pixel_constants);
                show("vertex constants", impl.vertex_constants);
            }
        }
    }
}

template<typename V>
inline void dump_shad(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* shad = tag.template data<geo::shad_header_t>(map.magic());
    if(!shad)
        return;
    fmt::print(
        "  template {} material={} flags={:#x}\n",
        tag_label(map, shad->shader_template),
        map.string(shad->material_name),
        from_le(shad->flags));
    if(auto rt = shad->runtime_properties.data(map.magic()); !rt.has_error())
        for(auto const& r : rt.value())
            fmt::print(
                "  runtime diffuse={} illum={}\n",
                tag_label(map, r.diffuse),
                tag_label(map, r.lightmap_emissive));
    auto resolved = h2::material::resolve(map, *shad);
    auto props    = shad->properties.data(map.magic());
    if(props.has_error())
        return;
    for(auto const& p : props.value())
    {
        auto bitmaps = p.bitmaps.data(map.magic());
        auto tiling  = p.tiling.data(map.magic());
        if(bitmaps.has_error() || tiling.has_error())
            continue;
        for(auto const& [i, b] : stl_types::enumerate(bitmaps.value()))
            fmt::print(
                "  bitmap {}: {} usage={}\n",
                i,
                tag_label(map, b.bitmap.id),
                i < resolved.usages.size() && !resolved.usages[i].empty()
                    ? resolved.usages[i]
                    : "?");
        for(auto const& [i, t] : stl_types::enumerate(tiling.value()))
            fmt::print(
                "  tiling {}: ({}, {}, {}, {}) argument={}\n",
                i,
                from_le(t.x),
                from_le(t.y),
                from_le(t.z),
                from_le(t.w),
                i < resolved.arguments.size() && !resolved.arguments[i].empty()
                    ? resolved.arguments[i]
                    : "?");
    }
    for(auto const& [i, c] : stl_types::enumerate(resolved.pixel_constants))
        fmt::print(
            "  pixel constant {}: r={} g={} b={} a={}\n", i, c[0], c[1], c[2], c[3]);
    fmt::print("  passes:\n");
    for(auto const& [i, name] : stl_types::enumerate(resolved.pass_names))
    {
        fmt::print("    {}", name);
        if(i < resolved.pass_pixel_slots.size() &&
           !resolved.pass_pixel_slots[i].empty())
        {
            fmt::print(" pixel constants");
            for(auto slot : resolved.pass_pixel_slots[i])
                fmt::print(" {}", slot);
        }
        fmt::print("\n");
    }
    if(!resolved.unknown_passes.empty())
    {
        fmt::print("  no pass definition for:");
        for(auto name : resolved.unknown_passes)
            fmt::print(" {}", name);
        fmt::print("\n");
    }
    fmt::print(
        "  base: {} via {} scale ({}, {}){}{}{}\n",
        resolved.bitmap ? std::string(map.name_of(*resolved.bitmap)) : "<none>",
        resolved.usage.empty() ? "first slot" : resolved.usage,
        resolved.scale_u,
        resolved.scale_v,
        resolved.alpha_test ? " alpha_test" : "",
        resolved.blend_mode.empty() ? "" : " blend=",
        resolved.blend_mode);
}

template<typename V, h2::heap_t Heap>
inline void dump_resources(
    h2::map_container<V> const&,
    h2::magic_ptr_base<Heap> const&            magic,
    h2::tag_block<geo::resource_t, Heap> const& resources)
{
    auto res = resources.data(magic);
    if(res.has_error())
        return;
    for(auto const& r : res.value())
        fmt::print(
            "      resource type=({}, {}) size={} offset={}\n",
            r.type0(),
            r.type1(),
            r.size(),
            r.offset());
}

template<typename V>
inline void dump_mode(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* model = tag.template data<geo::model_header_t>(map.magic());
    if(!model)
        return;
    fmt::print("  name={} flags={:#x}\n", map.string(model->name), from_le(model->flags));
    if(auto bounds = model->bounds.data(map.magic()); !bounds.has_error())
        for(auto const& b : bounds.value())
            fmt::print(
                "  bounds x[{}, {}] y[{}, {}] z[{}, {}] u[{}, {}] v[{}, {}]\n",
                from_le(b.x_min),
                from_le(b.x_max),
                from_le(b.y_min),
                from_le(b.y_max),
                from_le(b.z_min),
                from_le(b.z_max),
                from_le(b.u_min),
                from_le(b.u_max),
                from_le(b.v_min),
                from_le(b.v_max));
    if(auto regions = model->regions.data(map.magic()); !regions.has_error())
        for(auto const& region : regions.value())
        {
            fmt::print("  region {}\n", map.string(region.name));
            if(auto perms = region.permutations.data(map.magic());
               !perms.has_error())
                for(auto const& perm : perms.value())
                    fmt::print(
                        "    permutation {} sections L1..L6 = {} {} {} {} {} {}\n",
                        map.string(perm.name),
                        perm.lod_section(0),
                        perm.lod_section(1),
                        perm.lod_section(2),
                        perm.lod_section(3),
                        perm.lod_section(4),
                        perm.lod_section(5));
        }
    if(auto sections = model->sections.data(map.magic()); !sections.has_error())
        for(auto const& [i, s] : stl_types::enumerate(sections.value()))
        {
            fmt::print(
                "  section {}: class={} vertices={} faces={} nodes/vertex={} "
                "rigid node={} data {}+{:#x} {} bytes base={}\n",
                i,
                static_cast<i16>(from_le(s.classification)),
                s.vertex_count(),
                s.face_count(),
                s.nodes_per_vertex,
                s.rigid_node.value(),
                raw_file_name(s.data),
                s.data.offset(),
                s.data_size(),
                s.base_address());
            dump_resources(map, map.magic(), s.resources);
        }
    if(auto nodes = model->nodes.data(map.magic()); !nodes.has_error())
        for(auto const& [i, n] : stl_types::enumerate(nodes.value()))
            fmt::print(
                "  node {} {} parent={} pos=({}, {}, {}) rot=({}, {}, {}, {}) "
                "inv_scale={}\n",
                i,
                map.string(n.name),
                n.parent.value(),
                from_le(n.position[0]),
                from_le(n.position[1]),
                from_le(n.position[2]),
                from_le(n.rotation[0]),
                from_le(n.rotation[1]),
                from_le(n.rotation[2]),
                from_le(n.rotation[3]),
                from_le(n.inverse_scale));
    if(auto shaders = model->shaders.data(map.magic()); !shaders.has_error())
        for(auto const& [i, s] : stl_types::enumerate(shaders.value()))
            fmt::print("  shader {}: {}\n", i, tag_label(map, s.shader));
}

template<typename V, h2::heap_t Heap>
inline void dump_collision(
    h2::map_container<V> const&,
    h2::magic_ptr_base<Heap> const& magic,
    h2::coll::bsp_t<Heap> const&    bsp,
    const char*                     indent)
{
    auto count = [&](auto const& block) -> size_t {
        auto data = block.data(magic);
        return data.has_error() ? 0 : data.value().size();
    };
    size_t finite = 0, two_sided = 0, invisible = 0, invalid = 0, breakable = 0,
           climbable = 0, open_loops = 0;
    if(auto vertices = bsp.vertices.data(magic); !vertices.has_error())
        for(auto const& v : vertices.value())
            finite += std::isfinite(from_le(v.point[0])) &&
                      std::isfinite(from_le(v.point[1])) &&
                      std::isfinite(from_le(v.point[2]));
    if(auto surfaces = bsp.surfaces.data(magic); !surfaces.has_error())
        for(auto const& s : surfaces.value())
        {
            two_sided += s.has(h2::coll::surface_flags_t::two_sided);
            invisible += s.has(h2::coll::surface_flags_t::invisible);
            invalid += s.has(h2::coll::surface_flags_t::invalid);
            breakable += s.has(h2::coll::surface_flags_t::breakable);
            climbable += s.has(h2::coll::surface_flags_t::climbable);
        }
    if(auto surfaces = bsp.surfaces.data(magic), edges = bsp.edges.data(magic);
       !surfaces.has_error() && !edges.has_error())
        for(u32 si = 0; si < surfaces.value().size(); si++)
            open_loops += h2::coll::walk_surface(
                              surfaces.value(), edges.value(), si, [](i16) {}) == 0;
    fmt::print(
        "{}collision: {} bsp3d nodes, {} planes, {} leaves, {} bsp2d refs, "
        "{} bsp2d nodes, {} surfaces ({} two-sided, {} invisible, {} invalid, "
        "{} breakable, {} climbable, {} open loops), {} edges, "
        "{} vertices ({} finite)\n",
        indent,
        count(bsp.bsp3d_nodes),
        count(bsp.planes),
        count(bsp.leaves),
        count(bsp.bsp2d_references),
        count(bsp.bsp2d_nodes),
        count(bsp.surfaces),
        two_sided,
        invisible,
        invalid,
        breakable,
        climbable,
        open_loops,
        count(bsp.edges),
        count(bsp.vertices),
        finite);
}

/*! The sbsp body sits in its own region; `region` and `magic` come from
 * the scenario's structure_bsps entry */
template<typename V>
inline void dump_sbsp(
    h2::map_container<V> const& map,
    geo::bsp_header_t const&    bsp,
    h2::bsp_magic const&        magic)
{
    fmt::print(
        "  bounds x[{}, {}] y[{}, {}] z[{}, {}]\n",
        from_le(bsp.x_min),
        from_le(bsp.x_max),
        from_le(bsp.y_min),
        from_le(bsp.y_max),
        from_le(bsp.z_min),
        from_le(bsp.z_max));
    if(auto coll = bsp.collision.data(magic); !coll.has_error())
        for(auto const& c : coll.value())
            dump_collision(map, magic, c, "  ");
    auto print_bounds = [&](auto const& section, const char* indent) {
        auto bounds = section.bounds.data(magic);
        if(bounds.has_error())
            return;
        for(auto const& b : bounds.value())
            fmt::print(
                "{}bounds x[{}, {}] y[{}, {}] z[{}, {}] u[{}, {}] v[{}, {}]\n",
                indent,
                from_le(b.x_min),
                from_le(b.x_max),
                from_le(b.y_min),
                from_le(b.y_max),
                from_le(b.z_min),
                from_le(b.z_max),
                from_le(b.u_min),
                from_le(b.u_max),
                from_le(b.v_min),
                from_le(b.v_max));
    };
    if(auto leaves = bsp.leaves.data(magic); !leaves.has_error())
    {
        i16 max_cluster = -1;
        for(auto const& l : leaves.value())
            max_cluster = std::max(max_cluster, l.cluster.value());
        fmt::print(
            "  {} leaves, clusters referenced up to {}\n",
            leaves.value().size(),
            max_cluster);
    }
    if(auto portals = bsp.portals.data(magic); !portals.has_error())
    {
        fmt::print("  {} cluster portals\n", portals.value().size());
        for(auto const& [i, p] : stl_types::enumerate(portals.value()))
            if(i < 4)
            {
                auto verts = p.vertices.data(magic);
                fmt::print(
                    "    portal {}: {} <-> {} plane={} centroid=({}, {}, {}) "
                    "r={} flags={:#x} vertices={}\n",
                    i,
                    p.back_cluster(),
                    p.front_cluster(),
                    from_le(p.plane_raw),
                    from_le(p.centroid[0]),
                    from_le(p.centroid[1]),
                    from_le(p.centroid[2]),
                    from_le(p.bounding_radius),
                    from_le(p.flags_raw),
                    verts.has_error() ? 0 : verts.value().size());
            }
    }
    if(auto clusters = bsp.clusters.data(magic); !clusters.has_error())
    {
        size_t vertices = 0, faces = 0;
        for(auto const& [i, c] : stl_types::enumerate(clusters.value()))
        {
            vertices += c.vertex_count();
            faces += c.face_count();
            {
                auto portals   = c.portals.data(magic);
                auto instances = c.instance_indices.data(magic);
                fmt::print(
                    "  cluster {}: bounds x[{}, {}] y[{}, {}] z[{}, {}] sky={} "
                    "visible_sky={} fog={} planar={} fog_plane={} media={} "
                    "portals={} instances={}\n",
                    i,
                    from_le(c.x_min),
                    from_le(c.x_max),
                    from_le(c.y_min),
                    from_le(c.y_max),
                    from_le(c.z_min),
                    from_le(c.z_max),
                    c.sky.value(),
                    c.visible_sky.value(),
                    c.atmospheric_fog.value(),
                    c.planar_fog_designator,
                    c.visible_fog_plane.value(),
                    c.media_index,
                    portals.has_error() ? 0 : portals.value().size(),
                    instances.has_error() ? 0 : instances.value().size());
            }
            if(i < 3)
            {
                fmt::print(
                    "  cluster {}: vertices={} faces={} data {}+{:#x} {} bytes base={}\n",
                    i,
                    c.vertex_count(),
                    c.face_count(),
                    raw_file_name(c.data),
                    c.data.offset(),
                    c.data_size(),
                    c.base_address());
                print_bounds(c, "    ");
                dump_resources(map, magic, c.resources);
            }
        }
        fmt::print(
            "  {} clusters, {} vertices, {} faces\n",
            clusters.value().size(),
            vertices,
            faces);
    }
    auto sections = bsp.sections.data(magic);
    if(!sections.has_error())
        for(auto const& [i, s] : stl_types::enumerate(sections.value()))
        {
            fmt::print(
                "  definition {}: vertices={} faces={} data {}+{:#x} {} bytes\n",
                i,
                s.vertex_count(),
                s.face_count(),
                raw_file_name(s.data),
                s.data.offset(),
                s.data_size());
            print_bounds(s, "    ");
            if(i < 3)
                dump_resources(map, magic, s.resources);
            dump_collision(map, magic, s.collision, "    ");
        }
    if(auto instances = bsp.instances.data(magic); !instances.has_error())
        for(auto const& [i, inst] : stl_types::enumerate(instances.value()))
            fmt::print(
                "  instance {} '{}' definition={} scale={} at ({}, {}, {}) "
                "sphere r={}\n",
                i,
                map.string(inst.name),
                inst.section.value(),
                from_le(inst.scale),
                from_le(inst.transform[9]),
                from_le(inst.transform[10]),
                from_le(inst.transform[11]),
                from_le(inst.sphere_radius));
    if(auto shaders = bsp.shaders.data(magic); !shaders.has_error())
        for(auto const& [i, s] : stl_types::enumerate(shaders.value()))
            fmt::print("  shader {}: {}\n", i, tag_label(map, s.shader));
}

template<typename V>
inline void dump_ltmp(
    h2::map_container<V> const& map,
    h2::ltmp::header_t const&   ltmp,
    h2::bsp_magic const&        magic)
{
    auto groups = ltmp.groups.data(magic);
    if(groups.has_error())
        return;
    for(auto const& [gi, g] : stl_types::enumerate(groups.value()))
    {
        auto count = [&](auto const& block) -> size_t {
            auto data = block.data(magic);
            return data.has_error() ? 0 : data.value().size();
        };
        fmt::print(
            "  group {}: type={} flags={:#x} bitmaps={} palettes={} writable={} "
            "clusters={} render_info={} poops={} instance_render_info={} "
            "environments={} buckets={}\n",
            gi,
            from_le(g.type),
            from_le(g.flags),
            tag_label(map, g.bitmap_group),
            count(g.section_palette),
            count(g.writable_palettes),
            count(g.clusters),
            count(g.cluster_render_info),
            count(g.poop_definitions),
            count(g.instance_render_info),
            count(g.lighting_environments),
            count(g.geometry_buckets));
        if(auto palettes = g.section_palette.data(magic);
           !palettes.has_error())
            for(auto const& [pi, pal] : stl_types::enumerate(palettes.value()))
                if(pi < 2)
                    fmt::print(
                        "    palette {}: [0]={:#010x} [1]={:#010x} [128]={:#010x} "
                        "[255]={:#010x}\n",
                        pi,
                        from_le(pal.colors[0]),
                        from_le(pal.colors[1]),
                        from_le(pal.colors[128]),
                        from_le(pal.colors[255]));
        auto dump_sections = [&](auto const& block, const char* what) {
            auto sections = block.data(magic);
            if(sections.has_error())
                return;
            for(auto const& [i, c] : stl_types::enumerate(sections.value()))
            {
                if(i >= 3)
                    break;
                fmt::print(
                    "    {} {}: vertices={} faces={} data {}+{:#x} {} bytes base={}\n",
                    what,
                    i,
                    c.vertex_count(),
                    c.face_count(),
                    raw_file_name(c.data),
                    c.data.offset(),
                    c.data_size(),
                    c.base_address());
                if(auto b = c.lightmap_bounds(magic); !b.has_error())
                    for(auto const& bb : b.value())
                        fmt::print(
                            "      bounds u[{}, {}] v[{}, {}] u2[{}, {}] v2[{}, {}]\n",
                            from_le(bb.u_min),
                            from_le(bb.u_max),
                            from_le(bb.v_min),
                            from_le(bb.v_max),
                            from_le(bb.u2_min),
                            from_le(bb.u2_max),
                            from_le(bb.v2_min),
                            from_le(bb.v2_max));
                dump_resources(map, magic, c.resources);
            }
        };
        dump_sections(g.clusters, "cluster");
        dump_sections(g.poop_definitions, "poop");
        auto dump_info = [&](auto const& block, const char* what) {
            auto info = block.data(magic);
            if(info.has_error())
                return;
            fmt::print("    {}:", what);
            for(auto const& r : info.value())
                fmt::print(" {}/{}", r.bitmap.value(), r.palette.value());
            fmt::print("\n");
        };
        dump_info(g.cluster_render_info, "cluster bitmap/palette");
        dump_info(g.instance_render_info, "instance bitmap/palette");
        if(auto buckets = g.geometry_buckets.data(magic); !buckets.has_error())
            for(auto const& [i, b] : stl_types::enumerate(buckets.value()))
            {
                fmt::print(
                    "    bucket {}: flags={:#x} data {}+{:#x} {} bytes base={} "
                    "resource data {} owner {:#x} section offset {}\n",
                    i,
                    from_le(b.flags),
                    raw_file_name(b.data),
                    b.data.offset(),
                    b.data_size(),
                    b.base_address(),
                    from_le(b.resource_data_size_raw),
                    from_le(b.owner_tag.id.raw),
                    from_le(b.owner_tag_section_offset_raw));
                dump_resources(map, magic, b.resources);
            }
        auto dump_refs = [&](auto const& block, const char* what) {
            auto refs = block.data(magic);
            if(refs.has_error())
                return;
            for(auto const& [i, r] : stl_types::enumerate(refs.value()))
            {
                fmt::print(
                    "    {} {}: flags={:#x} bucket={} offsets", what, i, from_le(r.flags), r.bucket.value());
                if(auto offs = r.section_offsets.data(magic); !offs.has_error())
                    for(auto const& o : offs.value())
                        fmt::print(" {}", o.value());
                fmt::print("\n");
            }
        };
        dump_refs(g.instance_bucket_refs, "instance bucket ref");
        if(auto infos = g.scenery_object_info.data(magic); !infos.has_error())
            for(auto const& [i, o] : stl_types::enumerate(infos.value()))
                if(i < 6)
                    fmt::print(
                        "    scenery object {}: id={:#x} bsp={} type={} source={} "
                        "checksum={:#x}\n",
                        i,
                        from_le(o.unique_id.raw),
                        from_le(o.origin_bsp.raw),
                        o.type,
                        o.source,
                        from_le(o.render_model_checksum_raw));
        dump_refs(g.scenery_bucket_refs, "scenery bucket ref");
    }
}

template<typename V>
inline void dump_sky(h2::map_container<V> const& map, h2::tag_t const& tag)
{
    auto const* sky = tag.template data<h2::obj::sky_t>(map.magic());
    if(!sky)
        return;
    fmt::print(
        "  render model {}\n  animation graph {}\n  outdoor ambient ({}, {}, "
        "{}) fog spread {}\n",
        tag_label(map, sky->render_model),
        tag_label(map, sky->animation_graph),
        from_le(sky->outdoor_ambient[0]),
        from_le(sky->outdoor_ambient[1]),
        from_le(sky->outdoor_ambient[2]),
        from_le(sky->fog_spread_distance));
    auto fogs = [&](auto const& block, const char* what) {
        auto data = block.data(map.magic());
        if(data.has_error())
            return;
        for(auto const& f : data.value())
            fmt::print(
                "  {} color=({}, {}, {}) density={}\n",
                what,
                from_le(f.color[0]),
                from_le(f.color[1]),
                from_le(f.color[2]),
                from_le(f.maximum_density));
    };
    fogs(sky->atmospheric_fog, "atmospheric fog");
    fogs(sky->secondary_fog, "secondary fog");
    if(auto data = sky->sky_fog.data(map.magic()); !data.has_error())
        for(auto const& f : data.value())
            fmt::print(
                "  sky fog color=({}, {}, {}) density={}\n",
                from_le(f.color[0]),
                from_le(f.color[1]),
                from_le(f.color[2]),
                from_le(f.density));
}

/*! Dispatch on class; sbsp bodies are only reachable through the scenario
 * (own region), so they are dumped from there */
template<typename V>
inline void dump_tag(
    h2::map_container<V> const& map, h2::tag_t const& tag, bool header = true)
{
    if(header)
        fmt::print(
            "{} [{}] {} bytes @{:#x}\n",
            map.name_of(tag),
            tag.tag_class.str(),
            tag.size(),
            tag.pointer());
    if(tag.matches("bitm"))
        dump_bitm(map, tag);
    else if(tag.matches("shad"))
        dump_shad(map, tag);
    else if(tag.matches("stem"))
        dump_stem(map, tag);
    else if(tag.matches("mode"))
        dump_mode(map, tag);
    else if(tag.matches("sky "))
        dump_sky(map, tag);
}

} // namespace dimeter_info
