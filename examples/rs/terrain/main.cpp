/*
 * RS2 terrain/loc parser — reads RuneScape 2 cache files, generates meshes.
 *
 * Layered:
 *   rs2_cache.h    — cache bytes → parsed structures (JAG, configs, models)
 *   rs2_geometry.h — region-level vertex/index buffers (terrain + locs),
 *                    colours baked per vertex; renderer-ready
 *   main.cpp       — CLI + PLY/OBJ+MTL writers on top of the geometry layer
 *
 * Output: terrain as ASCII PLY with per-vertex RGB, locs (buildings, trees,
 * furniture …) as OBJ+MTL with flat-coloured materials.
 *
 * Usage:
 *   RSTerrainParser --cache <dir> [--region-x <rx> --region-y <ry>] [--all]
 *                   [--plane <n>] [--output <ply>] [--buildings <obj>]
 *
 * Reference (BSD): https://github.com/RuneChamps/OSRS-World-Map
 */

#include "rs2_geometry.h"

#include "coffee/application/application_start.h"
#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>

#include <coffee/core/argument_handling.h>
#include "coffee/core/coffee_args.h"
#include <cxxopts.hpp>

#include <cstdio>
#include <filesystem>
#include <map>

using rs2::i32;
using rs2::Mesh;
using rs2::u32;

// Append src into dst, offsetting indices.
static void append_mesh(Mesh& dst, const Mesh& src)
{
    u32 base = u32(dst.vertices.size());
    dst.vertices.insert(
        dst.vertices.end(), src.vertices.begin(), src.vertices.end());
    dst.indices.reserve(dst.indices.size() + src.indices.size());
    for(u32 i : src.indices)
        dst.indices.push_back(base + i);
    dst.tri_texture.insert(
        dst.tri_texture.end(), src.tri_texture.begin(),
        src.tri_texture.end());
    dst.tri_overlay.insert(
        dst.tri_overlay.end(), src.tri_overlay.begin(),
        src.tri_overlay.end());
}

// ─────────────────────────────────────────────────────────── PLY output ──

static void write_ply(const std::string& path, const Mesh& mesh, int plane_idx)
{
    std::ofstream f(path);
    if(!f) throw std::runtime_error("cannot write: " + path);

    f << "ply\n"
         "format ascii 1.0\n"
         "comment RS2 terrain plane " << plane_idx << " (X=east Y=north Z=height)\n"
         "element vertex " << mesh.vertices.size() << "\n"
         "property float x\n"
         "property float y\n"
         "property float z\n"
         "property uchar red\n"
         "property uchar green\n"
         "property uchar blue\n"
         "element face " << mesh.indices.size() / 3 << "\n"
         "property list uchar int vertex_indices\n"
         "end_header\n";

    for(const auto& v : mesh.vertices)
        f << v.x << ' ' << v.y << ' ' << v.z << ' '
          << int(v.r) << ' ' << int(v.g) << ' ' << int(v.b) << '\n';

    for(size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
        f << "3 " << mesh.indices[i] << ' ' << mesh.indices[i + 1] << ' '
          << mesh.indices[i + 2] << '\n';
}

// ──────────────────────────────────────────────────── OBJ + MTL output ──

// Flat faces become one material per distinct baked colour; textured faces
// become one material per texture id, with the texture exported as a PNG
// next to the OBJ (textures/<id>.png) and real per-corner UVs.
static void write_obj(
    const std::string& obj_path, const Mesh& mesh, rs2::RegionLoader& loader)
{
    std::string mtl_path = obj_path.substr(0, obj_path.rfind('.')) + ".mtl";
    std::string dir      = obj_path.rfind('/') == std::string::npos
                               ? std::string{}
                               : obj_path.substr(0, obj_path.rfind('/') + 1);

    auto pack = [](const rs2::Vertex& v) {
        return u32(v.r) << 16 | u32(v.g) << 8 | v.b;
    };
    // material key: texture id offset past the 24-bit colour space
    constexpr u32 tex_key = 0x1000000;

    std::map<u32, std::vector<size_t>> by_material; // key → triangle index
    for(size_t t = 0; t * 3 + 2 < mesh.indices.size(); ++t)
    {
        i32 tex = t < mesh.tri_texture.size() ? mesh.tri_texture[t] : -1;
        u32 key = tex >= 0 ? tex_key + u32(tex)
                           : pack(mesh.vertices[mesh.indices[t * 3]]);
        by_material[key].push_back(t);
    }

    // ── export textures + MTL ──
    {
        bool made_dir = false;
        std::ofstream mf(mtl_path);
        for(const auto& [key, tris] : by_material)
        {
            char name[16];
            if(key >= tex_key)
            {
                int id = int(key - tex_key);
                snprintf(name, sizeof(name), "t%d", id);
                // UNLIT average colour as Kd (baseline for renderers that
                // modulate by the lit vertex colours) + real image as map_Kd
                rs2::Color avg =
                    loader.face_color(rs2::FACE_TEXTURED | u32(id));
                mf << "newmtl " << name << "\n"
                   << "Ka 0.1 0.1 0.1\n"
                   << "Kd " << avg.r / 255.f << ' ' << avg.g / 255.f << ' '
                   << avg.b / 255.f << "\nNs 10\n";
                if(auto sprite = loader.texture_sprite(id))
                {
                    if(!made_dir)
                    {
                        std::filesystem::create_directories(dir + "textures");
                        made_dir = true;
                    }
                    char png[64];
                    snprintf(png, sizeof(png), "textures/%d.png", id);
                    rs2::write_png_rgba(
                        dir + png, sprite->width, sprite->height,
                        sprite->rgba);
                    mf << "map_Kd " << png << "\nmap_d " << png << "\n";
                }
                mf << "\n";
            }
            else
            {
                snprintf(name, sizeof(name), "c%06X", key);
                mf << "newmtl " << name << "\n"
                   << "Ka 0.1 0.1 0.1\n"
                   << "Kd " << ((key >> 16) & 255) / 255.f << ' '
                   << ((key >> 8) & 255) / 255.f << ' ' << (key & 255) / 255.f
                   << "\nNs 10\n\n";
            }
        }
    }

    // ── OBJ ──
    {
        std::ofstream of(obj_path);
        of << "mtllib " << mtl_path.substr(mtl_path.rfind('/') + 1) << "\n";
        of << "o rs2\n";
        // v with the common vertex-colour extension (x y z r g b) — the
        // baked lit colours; widely supported (Blender, MeshLab)
        for(const auto& v : mesh.vertices)
            of << "v " << v.x << ' ' << v.y << ' ' << v.z << ' '
               << v.r / 255.f << ' ' << v.g / 255.f << ' ' << v.b / 255.f
               << "\n";
        // one vt per vertex, so f can always use v/vt with the same index
        for(const auto& v : mesh.vertices)
            of << "vt " << v.u << ' ' << 1.f - v.v << "\n";

        for(const auto& [key, tris] : by_material)
        {
            char name[16];
            if(key >= tex_key)
                snprintf(name, sizeof(name), "t%d", int(key - tex_key));
            else
                snprintf(name, sizeof(name), "c%06X", key);
            of << "usemtl " << name << "\n";
            for(size_t t : tris) // OBJ is 1-indexed
            {
                u32 a = mesh.indices[t * 3] + 1;
                u32 b = mesh.indices[t * 3 + 1] + 1;
                u32 c = mesh.indices[t * 3 + 2] + 1;
                of << "f " << a << '/' << a << ' ' << b << '/' << b << ' '
                   << c << '/' << c << "\n";
            }
        }
    }

    fprintf(stderr, "Wrote %s + %s (%zu verts, %zu faces, %zu materials)\n",
        obj_path.c_str(), mtl_path.c_str(), mesh.vertices.size(),
        mesh.indices.size() / 3, by_material.size());
}

// ────────────────────────────────────────────────────────────────── main ──

int coffee_main(int, char**)
{
    cxxopts::ParseResult arguments;
    {
        cxxopts::Options options("RSTerrainParser", "RS2 cache terrain parser");
        Coffee::BaseArgParser::GetBase(options);
        options.custom_help("[OPTION...]");

        options.add_options("Cache")
            ("cache",
             "Cache directory containing index dirs 0/ and 4/",
             cxxopts::value<std::string>())
            ;

        options.add_options("Region")
            ("region-x",
             "Region X coordinate (default 50 = Lumbridge)",
             cxxopts::value<int>()->default_value("50"))
            ("region-y",
             "Region Y coordinate (default 50 = Lumbridge)",
             cxxopts::value<int>()->default_value("50"))
            ("all",
             "Dump all regions in map_index (skips --region-x/y)")
            ("plane",
             "Terrain plane: 0=overworld, 1-3=upper floors",
             cxxopts::value<int>()->default_value("0"))
            ;

        options.add_options("Output")
            ("output",
             "Terrain output path (.ply vertex colours, .obj textured)",
             cxxopts::value<std::string>()->default_value("terrain.ply"))
            ("buildings",
             "OBJ output path for buildings/objects (omit to skip)",
             cxxopts::value<std::string>())
            ("links",
             "Write derived map links (stairs/ladders/caves) as JSON lines "
             "and exit",
             cxxopts::value<std::string>())
            ;

        auto& args = Coffee::GetInitArgs();
        arguments  = options.parse(args.size(), args.data());
        if(Coffee::BaseArgParser::PerformDefaults(options, args) >= 0)
            return 0;

        if(!arguments.count("cache"))
        {
            fprintf(stderr, "%s\n", options.help().c_str());
            return 1;
        }
    }

    std::string cache_dir   = arguments["cache"].as<std::string>();
    int         rx          = arguments["region-x"].as<int>();
    int         ry          = arguments["region-y"].as<int>();
    int         plane_idx   = arguments["plane"].as<int>();
    bool        all_regions = arguments.count("all") > 0;
    std::string output      = arguments["output"].as<std::string>();
    std::string buildings_path =
        arguments.count("buildings")
            ? arguments["buildings"].as<std::string>()
            : std::string{};

    fprintf(stderr, "Loading config archives...\n");
    rs2::RegionLoader loader(cache_dir);
    fprintf(stderr, "  %zu underlays, %zu loc definitions, %zu regions\n",
        loader.underlays().size(), loader.loc_defs().size(),
        loader.regions().size());

    if(arguments.count("links"))
    {
        auto links = loader.find_links();
        std::string path = arguments["links"].as<std::string>();
        std::ofstream f(path);
        if(!f)
            throw std::runtime_error("cannot write: " + path);
        size_t paired = 0;
        for(const auto& l : links)
        {
            f << "{\"kind\":\"" << rs2::to_string(l.kind) << "\",\"loc\":"
              << l.loc_id << ",\"name\":\"" << l.name << "\",\"action\":\""
              << l.action << "\",\"from\":[" << l.from_x << ',' << l.from_y
              << ',' << l.from_plane << "],\"to\":[" << l.to_x << ','
              << l.to_y << ',' << l.to_plane << "],\"paired\":"
              << (l.paired ? "true" : "false") << ",\"shape\":" << l.shape
              << ",\"rotation\":" << l.rotation << ",\"models\":[";
            for(size_t i = 0; i < l.model_ids.size(); ++i)
                f << (i ? "," : "") << l.model_ids[i];
            f << "]}\n";
            paired += l.paired;
        }
        fprintf(stderr, "Wrote %s (%zu links, %zu paired)\n", path.c_str(),
            links.size(), paired);
        return 0;
    }

    Mesh terrain, locs;

    if(all_regions)
    {
        size_t n = 0;
        for(const auto& ref : loader.regions())
        {
            auto geo = loader.load(ref.region_x, ref.region_y, plane_idx);
            if(!geo)
                continue;
            append_mesh(terrain, geo->terrain);
            if(!buildings_path.empty())
                append_mesh(locs, geo->locs);
            if(++n % 100 == 0)
                fprintf(stderr, "  %zu/%zu regions\n", n,
                    loader.regions().size());
        }
        fprintf(stderr, "  %zu regions loaded\n", n);
    }
    else
    {
        auto geo = loader.load(rx, ry, plane_idx);
        if(!geo)
        {
            fprintf(stderr, "Region (%d,%d) not found in map_index\n", rx, ry);
            return 1;
        }
        terrain = std::move(geo->terrain);
        locs    = std::move(geo->locs);
    }

    // terrain: OBJ (with textures + UVs) when the extension says so,
    // vertex-coloured PLY otherwise
    if(output.size() > 4 && output.substr(output.size() - 4) == ".obj")
        write_obj(output, terrain, loader);
    else
    {
        write_ply(output, terrain, plane_idx);
        fprintf(stderr, "Wrote %s (%zu verts, %zu faces)\n", output.c_str(),
            terrain.vertices.size(), terrain.indices.size() / 3);
    }

    if(!buildings_path.empty())
        write_obj(buildings_path, locs, loader);

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM_ARG(coffee_main)
