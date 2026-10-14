#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/coffee_args.h>

#include <blam/elegy/structure.h>
#include <cxxopts.hpp>

#include <cmath>
#include <limits>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace Coffee;
using semantic::BytesConst;
using semantic::Span;
using typing::vector_types::Vecf3;

namespace elegy = blam::elegy;

namespace {

struct options_t
{
    std::string class_filter;
    std::string name_filter;
    bool        list;
    bool        models;
    bool        bsp;
    std::string obj_path;
};

const char* cache_type_name(elegy::cache_type_t type)
{
    using elegy::cache_type_t;
    switch(type)
    {
    case cache_type_t::campaign:
        return "campaign";
    case cache_type_t::multiplayer:
        return "multiplayer";
    case cache_type_t::mainmenu:
        return "mainmenu";
    case cache_type_t::shared:
        return "shared";
    case cache_type_t::campaign_shared:
        return "campaign_shared";
    default:
        return "unknown";
    }
}

const char* resource_error_name(elegy::resource_error error)
{
    using elegy::resource_error;
    switch(error)
    {
    case resource_error::no_gestalt:
        return "no resource gestalt";
    case resource_error::no_resource:
        return "no such resource";
    case resource_error::bad_segment:
        return "bad segment";
    case resource_error::missing_cache_file:
        return "page in a cache file not attached";
    case resource_error::page_out_of_bounds:
        return "page out of bounds";
    case resource_error::decompression_failed:
        return "decompression failed";
    }
    return "?";
}

using triangle_t = std::array<u16, 3>;

/*! Decoded geometry plus how well stored normals agree with the faces */
struct geometry_stats
{
    u32    meshes{0};
    u32    failed{0};
    u32    empty{0};
    std::map<i16, u32> unsupported;
    u64    vertices{0};
    u64    triangles{0};
    f64    normal_agreement{0};
    u64    normal_samples{0};
    Vecf3  min{std::numeric_limits<f32>::max()};
    Vecf3  max{std::numeric_limits<f32>::lowest()};

    void add(
        std::vector<elegy::vertex_t> const& vertices,
        std::vector<triangle_t> const&      triangles)
    {
        meshes++;
        this->vertices += vertices.size();
        this->triangles += triangles.size();
        for(auto const& v : vertices)
        {
            min = glm::min(min, v.position);
            max = glm::max(max, v.position);
        }
        for(auto const& t : triangles)
        {
            if(t[0] >= vertices.size() || t[1] >= vertices.size() ||
               t[2] >= vertices.size())
                continue;
            auto const& a    = vertices[t[0]];
            auto        face = glm::cross(
                vertices[t[1]].position - a.position,
                vertices[t[2]].position - a.position);
            f32 len = glm::length(face) * glm::length(a.normal);
            if(len < 1e-12f)
                continue;
            normal_agreement += glm::dot(face, a.normal) / len;
            normal_samples++;
        }
    }

    void merge(geometry_stats const& other)
    {
        meshes += other.meshes;
        failed += other.failed;
        empty += other.empty;
        for(auto [format, count] : other.unsupported)
            unsupported[format] += count;
        vertices += other.vertices;
        triangles += other.triangles;
        normal_agreement += other.normal_agreement;
        normal_samples += other.normal_samples;
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }

    void print(std::string_view label) const
    {
        fmt::print(
            "{}: {} meshes ({} failed, {} empty), {} vertices, {} triangles, "
            "normal/face cos {:.3f}, bounds ({:.1f} {:.1f} {:.1f}) - "
            "({:.1f} {:.1f} {:.1f})\n",
            label,
            meshes,
            failed,
            empty,
            vertices,
            triangles,
            normal_samples ? normal_agreement / normal_samples : 0.0,
            min.x,
            min.y,
            min.z,
            max.x,
            max.y,
            max.z);
        for(auto [format, count] : unsupported)
            fmt::print("  {} meshes in unsupported vertex format {}\n", count, format);
    }
};

struct obj_writer
{
    std::ofstream out;
    u32           base{1};

    explicit obj_writer(std::string const& path)
        : out(path)
    {
    }

    void add(
        std::string_view                    name,
        std::vector<elegy::vertex_t> const& vertices,
        std::vector<triangle_t> const&      triangles,
        glm::mat4 const&                    transform = glm::mat4(1.f))
    {
        out << "o " << name << "\n";
        for(auto const& v : vertices)
        {
            auto p = transform * glm::vec4(v.position, 1.f);
            auto n = glm::mat3(transform) * v.normal;
            out << "v " << p.x << " " << p.y << " " << p.z << "\n";
            out << "vn " << n.x << " " << n.y << " " << n.z << "\n";
            out << "vt " << v.texcoord.x << " " << 1.f - v.texcoord.y << "\n";
        }
        for(auto const& t : triangles)
        {
            u32 a = base + t[0], b = base + t[1], c = base + t[2];
            out << "f " << a << "/" << a << "/" << a << " " << b << "/" << b
                << "/" << b << " " << c << "/" << c << "/" << c << "\n";
        }
        base += static_cast<u32>(vertices.size());
    }
};

/*! A mesh's vertices and triangles, all parts; a mesh skipped is counted */
bool decode_mesh(
    elegy::resource_view const&     view,
    elegy::mesh_t const&            mesh,
    elegy::compression_t const*     compression,
    std::vector<elegy::vertex_t>&   vertices,
    std::vector<triangle_t>&        triangles,
    elegy::tag_magic const&         magic,
    geometry_stats&                 stats)
{
    auto buffers = elegy::mesh_buffers::from(view, mesh);
    vertices.clear();
    triangles.clear();
    /* Instances that only carry lighting have no buffers at all */
    if(mesh.vertex_buffers[0].value() < 0)
    {
        stats.empty++;
        return false;
    }
    if(!buffers.decode(compression, vertices))
    {
        if(buffers.vertices)
            stats.unsupported[static_cast<i16>(buffers.vertices->format.value())]++;
        stats.failed++;
        return false;
    }
    auto parts = mesh.parts.data(magic).value_or(Span<elegy::part_t const>());
    if(parts.empty())
        buffers.triangles(mesh.index_type, 0, buffers.index_count(), triangles);
    for(auto const& part : parts)
        buffers.triangles(
            mesh.index_type, part.index_start, part.index_count, triangles);
    return true;
}

int list_tags(elegy::map_container const& map, options_t const& options)
{
    for(auto const& tag : map.tags())
    {
        if(!tag.valid())
            continue;
        auto const* group = map.group_of(tag);
        auto        name  = map.name_of(tag);
        if(!options.class_filter.empty() &&
           (!group || group->group.str() != options.class_filter))
            continue;
        if(name.find(options.name_filter) == std::string_view::npos)
            continue;
        fmt::print(
            "{:08x} {} {}\n",
            map.id_of(tag).raw.value(),
            group ? group->group.str() : "????",
            name);
    }
    return 0;
}

int dump_models(
    elegy::map_container const& map,
    elegy::resource_loader&     loader,
    options_t const&            options,
    obj_writer*                 obj)
{
    auto           magic = map.magic();
    geometry_stats total;
    std::vector<elegy::vertex_t> vertices;
    std::vector<triangle_t>      triangles;
    for(auto const& tag : map.tags())
    {
        if(!tag.valid() || !map.is(tag, "mode"))
            continue;
        auto name = map.name_of(tag);
        if(name.find(options.name_filter) == std::string_view::npos)
            continue;
        auto const* model = map.tag_data<elegy::render_model_t>(tag);
        if(!model)
            continue;
        auto const& geometry = model->geometry;
        auto        meshes   = geometry.meshes.data(magic).value_or(
            Span<elegy::mesh_t const>());
        auto compression = geometry.compression.data(magic).value_or(
            Span<elegy::compression_t const>());
        auto regions = model->regions.data(magic).value_or(
            Span<elegy::region_t const>());
        auto nodes =
            model->nodes.data(magic).value_or(Span<elegy::node_t const>());

        auto view = loader.load(geometry.resource);
        if(!view.has_value())
        {
            fmt::print(
                "{}: {}\n", name, resource_error_name(view.error()));
            total.failed += static_cast<u32>(meshes.size());
            continue;
        }
        geometry_stats stats;
        for(size_t i = 0; i < meshes.size(); i++)
        {
            if(!decode_mesh(
                   view.value(),
                   meshes[i],
                   compression.empty() ? nullptr : &compression[0],
                   vertices,
                   triangles,
                   magic,
                   stats))
                continue;
            stats.add(vertices, triangles);
            if(obj)
                obj->add(fmt::format("{}_{}", name, i), vertices, triangles);
        }
        fmt::print(
            "{} [{} regions, {} nodes, {} materials]\n",
            name,
            regions.size(),
            nodes.size(),
            model->materials.count());
        for(auto const& region : regions)
            for(auto const& perm : region.permutations.data(magic).value_or(
                    Span<elegy::permutation_t const>()))
                fmt::print(
                    "  {}:{} meshes {}+{}\n",
                    map.string(region.name),
                    map.string(perm.name),
                    perm.mesh.value(),
                    perm.mesh_count.value());
        stats.print("  geometry");
        total.merge(stats);
        loader.trim();
    }
    total.print("all models");
    return 0;
}

int dump_bsps(
    elegy::map_container const& map,
    elegy::resource_loader&     loader,
    options_t const&            options,
    obj_writer*                 obj)
{
    auto magic = map.magic();
    auto const* scenario_tag = map.find("scnr");
    auto const* lightmap_tag = map.find("sLdT");
    if(!scenario_tag || !lightmap_tag)
    {
        fmt::print("no scenario or scenario_lightmap\n");
        return 1;
    }
    auto const* scenario = map.tag_data<elegy::scenario_t>(*scenario_tag);
    auto const* lightmap = map.tag_data<elegy::scenario_lightmap_t>(*lightmap_tag);
    if(!scenario || !lightmap)
        return 1;
    auto scenario_bsps = scenario->bsps.data(magic).value_or(
        Span<elegy::scenario_bsp_t const>());

    std::vector<elegy::vertex_t> vertices;
    std::vector<triangle_t>      triangles;
    for(auto const& lightmap_bsp : lightmap->bsps.data(magic).value_or(
            Span<elegy::lightmap_bsp_t const>()))
    {
        i16 bsp_index = lightmap_bsp.bsp;
        if(bsp_index < 0 || static_cast<size_t>(bsp_index) >= scenario_bsps.size())
            continue;
        auto const* bsp_tag = map.tag_at(scenario_bsps[bsp_index].bsp.id);
        if(!bsp_tag)
            continue;
        auto name = map.name_of(*bsp_tag);
        if(name.find(options.name_filter) == std::string_view::npos)
            continue;
        auto const* bsp = map.tag_data<elegy::structure_bsp_t>(*bsp_tag);
        if(!bsp)
            continue;

        auto const& geometry = lightmap_bsp.geometry;
        auto        meshes   = geometry.meshes.data(magic).value_or(
            Span<elegy::mesh_t const>());
        auto compression = geometry.compression.data(magic).value_or(
            Span<elegy::compression_t const>());
        auto clusters =
            bsp->clusters.data(magic).value_or(Span<elegy::cluster_t const>());
        auto instances = bsp->instances.data(magic).value_or(
            Span<elegy::instanced_geometry_t const>());

        auto view = loader.load(geometry.resource);
        if(!view.has_value())
        {
            fmt::print(
                "{}: geometry {}\n", name, resource_error_name(view.error()));
            continue;
        }
        auto resources = loader.load(bsp->tag_resources);
        Span<elegy::instanced_geometry_definition_t const> definitions;
        if(resources.has_value())
            if(auto const* root =
                   resources->root_as<elegy::structure_bsp_resources_t>())
                definitions = resources->block(root->instanced_geometry);

        fmt::print(
            "{} [{} meshes, {} clusters, {} instances of {} definitions]\n",
            name,
            meshes.size(),
            clusters.size(),
            instances.size(),
            definitions.size());

        geometry_stats cluster_stats;
        for(size_t i = 0; i < clusters.size(); i++)
        {
            auto const* mesh = clusters[i].mesh.resolve(meshes);
            if(!mesh)
                continue;
            if(!decode_mesh(
                   view.value(), *mesh, nullptr, vertices, triangles, magic, cluster_stats))
                continue;
            cluster_stats.add(vertices, triangles);
            if(obj)
                obj->add(fmt::format("cluster_{}", i), vertices, triangles);
        }
        cluster_stats.print("  clusters");

        geometry_stats instance_stats;
        for(auto const& instance : instances)
        {
            auto const* definition = instance.definition.resolve(definitions);
            if(!definition)
            {
                instance_stats.failed++;
                continue;
            }
            auto const* mesh = definition->mesh.resolve(meshes);
            auto const* comp = definition->compression.resolve(compression);
            if(!mesh)
            {
                instance_stats.failed++;
                continue;
            }
            if(!decode_mesh(
                   view.value(), *mesh, comp, vertices, triangles, magic, instance_stats))
                continue;
            instance_stats.add(vertices, triangles);
            if(obj)
            {
                glm::mat4 transform(1.f);
                f32       scale = instance.scale;
                transform[0]    = glm::vec4(instance.forward.value() * scale, 0.f);
                transform[1]    = glm::vec4(instance.left.value() * scale, 0.f);
                transform[2]    = glm::vec4(instance.up.value() * scale, 0.f);
                transform[3]    = glm::vec4(instance.position.value(), 1.f);
                obj->add(
                    map.string(instance.name), vertices, triangles, transform);
            }
        }
        instance_stats.print("  instances (object space)");
        loader.trim();
    }
    return 0;
}

int run_map(
    elegy::map_container const& map,
    std::string const&          map_dir,
    options_t const&            options)
{
    using namespace platform::url::constructors;

    auto const& header = *map.header;
    fmt::print(
        "{} ({}) build {}, {} tags in {} groups\n",
        map.name(),
        cache_type_name(header.map_type),
        map.build(),
        map.tags().size(),
        map.groups().size());
    fmt::print("  scenario {}\n", map.scenario_name());

    if(options.list)
        return list_tags(map, options);

    auto loader = elegy::resource_loader::create(map);
    if(!loader)
    {
        fmt::print("no zone/play tags\n");
        return 1;
    }
    fmt::print("  {} resources\n", loader->resources().size());

    /* Pages may live in the other cache files; attach those present */
    std::vector<std::unique_ptr<Resource>>            side_files;
    std::vector<std::unique_ptr<elegy::map_container>> side_maps;
    for(auto const& cache : loader->external_caches())
    {
        auto file_name = std::string(cache.file_name());
        auto res =
            std::make_unique<Resource>(MkSysUrl(map_dir + "/" + file_name));
        auto bytes = C_OCAST<BytesConst>(*res);
        if(!bytes.data || !bytes.size)
        {
            fmt::print("  {} not found, its pages will fail\n", file_name);
            continue;
        }
        auto side = elegy::map_container::from_bytes(bytes);
        if(!side.has_value())
        {
            fmt::print("  {} not found, its pages will fail\n", file_name);
            continue;
        }
        side_maps.push_back(std::make_unique<elegy::map_container>(side.value()));
        loader->attach(file_name, *side_maps.back());
        side_files.push_back(std::move(res));
        fmt::print("  attached {}\n", file_name);
    }

    std::unique_ptr<obj_writer> obj;
    if(!options.obj_path.empty())
        obj = std::make_unique<obj_writer>(options.obj_path);

    int rc = 0;
    if(options.models)
        rc |= dump_models(map, *loader, options, obj.get());
    if(options.bsp)
        rc |= dump_bsps(map, *loader, options, obj.get());
    return rc;
}

} // namespace

i32 coffee_main(i32, cstring_w*)
{
    using namespace platform::url::constructors;

    cxxopts::Options opts(
        "Halo 3 Map Info", "Inspect a Halo 3 (Xbox 360) cache file");
    opts.add_options("Help")("h,help", "Show this help info");
    opts.positional_help("<map file>");
    opts.add_options("Tag selection")
        //
        ("c,class", "Only tags of this group, e.g. mode", cxxopts::value<std::string>())
        //
        ("n,name", "Only tags whose name contains this", cxxopts::value<std::string>())
        //
        ("l,list", "List matching tags")
        //
        ("models", "Decode render model geometry")
        //
        ("bsp", "Decode structure BSP geometry (clusters + instances)")
        //
        ("obj", "Write decoded geometry to this OBJ", cxxopts::value<std::string>())
        //
        ;

    auto& args      = Coffee::GetInitArgs();
    auto  arguments = opts.parse(args.size(), args.data());
    if(Coffee::BaseArgParser::PerformDefaults(opts, args) >= 0)
        return 0;

    std::string map_path;
    for(auto const& positional : arguments.unmatched())
        if(std::filesystem::is_regular_file(positional))
        {
            map_path = positional;
            break;
        }
    if(map_path.empty())
    {
        cBasicPrint("Needs a map file, check --help");
        return 1;
    }

    options_t options{
        .class_filter = arguments.as_optional<std::string>("class").value_or(""),
        .name_filter  = arguments.as_optional<std::string>("name").value_or(""),
        .list         = arguments.count("list") > 0,
        .models       = arguments.count("models") > 0,
        .bsp          = arguments.count("bsp") > 0,
        .obj_path     = arguments.as_optional<std::string>("obj").value_or(""),
    };

    auto        slash   = map_path.find_last_of('/');
    std::string map_dir = slash == std::string::npos
                              ? std::string(".")
                              : map_path.substr(0, slash);

    Resource map_file(MkSysUrl(map_path));
    auto     map_data = C_OCAST<BytesConst>(map_file);
    if(!map_data.data || map_data.size == 0)
    {
        cBasicPrint("failed to read {0}", map_path);
        return 1;
    }

    int rc = 1;
    if(auto map = elegy::map_container::from_bytes(map_data); map.has_value())
    {
        if(!map->has_tags())
            cBasicPrint("{0} is a resource-only cache file", map_path);
        else
            rc = run_map(map.value(), map_dir, options);
    } else
        cBasicPrint(
            "not a (supported) Halo 3 map: error {0}",
            static_cast<int>(map.error()));

    /* quick_exit skips stdio teardown, which would drop a short dump */
    fflush(stdout);
    std::quick_exit(rc);
}

COFFEE_APPLICATION_MAIN_CUSTOM(coffee_main, 0x1 | 0x2)
