#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/coffee_args.h>

#include <blam/elegy/bitmap.h>
#include <blam/elegy/shader.h>
#include <blam/elegy/structure.h>
#include <cxxopts.hpp>

#include <cmath>
#include <limits>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <memory>
#include <string>
#include <vector>

using namespace Coffee;
using semantic::BytesConst;
using semantic::Span;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

namespace elegy = blam::elegy;

namespace {

struct options_t
{
    std::string class_filter;
    std::string name_filter;
    bool        list;
    bool        models;
    bool        bsp;
    bool        shaders;
    bool        bitmaps;
    std::string obj_path;
    std::string extract_dir;
    u32         limit;
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

std::string sanitize(std::string_view tag_name)
{
    std::string out(tag_name);
    for(char& c : out)
        if(c == '\\' || c == '/' || c == ' ')
            c = '_';
    return out;
}

template<typename T>
void append(std::vector<char>& out, T const& value)
{
    auto const* bytes = reinterpret_cast<char const*>(&value);
    out.insert(out.end(), bytes, bytes + sizeof(T));
}

struct dds_info
{
    u32 fourcc{0}; /* 0 = uncompressed, described by the mask fields */
    u32 bpp{0};
    u32 pf_flags{0};
    u32 mask_r{0};
    u32 mask_g{0};
    u32 mask_b{0};
    u32 mask_a{0};
};

/* DDS can carry the PC-compatible formats once linearized; CTX1 and the
 * DXT3a family have no DDS equivalent */
std::optional<dds_info> dds_format(elegy::bitmap_format_t format)
{
    using enum elegy::bitmap_format_t;
    constexpr u32 RGB  = 0x40;
    constexpr u32 RGBA = 0x41;
    constexpr u32 LUM  = 0x20000;
    constexpr u32 LUMA = 0x20001;
    constexpr u32 A    = 0x2;
    switch(format)
    {
    case dxt1:
        return dds_info{.fourcc = 0x31545844 /*DXT1*/};
    case dxt3:
        return dds_info{.fourcc = 0x33545844 /*DXT3*/};
    case dxt5:
        return dds_info{.fourcc = 0x35545844 /*DXT5*/};
    case dxt5a:
    case dxt5a_alpha:
    case dxt5a_mono:
        return dds_info{.fourcc = 0x31495441 /*ATI1*/};
    case dxn:
    case dxn_mono_alpha:
        return dds_info{.fourcc = 0x32495441 /*ATI2*/};
    case a8r8g8b8:
        return dds_info{
            .bpp      = 32,
            .pf_flags = RGBA,
            .mask_r   = 0x00FF0000,
            .mask_g   = 0x0000FF00,
            .mask_b   = 0x000000FF,
            .mask_a   = 0xFF000000};
    case x8r8g8b8:
        return dds_info{
            .bpp      = 32,
            .pf_flags = RGB,
            .mask_r   = 0x00FF0000,
            .mask_g   = 0x0000FF00,
            .mask_b   = 0x000000FF};
    case r5g6b5:
        return dds_info{
            .bpp = 16, .pf_flags = RGB, .mask_r = 0xF800, .mask_g = 0x07E0, .mask_b = 0x001F};
    case a4r4g4b4:
        return dds_info{
            .bpp      = 16,
            .pf_flags = RGBA,
            .mask_r   = 0x0F00,
            .mask_g   = 0x00F0,
            .mask_b   = 0x000F,
            .mask_a   = 0xF000};
    case a8y8:
        return dds_info{.bpp = 16, .pf_flags = LUMA, .mask_r = 0x00FF, .mask_a = 0xFF00};
    case y8:
    case ay8:
        return dds_info{.bpp = 8, .pf_flags = LUM, .mask_r = 0xFF};
    case a8:
        return dds_info{.bpp = 8, .pf_flags = A, .mask_a = 0xFF};
    default:
        return std::nullopt;
    }
}

std::vector<char> make_dds(
    dds_info const& info, u32 width, u32 height, Span<byte_t const> pixels)
{
    std::vector<char> out;
    out.reserve(128 + pixels.size());
    append<u32>(out, 0x20534444); /* 'DDS ' */
    append<u32>(out, 124);
    append<u32>(out, 0x1007); /* CAPS|HEIGHT|WIDTH|PIXELFORMAT */
    append<u32>(out, height);
    append<u32>(out, width);
    append<u32>(out, static_cast<u32>(pixels.size()));
    for(int i = 0; i < 13; i++)
        append<u32>(out, 0);
    append<u32>(out, 32);
    if(info.fourcc)
    {
        append<u32>(out, 0x4);
        append<u32>(out, info.fourcc);
        for(int i = 0; i < 5; i++)
            append<u32>(out, 0);
    } else
    {
        append<u32>(out, info.pf_flags);
        append<u32>(out, 0);
        append<u32>(out, info.bpp);
        append<u32>(out, info.mask_r);
        append<u32>(out, info.mask_g);
        append<u32>(out, info.mask_b);
        append<u32>(out, info.mask_a);
    }
    append<u32>(out, 0x1000);
    for(int i = 0; i < 4; i++)
        append<u32>(out, 0);
    out.insert(out.end(), pixels.begin(), pixels.end());
    return out;
}

bool is_render_method(elegy::map_container const& map, elegy::tag_t const& tag)
{
    auto const* group = map.group_of(tag);
    return group && (group->group == "rm  " || group->parent == "rm  ");
}

std::string_view tag_name(elegy::map_container const& map, elegy::tag_reference const& ref)
{
    auto const* tag = map.tag_at(ref.id);
    return tag ? map.name_of(*tag) : std::string_view("(none)");
}

int dump_shaders(elegy::map_container const& map, options_t const& options)
{
    auto magic = map.magic();
    u32  count = 0;
    for(auto const& tag : map.tags())
    {
        if(!tag.valid() || !is_render_method(map, tag))
            continue;
        auto name = map.name_of(tag);
        if(name.find(options.name_filter) == std::string_view::npos)
            continue;
        if(!options.class_filter.empty() &&
           map.group_of(tag)->group.str() != options.class_filter)
            continue;
        auto const* method = map.tag_data<elegy::render_method_t>(tag);
        if(!method)
            continue;
        count++;
        fmt::print("{} [{}]\n", name, map.group_of(tag)->group.str());

        /* Options picked per category of the definition */
        auto options_picked = method->options.data(magic).value_or(Span<elegy::be<i16> const>());
        if(auto const* def_tag = map.tag_at(method->definition.id))
            if(auto const* def = map.tag_data<elegy::render_method_definition_t>(*def_tag))
            {
                std::string line;
                auto categories = def->categories.data(magic).value_or(
                    Span<elegy::render_method_category_t const>());
                for(size_t i = 0; i < categories.size(); i++)
                {
                    auto choices = categories[i].options.data(magic).value_or(
                        Span<elegy::render_method_option_t const>());
                    i16 pick = i < options_picked.size() ? options_picked[i].value() : 0;
                    auto option = pick >= 0 && static_cast<size_t>(pick) < choices.size()
                                      ? map.string(choices[pick].name)
                                      : std::string_view("?");
                    line += fmt::format(
                        "{}{}={}", line.empty() ? "" : " ", map.string(categories[i].name), option);
                }
                fmt::print("  {} ({})\n", map.name_of(*def_tag), line);
            }

        for(auto const& post : method->postprocess.data(magic).value_or(
                Span<elegy::render_method_postprocess_t const>()))
        {
            fmt::print(
                "  template {} blend {}\n",
                tag_name(map, post.shader_template),
                post.blend_mode.value());
            Span<elegy::string_id const> texture_names, float_names;
            if(auto const* tmpl_tag = map.tag_at(post.shader_template.id))
                if(auto const* tmpl = map.tag_data<elegy::render_method_template_t>(*tmpl_tag))
                {
                    texture_names = tmpl->textures.data(magic).value_or(Span<elegy::string_id const>());
                    float_names = tmpl->float_constants.data(magic).value_or(Span<elegy::string_id const>());
                }
            auto textures = post.textures.data(magic).value_or(
                Span<elegy::postprocess_texture_t const>());
            for(size_t i = 0; i < textures.size(); i++)
                fmt::print(
                    "    {:<24} {} (address {}, filter {}, transform {})\n",
                    i < texture_names.size() ? map.string(texture_names[i]) : "?",
                    tag_name(map, textures[i].bitmap),
                    textures[i].address_mode,
                    textures[i].filter_mode,
                    textures[i].transform);
            auto floats = post.float_constants.data(magic).value_or(
                Span<elegy::be<Vecf4> const>());
            for(size_t i = 0; i < floats.size(); i++)
            {
                Vecf4 v = floats[i];
                fmt::print(
                    "    {:<24} {:g} {:g} {:g} {:g}\n",
                    i < float_names.size() ? map.string(float_names[i]) : "?",
                    v.x,
                    v.y,
                    v.z,
                    v.w);
            }
        }
        if(options.limit && count >= options.limit)
            break;
    }
    return 0;
}

int dump_bitmaps(
    elegy::map_container const& map,
    elegy::resource_loader&     loader,
    options_t const&            options)
{
    auto magic = map.magic();
    if(!options.extract_dir.empty())
        std::filesystem::create_directories(options.extract_dir);
    u32 count = 0, written = 0;
    std::vector<byte_t> linear;
    for(auto const& tag : map.tags())
    {
        if(!tag.valid() || !map.is(tag, "bitm"))
            continue;
        auto name = map.name_of(tag);
        if(name.find(options.name_filter) == std::string_view::npos)
            continue;
        auto const* bitmap = map.tag_data<elegy::bitmap_t>(tag);
        if(!bitmap)
            continue;
        count++;
        auto images = bitmap->images.data(magic).value_or(Span<elegy::bitmap_image_t const>());
        auto textures = bitmap->hardware_textures.data(magic).value_or(
            Span<elegy::hardware_texture_t const>());
        fmt::print("{} [{} images, {} textures]\n", name, images.size(), textures.size());
        for(size_t i = 0; i < images.size(); i++)
        {
            auto const& image = images[i];
            auto format = image.format.value();
            fmt::print(
                "  {}: {}x{}x{} {} type {} mips {} xbox flags {:#x}\n",
                i,
                image.width.value(),
                image.height.value(),
                image.depth,
                elegy::to_string(format),
                static_cast<u16>(image.type.value()),
                image.mipmap_count,
                static_cast<u8>(image.xbox_flags));
            if(options.extract_dir.empty() || i >= textures.size())
                continue;
            if(image.type.value() != elegy::bitmap_type_t::texture_2d)
                continue;
            auto dds = dds_format(format);
            if(!dds)
                continue;
            auto view = loader.load(textures[i].resource);
            if(!view.has_value())
            {
                fmt::print("    {}\n", resource_error_name(view.error()));
                continue;
            }
            auto stored = elegy::top_level(view.value(), image);
            u32  width = static_cast<u32>(image.width.value());
            u32  height = static_cast<u32>(image.height.value());
            if(stored.empty() ||
               !elegy::linearize(stored, format, width, height, image.xbox_flags, linear))
            {
                fmt::print("    no pixel data\n");
                continue;
            }
            auto path = fmt::format(
                "{}/{}{}.dds",
                options.extract_dir,
                sanitize(name),
                images.size() > 1 ? fmt::format("_{}", i) : std::string());
            auto file = make_dds(*dds, width, height, linear);
            std::ofstream(path, std::ios::binary).write(file.data(), static_cast<std::streamsize>(file.size()));
            written++;
        }
        loader.trim();
        if(options.limit && count >= options.limit)
            break;
    }
    if(!options.extract_dir.empty())
        fmt::print("wrote {} DDS files to {}\n", written, options.extract_dir);
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
    if(options.shaders)
        rc |= dump_shaders(map, options);
    if(options.bitmaps)
        rc |= dump_bitmaps(map, *loader, options);
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
        ("shaders", "Decode render methods: options, template, textures, constants")
        //
        ("bitmaps", "List bitmap images")
        //
        ("obj", "Write decoded geometry to this OBJ", cxxopts::value<std::string>())
        //
        ("e,extract",
         "With --bitmaps, write each image's top level as DDS here",
         cxxopts::value<std::string>())
        //
        ("limit",
         "Cap on shaders/bitmaps visited, 0 = everything",
         cxxopts::value<int>()->default_value("0"))
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
        .shaders      = arguments.count("shaders") > 0,
        .bitmaps      = arguments.count("bitmaps") > 0,
        .obj_path     = arguments.as_optional<std::string>("obj").value_or(""),
        .extract_dir  = arguments.as_optional<std::string>("extract").value_or(""),
        .limit        = static_cast<u32>(arguments["limit"].as<int>()),
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
