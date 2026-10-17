#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/coffee_args.h>

#include <blam/elegy/bitmap.h>
#include <blam/elegy/collision.h>
#include <blam/elegy/physics.h>
#include <blam/elegy/shader.h>
#include <blam/elegy/sound.h>
#if defined(OAF_XMA2_DECODER_ENABLED)
#include <oaf/xma2/xma2_decode.h>
#endif
#include <blam/elegy/structure.h>
#include <blam/elegy/views.h>
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
    bool        sounds;
    bool        collision;
    bool        objects;
    bool        nodes;
    bool        lights;
    bool        zones;
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
    auto parts = mesh.parts.span(magic);
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
        auto        meshes   = geometry.meshes.span(magic);
        auto compression = geometry.compression.span(magic);
        auto regions = model->regions.span(magic);
        auto nodes =
            model->nodes.span(magic);

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
            for(auto const& perm : region.permutations.span(magic))
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
    auto const scenario = map.find<elegy::scenario_t>();
    auto const lightmap = map.find<elegy::scenario_lightmap_t>();
    if(!scenario || !lightmap)
    {
        fmt::print("no scenario or scenario_lightmap\n");
        return 1;
    }

    std::vector<elegy::vertex_t> vertices;
    std::vector<triangle_t>      triangles;
    for(auto const& lightmap_bsp : lightmap->bsps.span(magic))
    {
        auto const bsp = elegy::structure_bsp(map, *scenario, lightmap_bsp.bsp);
        if(!bsp)
            continue;
        auto name = map.name_of(*bsp.tag);
        if(name.find(options.name_filter) == std::string_view::npos)
            continue;

        auto const& geometry = lightmap_bsp.geometry;
        auto        meshes   = geometry.meshes.span(magic);
        auto compression = geometry.compression.span(magic);
        auto clusters =
            bsp->clusters.span(magic);
        auto instances = bsp->instances.span(magic);

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
                obj->add(map.string(instance.name), vertices, triangles, instance.transform());
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

/* DDS can carry the PC-compatible formats once linearized, and CTX1
 * decoded to two channels; the DXT3a family has no DDS equivalent */
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
    case ctx1:
        return dds_info{.bpp = 16, .pf_flags = RGB, .mask_r = 0x00FF, .mask_g = 0xFF00};
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
        auto const method = elegy::render_method_view::of(map, tag);
        if(!method)
            continue;
        count++;
        fmt::print("{} [{}]\n", name, map.group_of(tag)->group.str());

        /* Options picked per category of the definition */
        if(method.definition)
        {
            std::string line;
            for(auto const& [category, option] : method.options())
                line += fmt::format(
                    "{}{}={}",
                    line.empty() ? "" : " ",
                    category,
                    option.empty() ? std::string_view("?") : option);
            fmt::print("  {} ({})\n", map.name_of(*method.definition.tag), line);
        }

        for(size_t index = 0; index < map.span(method.method->postprocess).size(); index++)
        {
            auto const  view = elegy::render_method_view::of(map, tag, index);
            auto const& post = *view.postprocess;
            fmt::print(
                "  template {} blend {}\n",
                tag_name(map, post.shader_template),
                post.blend_mode.value());
            for(size_t i = 0; i < view.textures.size(); i++)
            {
                auto const slot = view.texture_name(i);
                fmt::print(
                    "    {:<24} {} (address {}, filter {}, transform {})\n",
                    slot.empty() ? std::string_view("?") : slot,
                    tag_name(map, view.textures[i].bitmap),
                    view.textures[i].address_mode,
                    view.textures[i].filter_mode,
                    view.textures[i].transform);
            }
            for(size_t i = 0; i < view.constants.size(); i++)
            {
                auto const name = view.constant_name(i);
                Vecf4      v    = view.constants[i];
                fmt::print(
                    "    {:<24} {:g} {:g} {:g} {:g}\n",
                    name.empty() ? std::string_view("?") : name,
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
        auto images = bitmap->images.span(magic);
        auto textures = bitmap->hardware_textures.span(magic);
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
            auto interleaved = bitmap->interleaved_textures.span(magic);
            bool const in_pair = textures.empty() && image.interleaved_interop >= 0 &&
                                 static_cast<size_t>(image.interleaved_interop) < interleaved.size();
            if(options.extract_dir.empty() || (i >= textures.size() && !in_pair))
                continue;
            auto type = image.type.value();
            if(type != elegy::bitmap_type_t::texture_2d && type != elegy::bitmap_type_t::array &&
               type != elegy::bitmap_type_t::cube_map)
                continue;
            auto dds = dds_format(format);
            if(!dds)
                continue;
            auto view = loader.load(
                in_pair ? interleaved[static_cast<size_t>(image.interleaved_interop)].resource
                        : textures[i].resource);
            if(!view.has_value())
            {
                fmt::print("    {}\n", resource_error_name(view.error()));
                continue;
            }
            auto stored = in_pair ? elegy::top_level_interleaved(view.value(), image)
                              : elegy::top_level(view.value(), image);
            u32  width = static_cast<u32>(image.width.value());
            u32  height = static_cast<u32>(image.height.value());
            /* Array slices and cube faces are written one file each */
            u32 slices = type == elegy::bitmap_type_t::array      ? std::max<u32>(image.depth, 1)
                         : type == elegy::bitmap_type_t::cube_map ? 6
                                                                  : 1;
            u32 stride = elegy::slice_stride(format, width, height);
            for(u32 z = 0; z < slices; z++)
            {
                if(size_t(z) * stride >= stored.size())
                    break;
                auto slice = stored.subspan(size_t(z) * stride);
                if(slice.empty() ||
                   !elegy::linearize(slice, format, width, height, image.xbox_flags, linear))
                {
                    fmt::print("    no pixel data\n");
                    break;
                }
                /* CTX1 has no DDS form; written decoded, x in red */
                if(format == elegy::bitmap_format_t::ctx1)
                {
                    std::vector<byte_t> blocks = std::move(linear);
                    elegy::decode_ctx1(blocks, width, height, linear);
                }
                auto path = fmt::format(
                    "{}/{}{}{}.dds",
                    options.extract_dir,
                    sanitize(name),
                    images.size() > 1 ? fmt::format("_{}", i) : std::string(),
                    slices > 1 ? fmt::format("_z{}", z) : std::string());
                auto file = make_dds(*dds, width, height, linear);
                std::ofstream(path, std::ios::binary)
                    .write(file.data(), static_cast<std::streamsize>(file.size()));
                written++;
            }
        }
        loader.trim();
        if(options.limit && count >= options.limit)
            break;
    }
    if(!options.extract_dir.empty())
        fmt::print("wrote {} DDS files to {}\n", written, options.extract_dir);
    return 0;
}

const char* compression_name(elegy::sound_compression_t compression)
{
    using enum elegy::sound_compression_t;
    switch(compression)
    {
    case none_be:
        return "pcm_be";
    case xbox_adpcm:
        return "xbox_adpcm";
    case ima_adpcm:
        return "ima_adpcm";
    case none_le:
        return "pcm_le";
    case wma:
        return "wma";
    case none:
        return "pcm";
    case xma:
        return "xma";
    case xma2:
        return "xma2";
    }
    return "?";
}

/* RIFF WAVE around XMA2 data, as ffmpeg and vgmstream read it */
std::vector<char> make_xma2_riff(elegy::xma2_format_t const& format, Span<byte_t const> data)
{
    std::vector<char> out;
    auto u32le = [&](u32 v) { append<u32>(out, blam::to_le(v)); };
    out.insert(out.end(), {'R', 'I', 'F', 'F'});
    u32le(static_cast<u32>(4 + 8 + sizeof(format) + 8 + data.size()));
    out.insert(out.end(), {'W', 'A', 'V', 'E', 'f', 'm', 't', ' '});
    u32le(sizeof(format));
    append(out, format);
    out.insert(out.end(), {'d', 'a', 't', 'a'});
    u32le(static_cast<u32>(data.size()));
    out.insert(out.end(), data.begin(), data.end());
    return out;
}

#if defined(OAF_XMA2_DECODER_ENABLED)
std::vector<char> make_wav(std::vector<i16> const& samples, u32 channels, u32 rate)
{
    std::vector<char> out;
    u32  bytes = static_cast<u32>(samples.size() * sizeof(i16));
    auto u32le = [&](u32 v) { append<u32>(out, blam::to_le(v)); };
    auto u16le = [&](u16 v) { append<u16>(out, blam::to_le(v)); };
    out.insert(out.end(), {'R', 'I', 'F', 'F'});
    u32le(36 + bytes);
    out.insert(out.end(), {'W', 'A', 'V', 'E', 'f', 'm', 't', ' '});
    u32le(16);
    u16le(1);
    u16le(static_cast<u16>(channels));
    u32le(rate);
    u32le(rate * channels * 2);
    u16le(static_cast<u16>(channels * 2));
    u16le(16);
    out.insert(out.end(), {'d', 'a', 't', 'a'});
    u32le(bytes);
    for(i16 sample : samples)
        append<i16>(out, blam::to_le(sample));
    return out;
}
#endif

int dump_sounds(
    elegy::map_container const& map,
    elegy::resource_loader&     loader,
    options_t const&            options)
{
    auto        magic       = map.magic();
    auto const  gestalt     = map.find<elegy::sound_gestalt_t>();
    if(!gestalt)
    {
        fmt::print("no sound gestalt\n");
        return 1;
    }
    if(!options.extract_dir.empty())
        std::filesystem::create_directories(options.extract_dir);
    auto codecs = gestalt->codecs.span(magic);
    auto names  = gestalt->names.span(magic);
    u32  count = 0, written = 0, missing = 0;
    for(auto const& tag : map.tags())
    {
        if(!tag.valid() || !map.is(tag, "snd!"))
            continue;
        auto name = map.name_of(tag);
        if(name.find(options.name_filter) == std::string_view::npos)
            continue;
        auto const* sound = map.tag_data<elegy::sound_t>(tag);
        if(!sound)
            continue;
        count++;
        auto const* codec = sound->codec.resolve(codecs);
        if(!codec)
            continue;
        auto view = loader.load(sound->resource);
        fmt::print(
            "{} [{} Hz x{} {}]{}\n",
            name,
            codec->rate(),
            codec->channels(),
            compression_name(codec->compression),
            view.has_value() ? "" : " (no resource)");
        if(!view.has_value())
            missing++;
        for(auto const& range : sound->pitch_ranges(*gestalt, magic))
        {
            auto const* range_name = range.name.resolve(names);
            fmt::print(
                "  pitch range {}: {} permutations\n",
                range_name ? map.string(*range_name) : std::string_view("?"),
                range.permutation_count());
            for(auto const& perm : elegy::permutations_of(range, *gestalt, magic))
            {
                auto const* perm_name = perm.name.resolve(names);
                u32 samples = perm.sample_count;
                auto bytes = view.has_value()
                                 ? elegy::permutation_data(perm, *gestalt, magic, view.value())
                                 : std::vector<byte_t>();
                fmt::print(
                    "    {:<24} {:7.2f} s, {} chunks, {} bytes\n",
                    perm_name ? map.string(*perm_name) : std::string_view("?"),
                    codec->rate() ? static_cast<f64>(samples) / codec->rate() : 0.0,
                    perm.chunk_count.value(),
                    bytes.size());
                if(options.extract_dir.empty() || bytes.empty() ||
                   codec->compression != elegy::sound_compression_t::xma2)
                    continue;
                auto format =
                    elegy::xma2_format_t::from(*codec, samples, static_cast<u32>(bytes.size()));
                auto file = make_xma2_riff(format, bytes);
                auto stem = fmt::format(
                    "{}/{}_{}",
                    options.extract_dir,
                    sanitize(name),
                    perm_name ? map.string(*perm_name) : std::string_view("perm"));
                std::ofstream(stem + ".xma", std::ios::binary)
                    .write(file.data(), static_cast<std::streamsize>(file.size()));
                written++;
#if defined(OAF_XMA2_DECODER_ENABLED)
                std::vector<i16>      pcm;
                oaf::format_t         pcm_format;
                oaf::decode::xma2::decoder xma2;
                if(xma2.decode_pcm(
                       gsl::span<const char>(
                           reinterpret_cast<char const*>(bytes.data()), bytes.size()),
                       {.channels = codec->channels(), .rate = codec->rate(), .samples = samples},
                       pcm,
                       pcm_format))
                {
                    auto wav = make_wav(pcm, codec->channels(), codec->rate());
                    std::ofstream(stem + ".wav", std::ios::binary)
                        .write(wav.data(), static_cast<std::streamsize>(wav.size()));
                } else
                    fmt::print("      XMA2 decode failed\n");
#endif
            }
        }
        loader.trim();
        if(options.limit && count >= options.limit)
            break;
    }
    fmt::print("{} sounds, {} without resource", count, missing);
    if(!options.extract_dir.empty())
        fmt::print(", wrote {} XMA files to {}", written, options.extract_dir);
    fmt::print("\n");
    return 0;
}

/* Collision polygons as an OBJ, fanned, through `transform` */
u32 write_collision(
    obj_writer& obj, std::string_view name, elegy::coll::bsp_view_t const& bsp, glm::mat4 const& transform)
{
    obj.out << "o " << name << "\n";
    for(auto const& v : bsp.vertices)
    {
        auto p = transform * glm::vec4(v.point.value(), 1.f);
        obj.out << "v " << p.x << " " << p.y << " " << p.z << "\n";
    }
    u32 polygons = 0;
    std::vector<u32> corners;
    for(u32 s = 0; s < bsp.surfaces.size(); s++)
    {
        corners.clear();
        if(bsp.walk_surface(s, [&](u32 v) { corners.push_back(v); }) < 3)
            continue;
        for(size_t i = 1; i + 1 < corners.size(); i++)
            obj.out << "f " << obj.base + corners[0] << " " << obj.base + corners[i] << " "
                    << obj.base + corners[i + 1] << "\n";
        polygons++;
    }
    obj.base += static_cast<u32>(bsp.vertices.size());
    return polygons;
}

/* Zone sets, the BSPs each loads, and the triggers that switch them */
int dump_zones(elegy::map_container const& map)
{
    auto const scenario = map.find<elegy::scenario_t>();
    if(!scenario)
        return 1;
    auto bsps = map.span(scenario->bsps);
    for(size_t i = 0; i < bsps.size(); i++)
    {
        auto const bsp = map.get<elegy::structure_bsp_t>(bsps[i].bsp);
        fmt::print("bsp {}: {}\n", i, bsp ? map.name_of(*bsp.tag) : std::string_view("?"));
    }
    auto sets = map.span(scenario->zone_sets);
    for(size_t i = 0; i < sets.size(); i++)
        fmt::print("zone set {} '{}': bsps {:#x} flags {:#x}\n", i, map.string(sets[i].name), sets[i].bsps.value(),
                   sets[i].flags.value());
    auto volumes = map.span(scenario->trigger_volumes);
    for(auto const& sw : map.span(scenario->zone_set_switch_triggers))
    {
        i16 const v = sw.trigger_volume;
        if(v < 0 || static_cast<size_t>(v) >= volumes.size())
            continue;
        auto const& tv = volumes[static_cast<size_t>(v)];
        Vecf3       p = tv.position, e = tv.extents, f = tv.forward, u = tv.up;
        Vecf3 const center = p + f * (e.x * .5f) + glm::cross(u, f) * (e.y * .5f) + u * (e.z * .5f);
        fmt::print("switch '{}': begin {} commit {} flags {:#x}, at ({:.1f}, {:.1f}, {:.1f}) extents ({:.1f}, {:.1f}, "
                   "{:.1f}) forward ({:.2f}, {:.2f}, {:.2f}) up ({:.2f}, {:.2f}, {:.2f}) center ({:.1f}, {:.1f}, {:.1f})\n",
                   map.string(tv.name), sw.begin_zone_set.value(), sw.commit_zone_set.value(), sw.flags.value(), p.x,
                   p.y, p.z, e.x, e.y, e.z, f.x, f.y, f.z, u.x, u.y, u.z, center.x, center.y, center.z);
    }
    for(auto const& start : map.span(scenario->starting_locations))
    {
        Vecf3 p = start.position;
        fmt::print("start at ({:.1f}, {:.1f}, {:.1f}) bsp {}\n", p.x, p.y, p.z, start.bsp.value());
    }
    return 0;
}

/* Lights: their shape and the raw functions colour and intensity follow */
int dump_lights(elegy::map_container const& map, options_t const& options)
{
    for(auto const& tag : map.tags())
    {
        if(!tag.valid() || !map.is(tag, elegy::light_t::group) ||
           map.name_of(tag).find(options.name_filter) == std::string_view::npos)
            continue;
        auto const* light = map.tag_data<elegy::light_t>(tag);
        if(!light)
            continue;
        fmt::print(
            "{}: type {} distance {} frustum near {} height {} fov {} diffusion {} smoothness {} spherical {} flags {:#x}\n",
            map.name_of(tag),
            static_cast<int>(light->type.value()),
            light->maximum_distance.value(),
            light->frustum_near_width.value(),
            light->frustum_height_scale.value(),
            light->frustum_field_of_view.value(),
            light->distance_diffusion.value(),
            light->angular_smoothness.value(),
            light->percent_spherical.value(),
            light->flags.value());
        for(auto const* f : {&light->color, &light->intensity})
        {
            auto bytes = f->function.data(map.magic());
            std::string hex;
            for(auto b : bytes)
                hex += fmt::format("{:02x}", b);
            fmt::print("  {} input '{}' range '{}': {} bytes {}\n", f == &light->color ? "color" : "intensity",
                       map.string(f->input), map.string(f->range), bytes.size(), hex);
        }
    }
    return 0;
}

/* Node hierarchy checks: default pose against the inverse bind matrices
 * under each quaternion/matrix convention, and what the meshes index */
int dump_nodes(elegy::map_container const& map, elegy::resource_loader& loader, options_t const& options)
{
    auto magic = map.magic();
    for(auto const& tag : map.tags())
    {
        if(!tag.valid() || !map.is(tag, "mode") ||
           map.name_of(tag).find(options.name_filter) == std::string_view::npos)
            continue;
        auto const* model = map.tag_data<elegy::render_model_t>(tag);
        auto        nodes = model ? map.span(model->nodes) : Span<elegy::node_t const>();
        if(nodes.empty())
            continue;
        fmt::print("{} [{} nodes]\n", map.name_of(tag), nodes.size());
        for(int conj = 0; conj < 2; conj++)
            for(int rows = 0; rows < 2; rows++)
            {
                std::vector<glm::mat4> world(nodes.size(), glm::mat4(1.f));
                f32                    worst = 0.f;
                for(size_t i = 0; i < nodes.size(); i++)
                {
                    auto const& n = nodes[i];
                    Vecf4       r = n.default_rotation.value();
                    glm::quat   q(r.w, r.x, r.y, r.z);
                    if(conj)
                        q = glm::conjugate(q);
                    glm::mat4 local = glm::translate(glm::mat4(1.f), n.default_translation.value()) *
                                      glm::mat4_cast(q);
                    i16 parent = n.parent.value();
                    world[i]   = parent >= 0 && static_cast<size_t>(parent) < i ? world[parent] * local
                                                                                : local;
                    glm::mat4 inverse(1.f);
                    inverse[0] = glm::vec4(n.inverse_forward.value(), 0.f);
                    inverse[1] = glm::vec4(n.inverse_left.value(), 0.f);
                    inverse[2] = glm::vec4(n.inverse_up.value(), 0.f);
                    inverse[3] = glm::vec4(n.inverse_position.value(), 1.f);
                    if(rows)
                    {
                        glm::mat3 basis = glm::transpose(glm::mat3(inverse));
                        inverse         = glm::mat4(basis);
                        inverse[3]      = glm::vec4(n.inverse_position.value(), 1.f);
                    }
                    glm::mat4 product = world[i] * inverse;
                    for(int c = 0; c < 4; c++)
                        for(int r2 = 0; r2 < 4; r2++)
                            worst = std::max(worst, std::abs(product[c][r2] - (c == r2 ? 1.f : 0.f)));
                }
                fmt::print("  quaternion {}, inverse as {}: worst |world * inverse - I| {:.5f}\n",
                           conj ? "conjugated" : "as stored", rows ? "rows" : "columns", worst);
            }
        auto node_maps   = map.span(model->geometry.per_mesh_node_map);
        auto meshes      = map.span(model->geometry.meshes);
        auto compression = map.span(model->geometry.compression);
        auto view        = loader.load(model->geometry.resource);
        if(!view.has_value())
            continue;
        std::vector<elegy::vertex_t> vertices;
        std::vector<triangle_t>      triangles;
        geometry_stats               stats;
        for(size_t m = 0; m < meshes.size(); m++)
        {
            u32 highest = 0;
            f32 weight  = 0.f;
            if(decode_mesh(view.value(), meshes[m], compression.empty() ? nullptr : &compression[0],
                           vertices, triangles, magic, stats))
                for(auto const& v : vertices)
                {
                    for(auto n : v.nodes)
                        highest = std::max<u32>(highest, n);
                    weight += v.weights.x + v.weights.y + v.weights.z + v.weights.w;
                }
            fmt::print("  mesh {}: type {} rigid node {} node map {} entries, {} vertices, highest node {}, mean weight sum {:.3f}\n",
                       m, static_cast<int>(meshes[m].vertex_type), meshes[m].rigid_node,
                       m < node_maps.size() ? map.span(node_maps[m]).size() : 0,
                       vertices.size(), highest, vertices.empty() ? 0.f : weight / vertices.size());
        }
        loader.trim();
    }
    if(auto const scenario = map.find<elegy::scenario_t>())
        for(auto const& [placement, entry] : elegy::placements(map, scenario->bipeds, scenario->biped_palette))
        {
            auto const* tag = entry ? map.get<elegy::object_t>(entry->object).tag : nullptr;
            if(!tag || map.name_of(*tag).find(options.name_filter) == std::string_view::npos)
                continue;
            for(auto const& pose : map.span(placement.object.node_orientations))
            {
                auto flags = map.span(pose.node_flags);
                auto rotations = map.span(pose.orientations);
                std::string nodes;
                for(size_t bit = 0; bit < flags.size() * 8; bit++)
                    if(flags[bit / 8] & (1u << (bit % 8)))
                        nodes += fmt::format(" {}", bit);
                fmt::print("{} placement: {} nodes, flagged{}; {} shorts:", map.name_of(*tag),
                           pose.node_count.value(), nodes, rotations.size());
                for(auto const& r : rotations)
                    fmt::print(" {}", r.value());
                fmt::print("\n");
            }
        }
    return 0;
}

/* A palette entry's physics model: its rigid bodies and their shapes */
void print_physics(elegy::map_container const& map, elegy::tag_reference const& object_ref)
{
    using namespace elegy::phys;
    auto       magic = map.magic();
    auto const phmo  = elegy::object_view::of(map, object_ref).physics_model;
    if(!phmo)
    {
        fmt::print("    no physics model\n");
        return;
    }
    auto const globals = map.find<globals_t>();
    fmt::print("    {} mass {}\n", map.name_of(*phmo.tag), phmo->mass.value());
    for(auto const& body : phmo->rigid_bodies.span(magic))
    {
        auto hulls  = hulls_of(*phmo, magic, body);
        u32  points = 0;
        for(auto const& hull : hulls)
            points += static_cast<u32>(hull.points.size());
        fmt::print(
            "      body node {} region {} permutation {} motion {} mass {} shape {}:{} -> {} hulls, {} points\n",
            body.node.value(),
            body.region.value(),
            body.permutation.value(),
            static_cast<int>(body.motion_type.value()),
            body.mass.value(),
            body.shape_type.value(),
            body.shape_index.value(),
            hulls.size(),
            points);
        if(body.shape_type.value() == static_cast<i16>(shape_type_t::list))
            for(auto const& child : phmo->list_shapes.span(magic))
                fmt::print("        list shape {}:{}\n", child.type.value(), child.index.value());
        for(auto const& hull : hulls)
        {
            auto const* material = phmo->material(magic, hull.material);
            auto const* global =
                material && globals ? globals->material(magic, material->global_name()) : nullptr;
            Vecf3 lo(1e9f), hi(-1e9f);
            for(auto const& p : hull.points)
                lo = glm::min(lo, p), hi = glm::max(hi, p);
            fmt::print(
                "        {} points ({:.3f}, {:.3f}, {:.3f})..({:.3f}, {:.3f}, {:.3f}) "
                "friction {} restitution {} mass {}; material {} -> {} (friction {}, "
                "restitution {}, density {})\n",
                hull.points.size(),
                lo.x, lo.y, lo.z, hi.x, hi.y, hi.z,
                hull.friction,
                hull.restitution,
                hull.mass,
                material ? map.string(material->name) : "?",
                material ? map.string(material->global_name()) : "?",
                global ? global->friction.value() : -1.f,
                global ? global->restitution.value() : -1.f,
                global ? global->density.value() : -1.f);
        }
    }
}

/* Objects placed in the scenario, by type, with their physics models */
int dump_objects(elegy::map_container const& map, options_t const& options)
{
    auto const scenario = map.find<elegy::scenario_t>();
    if(!scenario)
        return 1;
    auto list = [&](std::string_view kind, auto const& placements, auto const& palette_block) {
        auto items   = map.span(placements);
        auto palette = map.span(palette_block);
        fmt::print("{}: {} placed, {} in the palette\n", kind, items.size(), palette.size());
        for(size_t i = 0; i < palette.size(); i++)
        {
            auto const* tag = map.get<elegy::object_t>(palette[i].object).tag;
            if(!tag || map.name_of(*tag).find(options.name_filter) == std::string_view::npos)
                continue;
            u32 count = 0;
            for(auto const& item : items)
                count += item.object.palette.value() == static_cast<i16>(i);
            fmt::print("  {} [{}] x{}\n", map.name_of(*tag), map.group_of(*tag)->group.str(), count);
            for(auto const& item : items)
                if(item.object.palette.value() == static_cast<i16>(i))
                {
                    Vecf3 const at = item.object.position.value();
                    fmt::print(
                        "    at ({:.2f}, {:.2f}, {:.2f}) bsp {} flags {:#x} placement {:#x}\n",
                        at.x,
                        at.y,
                        at.z,
                        item.object.origin_bsp.value(),
                        item.object.bsp_flags.value(),
                        static_cast<u32>(item.object.placement_flags.value()));
                }
            print_physics(map, palette[i].object);
        }
    };
    list("scenery", scenario->scenery, scenario->scenery_palette);
    list("bipeds", scenario->bipeds, scenario->biped_palette);
    list("vehicles", scenario->vehicles, scenario->vehicle_palette);
    list("equipment", scenario->equipment, scenario->equipment_palette);
    list("weapons", scenario->weapons, scenario->weapon_palette);
    list("machines", scenario->machines, scenario->machine_palette);
    list("crates", scenario->crates, scenario->crate_palette);
    return 0;
}

int dump_collision(
    elegy::map_container const& map,
    elegy::resource_loader&     loader,
    options_t const&            options,
    obj_writer*                 obj)
{
    auto       magic    = map.magic();
    auto const scenario = map.find<elegy::scenario_t>();
    if(!scenario)
        return 1;
    for(auto const& ref : map.span(scenario->bsps))
    {
        auto const bsp     = map.get<elegy::structure_bsp_t>(ref.bsp);
        auto const bsp_tag = bsp.tag;
        if(!bsp || map.name_of(*bsp_tag).find(options.name_filter) == std::string_view::npos)
            continue;
        auto resources = loader.load(bsp->tag_resources);
        auto const* root = resources.has_value()
                               ? resources->root_as<elegy::structure_bsp_resources_t>()
                               : nullptr;
        if(!root)
        {
            fmt::print("{}: no tag resources\n", map.name_of(*bsp_tag));
            continue;
        }
        auto collision_bsps = resources->block(root->collision_bsps);
        auto definitions    = resources->block(root->instanced_geometry);
        auto leaves  = bsp->leaves.span(magic);
        auto clusters = bsp->clusters.span(magic);
        auto portals = bsp->cluster_portals.span(magic);
        fmt::print(
            "{} [{} collision bsps, {} instance definitions, {} clusters, {} portals, {} leaves]\n",
            map.name_of(*bsp_tag),
            collision_bsps.size(),
            definitions.size(),
            clusters.size(),
            portals.size(),
            leaves.size());
        if(collision_bsps.empty())
            continue;
        auto world = elegy::coll::bsp_view_t::from(resources.value(), collision_bsps[0]);
        if(world.nodes.empty() && collision_bsps[0].bsp3d_nodes.count.value() > 0)
            fmt::print(
                "  world: {} nodes declared, but the resource page is not in the loaded maps\n",
                collision_bsps[0].bsp3d_nodes.count.value());
        fmt::print(
            "  world: {} nodes, {} planes, {} leaves, {} bsp2d refs, {} bsp2d nodes, {} "
            "surfaces, {} edges, {} vertices\n",
            world.nodes.size(),
            world.planes.size(),
            world.leaves.size(),
            world.bsp2d_references.size(),
            world.bsp2d_nodes.size(),
            world.surfaces.size(),
            world.edges.size(),
            world.vertices.size());

        /* Spawns and scenery should sit in open space, in a cluster whose
         * bounds hold them */
        std::vector<Vecf3> points;
        for(auto const& start : scenario->starting_locations.span(magic))
            points.push_back(start.position.value());
        for(auto const& placement : scenario->scenery.span(magic))
            points.push_back(placement.object.position.value());
        /* world_bounds is level-wide in campaigns; the clusters bound one BSP */
        Vecf3 low(std::numeric_limits<f32>::max()), high(-std::numeric_limits<f32>::max());
        for(auto const& cluster : clusters)
            for(int k = 0; k < 3; k++)
            {
                low[k]  = std::min<f32>(low[k], cluster.bounds[k].min);
                high[k] = std::max<f32>(high[k], cluster.bounds[k].max);
            }
        u32 tested = 0, inside = 0, elsewhere = 0, solid = 0;
        for(auto p : points)
        {
            p.z += 0.3f;
            bool in_world = true;
            for(int k = 0; k < 3; k++)
                in_world = in_world && p[k] >= low[k] && p[k] <= high[k];
            if(!in_world)
                continue;
            tested++;
            auto const* cluster = elegy::cluster_at(map, *bsp, resources.value(), p);
            if(!cluster)
            {
                solid++;
                continue;
            }
            auto const& b = cluster->bounds;
            bool in = true;
            for(int k = 0; k < 3; k++)
                in = in && p[k] >= b[k].min - 0.5f && p[k] <= b[k].max + 0.5f;
            (in ? inside : elsewhere)++;
        }
        fmt::print(
            "  point location: {} of {} spawns/scenery in their cluster's bounds, {} elsewhere, "
            "{} in solid\n",
            inside,
            tested,
            elsewhere,
            solid);

        /* Portal vertices should lie on the collision plane they name */
        f32 worst = 0.f;
        u32 vertices = 0;
        for(auto const& portal : portals)
        {
            i32 pi = portal.plane;
            if(pi < 0 || static_cast<size_t>(pi) >= world.planes.size())
                continue;
            auto const& plane = world.planes[static_cast<size_t>(pi)];
            for(auto const& v : portal.vertices.span(magic))
            {
                worst = std::max(
                    worst,
                    std::abs(glm::dot(plane.normal.value(), v.value()) - plane.distance.value()));
                vertices++;
            }
        }
        fmt::print("  portals: {} vertices, furthest {:.4f} from their plane\n", vertices, worst);

        if(!obj)
            continue;
        u32 polygons = write_collision(*obj, "world", world, glm::mat4(1.f));
        u32 placed   = 0;
        for(auto const& instance : bsp->instances.span(magic))
        {
            auto const* definition = instance.definition.resolve(definitions);
            if(!definition)
                continue;
            auto const transform = instance.transform();
            polygons += write_collision(
                *obj,
                map.string(instance.name),
                elegy::coll::bsp_view_t::from(resources.value(), definition->collision),
                transform);
            placed++;
        }
        fmt::print("  wrote {} collision polygons ({} instances)\n", polygons, placed);
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
    if(options.shaders)
        rc |= dump_shaders(map, options);
    if(options.bitmaps)
        rc |= dump_bitmaps(map, *loader, options);
    if(options.sounds)
        rc |= dump_sounds(map, *loader, options);
    if(options.collision)
        rc |= dump_collision(map, *loader, options, obj.get());
    if(options.objects)
        rc |= dump_objects(map, options);
    if(options.nodes)
        rc |= dump_nodes(map, *loader, options);
    if(options.lights)
        rc |= dump_lights(map, options);
    if(options.zones)
        rc |= dump_zones(map);
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
        ("collision",
         "Check BSP collision and portals; with --obj, write the collision polygons")
        //
        ("objects", "List placed objects by type, with their physics models")
        //
        ("nodes", "Check render model node hierarchies and biped poses")
        //
        ("lights", "List lights and the functions their colour and intensity follow")
        //
        ("zones", "List zone sets and the triggers switching between them")
        //
        ("sounds",
         "List sounds; with -e, write each permutation as RIFF XMA2 (and WAV "
         "when built with BUILD_OPENAL_XMA2)")
        //
        ("obj", "Write decoded geometry to this OBJ", cxxopts::value<std::string>())
        //
        ("e,extract",
         "Extraction directory: --bitmaps writes DDS, --sounds XMA2",
         cxxopts::value<std::string>())
        //
        ("limit",
         "Cap on shaders/bitmaps/sounds visited, 0 = everything",
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
        .sounds       = arguments.count("sounds") > 0,
        .collision    = arguments.count("collision") > 0,
        .objects      = arguments.count("objects") > 0,
        .nodes        = arguments.count("nodes") > 0,
        .lights       = arguments.count("lights") > 0,
        .zones        = arguments.count("zones") > 0,
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
