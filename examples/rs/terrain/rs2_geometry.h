/*
 * RS2 region geometry — mid level.
 *
 * Turns parsed cache data (rs2_cache.h) into renderer-ready meshes with
 * region-level granularity: one call per region yields vertex + index
 * buffers for the terrain and for all loc (object) models, colours baked
 * per vertex. Coordinates are world-space: X = east, Y = north, Z = up,
 * 128 units per tile, heights in game units (tile height × 8).
 *
 * The loader caches parsed regions/models/textures, and resolves heights
 * across region borders (including the procedural fallback for regions
 * without explicit heights), so independently loaded regions share seams.
 */
#pragma once

#include "peripherals/stl/decl_member_function.h"
#include "rs2_cache.h"

#include <hash-library/sha256.h>

#include <cctype>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>

namespace rs2 {

struct Vertex
{
    float x, y, z;
    u8    r, g, b; // baked flat colour (texture average for textured faces)
    u8    a = 255; // opacity (255 = opaque); from per-face model alphas
    float u = 0.f, v = 0.f;
};

static_assert(offsetof(Vertex, r) == 12, "Padding of rs2::Vertex not proper");
static_assert(offsetof(Vertex, a) == 15, "Padding of rs2::Vertex not proper");
static_assert(offsetof(Vertex, u) == 16, "Padding of rs2::Vertex not proper");
static_assert(sizeof(Vertex) == 24, "Padding of rs2::Vertex not proper");

// Every Mesh produced by the loader holds at most this many vertices —
// locs are chunked to it, terrain is structurally bounded well below it —
// so indices are u16 end to end.
constexpr u32 MESH_MAX_VERTICES = 65536;

// Coarse triangle classification, derived at build time from the placement
// shape and the loc definition's actions.
enum class TriClass : u8
{
    terrain = 0,
    scenery = 1,
    wall    = 2, // placement shapes 0-3 + 9 (diagonal)
    door    = 3, // def has an Open/Close action
    link    = 4, // def has a Climb/Enter action (see find_links)
    roof    = 5, // placement shape 12-21
    clip    = 6, // invisible collision skirts (blocked-tile boundaries)
};

// Semantic floor classes, derived from flo.dat entry NAMES at load time.
// The overlay ids themselves are cache data (b356: 1 = "cliff", 6 =
// "water", 19 = "lava") and differ between builds, so they cannot be a
// fixed enum — map tri_overlay through RegionLoader::floor_class().
enum class FloorClass : u8
{
    none = 0, // underlay / model geometry (tri_overlay == 0)
    water,    // "water", "gungywater", "water_fountain", …
    lava,
    invisible, // the magenta hidden marker entries
    other,     // roads, wooden floors, bricks, …
};

struct Mesh
{
    std::vector<Vertex> vertices;    // ≤ MESH_MAX_VERTICES
    std::vector<u16>    indices;     // triangle list
    std::vector<i32>    tri_texture; // per triangle: texture id, -1 = flat
    // per triangle: flo.dat overlay id (1-indexed) that produced it, 0 =
    // underlay/model geometry. RAW cache id — classify through
    // RegionLoader::floor_class(), or read the name via underlays().
    std::vector<i32> tri_overlay;
    // per triangle: loc def id that produced it (-1 = terrain) — lookup via
    // RegionLoader::loc_defs() gives name/actions (the client's right-click
    // data); matches MapLink::loc_id for teleport locs.
    std::vector<i32>      tri_loc;
    std::vector<TriClass> tri_class; // TriClass
};

// bitmask of traversal directions a loc's actions imply: 1 = up, 2 = down.
// Shared by find_links and the triangle classifier.
inline int loc_link_mask(LocDef const& def)
{
    int mask = 0;
    for(auto const& action : def.actions)
    {
        std::string a;
        for(char c : action)
            a += char(std::tolower((unsigned char)c));
        if(a == "climb-up" || a == "go-up" || a == "walk-up")
            mask |= 1;
        else if(
            a == "climb-down" || a == "go-down" || a == "walk-down" ||
            a == "enter" || a == "crawl-through" || a == "crawl-into")
            mask |= 2;
        else if(a == "climb")
            mask |= 3;
    }
    return mask;
}

inline bool loc_is_door(LocDef const& def)
{
    for(auto const& action : def.actions)
    {
        std::string a;
        for(char c : action)
            a += char(std::tolower((unsigned char)c));
        if(a == "open" || a == "close")
            return true;
    }
    return false;
}

// Lossless deduplication key over a vertex's full contents (positions are
// integer-derived; UVs quantised to 1/4096). Used as an unordered_map key
// with real equality — a hash is only the bucket, so distinct vertices are
// never welded (a lossy 64-bit hash-as-key silently merged different
// vertices, collapsing wall-end triangles when many share a material).
struct VertexKey
{
    i32 x, y, z, u, v;
    u32 rgba;

    bool operator==(VertexKey const& o) const
    {
        return x == o.x && y == o.y && z == o.z && u == o.u && v == o.v &&
               rgba == o.rgba;
    }
};

struct VertexKeyHash
{
    size_t operator()(VertexKey const& k) const
    {
        uint64_t h   = 1469598103934665603ull; // FNV-1a
        auto     mix = [&](uint64_t x) { h = (h ^ x) * 1099511628211ull; };
        mix(u32(k.x));
        mix(u32(k.y));
        mix(u32(k.z));
        mix(u32(k.u));
        mix(u32(k.v));
        mix(k.rgba);
        return size_t(h);
    }
};

inline VertexKey vertex_key(Vertex const& v)
{
    return VertexKey{
        i32(v.x),
        i32(v.y),
        i32(v.z),
        i32(v.u * 4096.f),
        i32(v.v * 4096.f),
        u32(v.r) << 24 | u32(v.g) << 16 | u32(v.b) << 8 | v.a};
}

template<typename T>
using VertexMap = std::unordered_map<VertexKey, T, VertexKeyHash>;

namespace sorting_method {

inline auto by_material(Mesh const& source, size_t index)
{
    return std::make_pair<i32, bool>(
        index < source.tri_texture.size() ? source.tri_texture[index] : -1,
        source.vertices[source.indices[index * 3]].a < 255);
}

inline size_t by_u16_chunk(Mesh const& /*source*/, size_t index)
{
    return size_t(index / 65'500);
}

} // namespace sorting_method

namespace filter_method {

inline bool always_true(Mesh const& /*source*/, size_t /*index*/)
{
    return true;
}

inline bool collidable_only(Mesh const& source, size_t index)
{
    // For physics, filter out doors and links, since we want to pass through
    // those Also ignore scenery for now to not overload Bullet
    switch(source.tri_class[index])
    {
    case TriClass::door:
    case TriClass::scenery:
        return false;
    default:
        return true;
    }
}

// Everything except the invisible collision skirts — the render set when
// clip meshes are in the same part list as the visible geometry.
inline bool renderable(Mesh const& source, size_t index)
{
    return source.tri_class[index] != TriClass::clip;
}

} // namespace filter_method

// Regroup any number of meshes into one mesh per material (texture id,
// -1 = flat colour) so a renderer draws one batch per material instead of
// one per (mesh × material). Transparent triangles (vertex opacity < 255)
// go into separate buckets so they can be drawn after opaque geometry.
// Output meshes are single-material (tri_texture uniform),
// vertex-deduplicated, and split at MESH_MAX_VERTICES like everything
// else; tri_overlay/tri_loc/tri_class are carried through.
template<typename Key = declreturntype(sorting_method::by_material)>
inline std::vector<Mesh> repack_by_material(
    std::vector<Mesh const*> const&           meshes,
    std::function<Key(Mesh const&, size_t)>&& sorter =
        sorting_method::by_material,
    std::function<bool(Mesh const&, size_t)>&& filter =
        filter_method::always_true)
{
    struct Bucket
    {
        Mesh           mesh;
        VertexMap<u32> dedupe;
    };

    std::map<Key, std::vector<Bucket>> buckets;

    for(Mesh const* src : meshes)
        for(size_t t = 0; t * 3 + 2 < src->indices.size(); ++t)
        {
            if(!filter(*src, t))
                continue;
            i32   mat = t < src->tri_texture.size() ? src->tri_texture[t] : -1;
            // bool  transparent = src->vertices[src->indices[t * 3]].a < 255;
            auto& list        = buckets[sorter(*src, t)];
            if(list.empty() ||
               list.back().mesh.vertices.size() + 3 > MESH_MAX_VERTICES)
                list.emplace_back();
            Bucket& b = list.back();

            for(int k = 0; k < 3; ++k)
            {
                Vertex const& v  = src->vertices[src->indices[t * 3 + k]];
                auto [it, fresh] = b.dedupe.emplace(
                    vertex_key(v), u32(b.mesh.vertices.size()));
                if(fresh)
                    b.mesh.vertices.push_back(v);
                b.mesh.indices.push_back(u16(it->second));
            }
            b.mesh.tri_texture.push_back(mat);
            b.mesh.tri_overlay.push_back(
                t < src->tri_overlay.size() ? src->tri_overlay[t] : 0);
            b.mesh.tri_loc.push_back(
                t < src->tri_loc.size() ? src->tri_loc[t] : -1);
            b.mesh.tri_class.push_back(
                t < src->tri_class.size() ? src->tri_class[t]
                                          : TriClass::terrain);
        }

    std::vector<Mesh> out;
    for(auto& [key, list] : buckets)
        for(auto& b : list)
            out.push_back(std::move(b.mesh));
    return out;
}

struct RegionGeometry
{
    int  region_x = 0, region_y = 0, plane = 0;
    Mesh terrain; // ≤ ~20k vertices, single mesh
    // loc geometry, split into ≤ MESH_MAX_VERTICES chunks at placement
    // granularity (models are never split across chunks)
    std::vector<Mesh> locs;
    // invisible collision skirts along blocked-tile boundaries (cliff
    // faces, water edges) — every triangle TriClass::clip; keep out of the
    // render set (filter_method::renderable), feed to physics
    Mesh clip;
};

// Inclusive region-coordinate extent of the map, with world-unit
// conversions (64 tiles per region, 128 units per tile).
struct MapBounds
{
    int min_region_x = 0, min_region_y = 0;
    int max_region_x = -1, max_region_y = -1; // empty when max < min

    float min_x() const
    {
        return float(min_region_x) * REGION_SIZE * 128.f;
    }

    float min_y() const
    {
        return float(min_region_y) * REGION_SIZE * 128.f;
    }

    float max_x() const
    {
        return float(max_region_x + 1) * REGION_SIZE * 128.f;
    }

    float max_y() const
    {
        return float(max_region_y + 1) * REGION_SIZE * 128.f;
    }
};

// A traversal link derived from loc actions. The cache stores no teleport
// table (that is server data); links are reconstructed from the
// "Climb-up"/"Climb-down"/"Enter" actions on loc definitions plus the RS2
// convention that dungeons lie 6400 tiles north of their surface spot
// (region_y + 100). `paired` = a loc with the opposite action was found
// near the destination, which makes the link near-certain.
struct MapLink
{
    enum class Kind
    {
        plane_up,
        plane_down,
        dungeon_down,
        dungeon_up
    };
    Kind        kind;
    int         loc_id;
    std::string name, action;
    int         from_x, from_y, from_plane;
    int         to_x, to_y, to_plane;
    bool        paired;
    // the placement rendering this link: shape type + rotation and the
    // model ids the def resolves to for that shape — the same geometry
    // that build_locs_mesh emitted at (from_x, from_y, from_plane)
    int              shape = 10, rotation = 0;
    std::vector<int> model_ids{};
    // world-space footprint box of the placement (mesh units: 128/tile,
    // height×8, z up): def size (swapped on odd rotation) in x/y, z from
    // the footprint's ground height up a nominal LINK_AABB_HEIGHT — meant
    // for overlap-trigger volumes, not exact model bounds
    float aabb_min[3]{}, aabb_max[3]{};
};

constexpr float LINK_AABB_HEIGHT = 256.f; // 2 tiles

inline const char* to_string(MapLink::Kind k)
{
    switch(k)
    {
    case MapLink::Kind::plane_up:
        return "plane_up";
    case MapLink::Kind::plane_down:
        return "plane_down";
    case MapLink::Kind::dungeon_down:
        return "dungeon_down";
    default:
        return "dungeon_up";
    }
}

// ── tile shape templates (client SceneTileModel) ──────────────────────────
// Template 0 = plain underlay tile; overlay tiles use template shape+1.
// Vertex codes: 1,3,5,7 = SW,SE,NE,NW corners; 2,4,6,8 = edge midpoints
// (S,E,N,W); 9-12 = inset edge midpoints; 13-16 = inset corners. Faces are
// (is_overlay, a, b, c) quads of indices into the template's vertex list;
// entries < 4 index the corner slots and rotate with the tile.

inline const std::vector<int> k_tile_shape_vertices[13] = {
    {1, 3, 5, 7},
    {1, 3, 5, 7},
    {1, 3, 5, 7},
    {1, 3, 5, 7, 6},
    {1, 3, 5, 7, 6},
    {1, 3, 5, 7, 6},
    {1, 3, 5, 7, 6},
    {1, 3, 5, 7, 2, 6},
    {1, 3, 5, 7, 2, 8},
    {1, 3, 5, 7, 2, 8},
    {1, 3, 5, 7, 11, 12},
    {1, 3, 5, 7, 11, 12},
    {1, 3, 5, 7, 13, 14},
};

inline const std::vector<int> k_tile_shape_faces[13] = {
    {0, 1, 2, 3, 0, 0, 1, 3},
    {1, 1, 2, 3, 1, 0, 1, 3},
    {0, 1, 2, 3, 1, 0, 1, 3},
    {0, 0, 1, 2, 0, 0, 2, 4, 1, 0, 4, 3},
    {0, 0, 1, 4, 0, 0, 4, 3, 1, 1, 2, 4},
    {0, 0, 4, 3, 1, 0, 1, 2, 1, 0, 2, 4},
    {0, 1, 2, 4, 1, 0, 1, 4, 1, 0, 4, 3},
    {0, 4, 1, 2, 0, 4, 2, 5, 1, 0, 4, 5, 1, 0, 5, 3},
    {0, 4, 1, 2, 0, 4, 2, 3, 0, 4, 3, 5, 1, 0, 4, 5},
    {0, 0, 4, 5, 1, 4, 1, 2, 1, 4, 2, 3, 1, 4, 3, 5},
    {0, 0, 1, 5, 0, 1, 4, 5, 0, 1, 2, 4, 1, 0, 5, 3, 1, 5, 4, 3, 1, 4, 2, 3},
    {1, 0, 1, 5, 1, 1, 4, 5, 1, 1, 2, 4, 0, 0, 5, 3, 0, 5, 4, 3, 0, 4, 2, 3},
    {1, 0, 5, 4, 1, 0, 1, 5, 0, 0, 4, 3, 0, 4, 5, 3, 0, 5, 2, 3, 0, 1, 2, 5},
};

// Vertex code → position within tile (units of 128) and the two corners
// whose height/colour it averages (corner order: 0=SW, 1=SE, 2=NE, 3=NW).
struct TileShapeVertex
{
    int px, py, ca, cb;
};

inline const TileShapeVertex k_tile_shape_vertex_pos[17] = {
    {0, 0, 0, 0},     // 0 (unused)
    {0, 0, 0, 0},     // 1  SW
    {64, 0, 1, 0},    // 2  S edge mid
    {128, 0, 1, 1},   // 3  SE
    {128, 64, 2, 1},  // 4  E edge mid
    {128, 128, 2, 2}, // 5  NE
    {64, 128, 3, 2},  // 6  N edge mid
    {0, 128, 3, 3},   // 7  NW
    {0, 64, 3, 0},    // 8  W edge mid
    {64, 32, 1, 0},   // 9  inset S mid
    {96, 64, 2, 1},   // 10 inset E mid
    {64, 96, 3, 2},   // 11 inset N mid
    {32, 64, 3, 0},   // 12 inset W mid
    {32, 32, 0, 0},   // 13 inset SW
    {96, 32, 1, 1},   // 14 inset SE
    {96, 96, 2, 2},   // 15 inset NE
    {32, 96, 3, 3},   // 16 inset NW
};

class RegionLoader
{
  public:
    explicit RegionLoader(std::string cache_dir)
        : m_cache_dir(std::move(cache_dir))
    {
        // Two on-disk layouts: classic dumps use numbered index dirs
        // (0/2.dat = config archive, 4/ = map squares, 1/ = models); beta
        // (~build 225) dumps use named archives ("config", "models",
        // "textures") plus a maps/ dir with m{x}_{y} / l{x}_{y} files.
        std::vector<u8> config_raw;
        try
        {
            config_raw = read_file(m_cache_dir + "/0/2.dat");
        } catch(const std::exception&)
        {
            config_raw = read_file(m_cache_dir + "/config");
            m_beta     = true;
        }
        auto arch2 = jag_load_archive(config_raw);

        // Content fingerprint for network handshakes: SHA-256 over the
        // decompressed config archive (flo/loc/obj…) and versionlist
        // archive. The versionlist holds every asset's CRC (models, maps,
        // anims, midis), so this one hash changes iff any content that
        // affects derived geometry changes — peers with matching signatures
        // are guaranteed identical worlds. Not a cache "version number"
        // (none exists in the bytes); a build-unique identity token.
        SHA256 sig;
        sig.add(arch2.data(), arch2.size());

        auto flo = jag_extract(arch2, "flo.dat");
        if(!flo)
            throw std::runtime_error("flo.dat not found in archive 2");
        m_underlays = parse_flo_dat(*flo);

        m_floor_classes.reserve(m_underlays.size());
        for(const auto& u : m_underlays)
        {
            std::string n;
            for(char c : u.name)
                n += char(std::tolower((unsigned char)c));
            FloorClass cls = FloorClass::other;
            if(n.find("water") != std::string::npos)
                cls = FloorClass::water;
            else if(n.find("lava") != std::string::npos)
                cls = FloorClass::lava;
            else if(n.find("invisible") != std::string::npos)
                cls = FloorClass::invisible;
            m_floor_classes.push_back(cls);
        }

        auto loc_dat = jag_extract(arch2, "loc.dat");
        auto loc_idx = jag_extract(arch2, "loc.idx");
        if(loc_dat && loc_idx)
            m_loc_defs = parse_loc_defs(*loc_dat, *loc_idx);

        if(!m_beta)
        {
            auto arch5 = jag_load_archive(read_file(m_cache_dir + "/0/5.dat"));
            sig.add(arch5.data(), arch5.size());
            m_signature = sig.getHash();

            auto mi = jag_extract(arch5, "map_index");
            if(!mi)
                throw std::runtime_error("map_index not found in archive 5");
            m_regions = parse_map_index(*mi);
        } else
        {
            // No versionlist archive pre-~234: the region list comes from
            // the maps/ directory itself, and the models archive stands in
            // for the versionlist in the content signature (it covers all
            // geometry-relevant assets alongside the config archive).
            auto models_arch =
                jag_load_archive(read_file(m_cache_dir + "/models"));
            sig.add(models_arch.data(), models_arch.size());
            m_signature = sig.getHash();
            if(!m_ob_models.load(models_arch))
                throw std::runtime_error("cannot parse ob_ model archive");

            // Some beta dumps (e.g. build 194) have no map data at all —
            // configs and models still load, the region list stays empty.
            if(std::filesystem::is_directory(m_cache_dir + "/maps"))
                for(const auto& entry :
                    std::filesystem::directory_iterator(m_cache_dir + "/maps"))
                {
                    int x, y;
                    if(sscanf(
                           entry.path().filename().c_str(), "m%d_%d", &x, &y) ==
                           2 &&
                       x >= 0 && x < 256 && y >= 0 && y < 256)
                        m_regions.push_back({u8(x), u8(y), 0, 0, 0});
                }
            std::sort(
                m_regions.begin(),
                m_regions.end(),
                [](const RegionRef& a, const RegionRef& b) {
                    return std::pair{a.region_x, a.region_y} <
                           std::pair{b.region_x, b.region_y};
                });
        }
        for(const auto& r : m_regions)
        {
            m_region_lookup[key(r.region_x, r.region_y)] = &r;
            auto grow                                    = [&](MapBounds& b) {
                if(b.max_region_x < b.min_region_x)
                    b = {r.region_x, r.region_y, r.region_x, r.region_y};
                b.min_region_x = std::min<int>(b.min_region_x, r.region_x);
                b.min_region_y = std::min<int>(b.min_region_y, r.region_y);
                b.max_region_x = std::max<int>(b.max_region_x, r.region_x);
                b.max_region_y = std::max<int>(b.max_region_y, r.region_y);
            };
            grow(m_bounds);
            // dungeons live at region_y + 100; below that is the surface
            if(r.region_y < 100)
                grow(m_surface_bounds);
        }
    }

    const std::vector<RegionRef>& regions() const
    {
        return m_regions;
    }

    // Extent of every region in the map index / of the surface world only
    // (underground regions sit at region_y + 100).
    const MapBounds& bounds() const
    {
        return m_bounds;
    }

    const MapBounds& surface_bounds() const
    {
        return m_surface_bounds;
    }

    const RegionRef* find_region(int rx, int ry) const
    {
        auto it = m_region_lookup.find(key(rx, ry));
        return it == m_region_lookup.end() ? nullptr : it->second;
    }

    // Full region geometry; nullopt when the region is not in map_index.
    std::optional<RegionGeometry> load(int rx, int ry, int plane)
    {
        const RegionRef* ref = find_region(rx, ry);
        if(!ref)
            return std::nullopt;
        RegionGeometry geo;
        geo.region_x = rx;
        geo.region_y = ry;
        geo.plane    = plane;
        geo.terrain  = build_terrain_mesh(rx, ry, plane);
        geo.locs     = build_locs_meshes(*ref, plane);
        geo.clip     = build_clip_mesh(rx, ry, plane);
        return geo;
    }

    // Tile-corner height in tile units at absolute tile (wx, wy),
    // cross-region, procedural fallback for missing data.
    int height_at(int wx, int wy, int plane)
    {
        int rx = wx / REGION_SIZE, ry = wy / REGION_SIZE;
        if(const Planes* p = region_planes(rx, ry))
            return (*p)[plane][wx - rx * REGION_SIZE][wy - ry * REGION_SIZE]
                .height;
        int h = ground_height(wx, wy);
        return plane == 0 ? h : std::max(0, h - 30 * plane);
    }

    // Tile at absolute coordinates, if its region has map data.
    const Tile* tile_at(int wx, int wy, int plane)
    {
        int rx = wx / REGION_SIZE, ry = wy / REGION_SIZE;
        if(const Planes* p = region_planes(rx, ry))
            return &(*p)[plane][wx - rx * REGION_SIZE][wy - ry * REGION_SIZE];
        return nullptr;
    }

    const std::vector<Underlay>& underlays() const
    {
        return m_underlays;
    }

    const std::vector<LocDef>& loc_defs() const
    {
        return m_loc_defs;
    }

    // SHA-256 hex of the cache's config + versionlist archives — a
    // build-unique identity token. Exchange it at connect time; peers must
    // match or their streamed geometry would diverge.
    const std::string& cache_signature() const
    {
        return m_signature;
    }

    // Semantic class of a tri_overlay value (raw flo.dat id) — the ids
    // differ between cache builds, the names don't.
    FloorClass floor_class(i32 overlay_id) const
    {
        if(overlay_id <= 0 || overlay_id > (i32)m_floor_classes.size())
            return FloorClass::none;
        return m_floor_classes[overlay_id - 1];
    }

    // Loc placements of one region (all planes).
    std::vector<Placement> placements(const RegionRef& ref)
    {
        try
        {
            auto raw = map_square(ref, true);
            return parse_map_locs(raw, ref.region_x, ref.region_y);
        } catch(const std::exception&)
        {
            return {}; // missing map file = ocean region
        }
    }

    // Resolved display colour for a model face colour value
    // (HSL or FACE_TEXTURED|texture_id).
    Color face_color(u32 col)
    {
        auto it = m_color_cache.find(col);
        if(it != m_color_cache.end())
            return it->second;
        Color c = (col & FACE_TEXTURED)
                      ? m_textures.get(m_cache_dir, int(col & 0xFFFF))
                      : hsl_to_rgb(u16(col));
        m_color_cache.emplace(col, c);
        return c;
    }

    // Parsed model by id (nullptr when missing/invalid). Cached.
    const RsModel* model(int id)
    {
        auto it = m_models.find(id);
        if(it != m_models.end())
            return it->second.valid ? &it->second : nullptr;
        RsModel parsed;
        try
        {
            if(m_beta)
                parsed = m_ob_models.decode(id);
            else
            {
                auto raw = read_file(
                    m_cache_dir + "/1/" + std::to_string(id) + ".dat");
                if(raw.size() >= 2 && raw[0] == 0x1F && raw[1] == 0x8B)
                    raw = gzip_decompress(raw.data(), raw.size());
                parsed = parse_model(raw);
            }
        } catch(const std::exception&)
        {
        }
        auto [ins, ok] = m_models.emplace(id, std::move(parsed));
        return ins->second.valid ? &ins->second : nullptr;
    }

    struct OverlayInfo
    {
        Color color;        // flat colour (texture average when textured)
        int   texture = -1; // texture id for real texturing
        bool  hidden  = false;
    };

    // Display info of an overlay floor (flo.dat id, 1-indexed);
    // hidden = the magenta 0xFF00FF invisible marker.
    OverlayInfo overlay_info(int overlay_id)
    {
        OverlayInfo info{{128, 128, 128}, -1, false};
        if(overlay_id <= 0 || overlay_id > (int)m_underlays.size())
            return info;
        const Underlay& o = m_underlays[overlay_id - 1];
        if(o.texture >= 0)
        {
            info.texture = o.texture;
            info.color   = m_textures.get(m_cache_dir, o.texture);
        } else if(o.r == 255 && o.g == 0 && o.b == 255)
            info.hidden = true;
        else
            info.color = Color{o.r, o.g, o.b};
        return info;
    }

    // full texture image, for PNG export
    std::optional<SpriteRgba> texture_sprite(int id)
    {
        return m_textures.sprite(m_cache_dir, id);
    }

    // ── mesh builders ────────────────────────────────────────────────

    // Is this tile a bridge tile? The plane-1 render flag bit 2 means the
    // plane-1 tile is the bridge deck drawn at ground level, with plane 0
    // (the water) below it; locs on upper planes shift down one plane.
    bool is_bridge(int wx, int wy)
    {
        const Tile* t1 = tile_at(wx, wy, 1);
        return t1 && (t1->settings & 2);
    }

    // Terrain via the client's tile shape templates: plain tiles are simple
    // quads with smooth-blended underlay corner colours; tiles with an
    // overlay (paths, roads, water, floors) split into underlay + flat
    // overlay faces along the template's shape. Textured overlays carry
    // world-aligned UVs (one repeat per tile) and the texture id per
    // triangle. Upper planes emit only tiles that have a floor; bridge
    // decks (plane-1 data) are folded into the plane-0 mesh.
    Mesh build_terrain_mesh(int rx, int ry, int plane)
    {
        Mesh           mesh;
        VertexMap<u32> vert_index;
        auto           add_vertex =
            [&](float x, float y, float z, Color c, float u, float v) -> u16 {
            Vertex vert{x, y, z, c.r, c.g, c.b, 255, u, v};
            auto [it, fresh] =
                vert_index.emplace(vertex_key(vert), u32(mesh.vertices.size()));
            if(fresh)
                mesh.vertices.push_back(vert);
            return u16(it->second);
        };

        // Blended underlay HSL at a tile (client blendUnderlays): weighted
        // box blur over ±5 tiles; -1 = no underlay in the window. Memoised
        // per build; tile_at crosses region borders so seams match.
        constexpr int                blend_radius = 5;
        std::unordered_map<u64, i32> blend_cache;
        auto blended_hsl = [&](int wx, int wy, int pl) -> i32 {
            u64  key = u64(u32(wx)) << 40 | u64(u32(wy)) << 8 | u32(pl);
            auto it  = blend_cache.find(key);
            if(it != blend_cache.end())
                return it->second;
            int hues = 0, sats = 0, lights = 0, mult = 0, num = 0;
            for(int dy = -blend_radius; dy <= blend_radius; ++dy)
                for(int dx = -blend_radius; dx <= blend_radius; ++dx)
                {
                    const Tile* t = tile_at(wx + dx, wy + dy, pl);
                    if(!t || t->underlay_id <= 0 ||
                       t->underlay_id > (int)m_underlays.size())
                        continue;
                    const Underlay& u = m_underlays[t->underlay_id - 1];
                    hues += u.hue;
                    sats += u.sat;
                    lights += u.light;
                    mult += u.hue_mult;
                    ++num;
                }
            i32 hsl = -1;
            if(num > 0 && mult > 0)
                hsl = pack_hsl(hues * 256 / mult, sats / num, lights / num);
            blend_cache.emplace(key, hsl);
            return hsl;
        };

        // Corner lightness (client Scene.calculateTileLights): heightmap
        // slope normal · light dir (-50,-10,-50), ambient 96. Client
        // heights are negative-up game units; ours are positive-up tile
        // units, hence the -8 scale.
        auto corner_light = [&](int wx, int wy, int pl) -> int {
            int dxc =
                -8 * (height_at(wx + 1, wy, pl) - height_at(wx - 1, wy, pl));
            int dyc =
                -8 * (height_at(wx, wy + 1, pl) - height_at(wx, wy - 1, pl));
            int len =
                int(std::sqrt(double(dxc) * dxc + double(dyc) * dyc + 65536.0));
            int nx = (dxc << 8) / len;
            int ny = 65536 / len;
            int nz = (dyc << 8) / len;
            // |light| = 71, intensity = (71*768)>>8 = 213
            return (nx * -50 + ny * -10 + nz * -50) / 213 + 96;
        };

        auto scale_color = [](Color c, int light) {
            auto s = [&](u8 ch) {
                return u8(std::min(255, int(ch) * light >> 7));
            };
            return Color{s(c.r), s(c.g), s(c.b)};
        };

        // Corner colour: blended underlay HSL adjusted by the corner light
        // (Gouraud — interpolation across the triangle does the rest);
        // falls back to the local tile's flat colour where no underlay is
        // in range.
        auto corner_color = [&](int wx, int wy, int pl, int light) -> Color {
            if(i32 hsl = blended_hsl(wx, wy, pl); hsl >= 0)
                return hsl_to_rgb(adjust_hsl_lightness(u16(hsl), light));
            const Tile* t = tile_at(wx, wy, pl);
            if(!t)
                t = tile_at(
                    std::min(wx, (rx + 1) * REGION_SIZE - 1),
                    std::min(wy, (ry + 1) * REGION_SIZE - 1),
                    pl);
            Color base = t ? underlay_color(t->underlay_id, m_underlays)
                           : Color{128, 128, 128};
            return scale_color(base, light);
        };

        // Emit one tile's template geometry, heights/colours from plane pl.
        auto emit_tile = [&](const Tile& tile, int wx, int wy, int pl) {
            // corners: 0=SW 1=SE 2=NE 3=NW
            const int cdx[4] = {0, 1, 1, 0};
            const int cdy[4] = {0, 0, 1, 1};
            float     hs[4];
            Color     cs[4];
            int       ls[4];
            for(int k = 0; k < 4; ++k)
            {
                hs[k] = float(height_at(wx + cdx[k], wy + cdy[k], pl)) * 8.f;
                ls[k] = corner_light(wx + cdx[k], wy + cdy[k], pl);
                cs[k] = corner_color(wx + cdx[k], wy + cdy[k], pl, ls[k]);
            }

            int         tpl = 0, rot = 0;
            OverlayInfo ov;
            if(tile.overlay_id != 0)
            {
                tpl = tile.overlay_shape + 1;
                rot = tile.overlay_rotation;
                ov  = overlay_info(tile.overlay_id);
            }

            // template vertices: rotate the code, then place. Overlay
            // colour is lit per vertex too (client adjustOverlayLight).
            struct TV
            {
                float x, y, z;
                Color under, over;
            };
            TV          tvs[8];
            const auto& codes = k_tile_shape_vertices[tpl];
            for(size_t i = 0; i < codes.size(); ++i)
            {
                int code = codes[i];
                if((code & 1) == 0 && code <= 8)
                    code = ((code - rot - rot - 1) & 7) + 1;
                else if(code > 8 && code <= 12)
                    code = ((code - 9 - rot) & 3) + 9;
                else if(code > 12)
                    code = ((code - 13 - rot) & 3) + 13;

                const TileShapeVertex& vp    = k_tile_shape_vertex_pos[code];
                int                    light = (ls[vp.ca] + ls[vp.cb]) / 2;
                tvs[i]                       = {
                    float(wx) * 128.f + float(vp.px),
                    float(wy) * 128.f + float(vp.py),
                    (hs[vp.ca] + hs[vp.cb]) * 0.5f,
                    Color{
                        u8((cs[vp.ca].r + cs[vp.cb].r) / 2),
                        u8((cs[vp.ca].g + cs[vp.cb].g) / 2),
                        u8((cs[vp.ca].b + cs[vp.cb].b) / 2)},
                    scale_color(ov.color, light)};
            }

            const auto& tf = k_tile_shape_faces[tpl];
            for(size_t i = 0; i + 3 < tf.size(); i += 4)
            {
                bool is_overlay = tf[i] == 1;
                if(is_overlay && ov.hidden)
                    continue;
                int idx[3] = {tf[i + 1], tf[i + 2], tf[i + 3]};
                for(int& v : idx) // corner slots rotate with the tile
                    if(v < 4)
                        v = (v - rot) & 3;
                bool textured = is_overlay && ov.texture >= 0;
                for(int v : idx)
                {
                    const TV& tv = tvs[v];
                    // world-aligned UVs: one texture repeat per tile
                    float u  = textured ? tv.x / 128.f : 0.f;
                    float vv = textured ? tv.y / 128.f : 0.f;
                    mesh.indices.push_back(add_vertex(
                        tv.x,
                        tv.y,
                        tv.z,
                        is_overlay ? tv.over : tv.under,
                        u,
                        vv));
                }
                mesh.tri_texture.push_back(textured ? ov.texture : -1);
                mesh.tri_overlay.push_back(is_overlay ? tile.overlay_id : 0);
                mesh.tri_loc.push_back(-1);
                mesh.tri_class.push_back(TriClass::terrain);
            }
        };

        for(int ty = 0; ty < REGION_SIZE; ++ty)
            for(int tx = 0; tx < REGION_SIZE; ++tx)
            {
                int         wx   = rx * REGION_SIZE + tx;
                int         wy   = ry * REGION_SIZE + ty;
                const Tile* tile = tile_at(wx, wy, plane);
                if(!tile)
                    continue;

                bool bridge = (plane == 0 || plane == 1) && is_bridge(wx, wy);
                if(plane == 1 && bridge)
                    continue; // deck already drawn as part of plane 0
                // upper planes: only tiles that actually have a floor
                if(plane > 0 && tile->underlay_id == 0 && tile->overlay_id == 0)
                    continue;

                emit_tile(*tile, wx, wy, plane);
                if(plane == 0 && bridge)
                    if(const Tile* deck = tile_at(wx, wy, 1))
                        emit_tile(*deck, wx, wy, 1);
            }
        return mesh;
    }

    // All loc models of a region on one plane, transformed to world space,
    // face colours baked into (deduplicated) vertices; textured faces carry
    // client-exact PMN-projected UVs and their texture id per triangle.
    // Output is chunked so no mesh exceeds MESH_MAX_VERTICES.
    std::vector<Mesh> build_locs_meshes(const RegionRef& ref, int plane)
    {
        std::vector<Mesh> chunks(1);
        Mesh*             mesh = &chunks.back();
        // dedupe on (position, colour, uv); positions are integer-derived.
        // Indices are chunk-local, so the map resets per chunk.
        VertexMap<u32> vert_index;
        auto           add_vertex =
            [&](float x, float y, float z, Color c, u8 a, float u, float v)
            -> u16 {
            Vertex vert{x, y, z, c.r, c.g, c.b, a, u, v};
            auto [it, fresh] = vert_index.emplace(
                vertex_key(vert), u32(mesh->vertices.size()));
            if(fresh)
                mesh->vertices.push_back(vert);
            return u16(it->second);
        };

        for(const auto& pl : placements(ref))
        {
            // Bridge tiles shift upper-plane locs down one plane (client
            // decodeLocs: level-- when renderFlags[1] & 2). Heights still
            // come from the original plane (the deck's own heightmap).
            int eff_plane = pl.plane;
            if(eff_plane > 0 && is_bridge(pl.world_x, pl.world_y))
                --eff_plane;
            if(eff_plane != plane)
                continue;
            if(pl.loc_id < 0 || pl.loc_id >= (int)m_loc_defs.size())
                continue;
            const LocDef& def = m_loc_defs[pl.loc_id];

            // Wall corners (type 2) are TWO wall pieces forming an L: the
            // client (SceneBuilder WALL_CORNER) draws the model twice —
            // mirrored at `rotation` (passed as rotation+4) and normal at
            // (rotation+1)&3. Rendering only one leg loses the final
            // segment of every wall run that ends in a corner.
            RsModel mdl;
            bool    ok;
            if(pl.type == 2)
            {
                RsModel leg2;
                ok = build_loc_model(
                         def,
                         pl.type,
                         pl.rotation + 4,
                         [&](int id) { return model(id); },
                         mdl) &&
                     build_loc_model(
                         def,
                         pl.type,
                         (pl.rotation + 1) & 3,
                         [&](int id) { return model(id); },
                         leg2);
                if(ok)
                {
                    int base     = (int)mdl.verts.size();
                    int tex_base = (int)mdl.tex_pmn.size();
                    mdl.verts.insert(
                        mdl.verts.end(), leg2.verts.begin(), leg2.verts.end());
                    for(const auto& fc : leg2.faces)
                        mdl.faces.push_back(
                            {base + fc[0], base + fc[1], base + fc[2]});
                    mdl.colors.insert(
                        mdl.colors.end(),
                        leg2.colors.begin(),
                        leg2.colors.end());
                    mdl.alphas.insert(
                        mdl.alphas.end(),
                        leg2.alphas.begin(),
                        leg2.alphas.end());
                    for(const auto& t : leg2.tex_pmn)
                        mdl.tex_pmn.push_back(
                            {base + t[0], base + t[1], base + t[2]});
                    for(i32 tc : leg2.tex_coord)
                        mdl.tex_coord.push_back(tc < 0 ? -1 : tc + tex_base);
                }
            } else
                ok = build_loc_model(
                    def,
                    pl.type,
                    pl.rotation,
                    [&](int id) { return model(id); },
                    mdl);
            if(std::getenv("RS2_LOC_DEBUG") && (pl.type <= 3 || pl.type == 9) &&
               plane == 0)
                fprintf(
                    stderr,
                    "WALL loc=%d shape=%d rot=%d world=(%d,%d) size=%dx%d "
                    "types=%zu %s\n",
                    pl.loc_id,
                    pl.type,
                    pl.rotation,
                    pl.world_x,
                    pl.world_y,
                    def.size_x,
                    def.size_y,
                    def.types.size(),
                    ok ? "OK" : "FAIL");
            // RS2_LOC_DEBUG_ID=<loc id>: trace every placement of one loc
            // (any type/plane) through the model build
            if(char const* dbg_id = std::getenv("RS2_LOC_DEBUG_ID");
               dbg_id && atoi(dbg_id) == pl.loc_id)
                fprintf(
                    stderr,
                    "LOC %d '%s' shape=%d rot=%d plane=%d world=(%d,%d) "
                    "tris=%zu %s\n",
                    pl.loc_id,
                    def.name.c_str(),
                    pl.type,
                    pl.rotation,
                    pl.plane,
                    pl.world_x,
                    pl.world_y,
                    mdl.faces.size(),
                    ok ? "OK" : "FAIL");
            if(!ok)
                continue;

            // classification for every triangle of this placement;
            // priority: link > door > wall > scenery
            auto tri_class = TriClass::scenery;
            if(loc_link_mask(def) != 0)
                tri_class = TriClass::link;
            else if(loc_is_door(def))
                tri_class = TriClass::door;
            else if(pl.type <= 3 || pl.type == 9)
                tri_class = TriClass::wall;
            else if(pl.type >= 12 && pl.type <= 21)
                tri_class = TriClass::roof;

            // start a new chunk when this model could overflow u16 indices
            // (conservative bound: 3 unique vertices per face)
            if(!mesh->vertices.empty() &&
               mesh->vertices.size() + mdl.faces.size() * 3 > MESH_MAX_VERTICES)
            {
                chunks.emplace_back();
                mesh = &chunks.back();
                vert_index.clear();
            }

            // World origin: centre of the tile footprint (client:
            // entityX = tileX*128 + sizeX*64), sizes swapped on odd rotations.
            int size_x = def.size_x, size_y = def.size_y;
            if(pl.rotation & 1)
                std::swap(size_x, size_y);

            float ox = float(pl.world_x) * 128.f + float(size_x) * 64.f;
            float oy = float(pl.world_y) * 128.f + float(size_y) * 64.f;

            // Ground height: average of the footprint's centre corners
            // (client centerHeight computation).
            int   sx = pl.world_x + (size_x >> 1);
            int   ex = pl.world_x + ((size_x + 1) >> 1);
            int   sy = pl.world_y + (size_y >> 1);
            int   ey = pl.world_y + ((size_y + 1) >> 1);
            float gz =
                float(
                    height_at(sx, sy, pl.plane) + height_at(ex, sy, pl.plane) +
                    height_at(sx, ey, pl.plane) + height_at(ex, ey, pl.plane)) *
                8.f / 4.f;

            // Ground decor (type 22: gnome walkway decks, bridge planks,
            // trapdoors, mud patches) sits exactly at floor height — the
            // client draws it after the floor in painter's order; a
            // z-buffered renderer needs a tiny lift or flat decor z-fights
            // the terrain underneath.
            float lift = pl.type == 22 ? 2.f : 0.f;

            // client model lighting on the final transformed model,
            // quantised to 8-step buckets to keep material counts sane
            std::vector<int> lights = model_vertex_lights(mdl);
            for(int& l : lights)
                l = std::min(126, std::max(2, l)) & ~7;

            // contoured ground (loc op 21, cave walls etc.): every vertex
            // follows the bilinearly interpolated heightmap at its own
            // world position, so adjacent models connect seamlessly
            auto ground_at = [&](float wxf, float wyf) -> float {
                int   gx  = int(std::floor(wxf / 128.f));
                int   gy  = int(std::floor(wyf / 128.f));
                float fx  = wxf / 128.f - float(gx);
                float fy  = wyf / 128.f - float(gy);
                float h00 = float(height_at(gx, gy, pl.plane));
                float h10 = float(height_at(gx + 1, gy, pl.plane));
                float h01 = float(height_at(gx, gy + 1, pl.plane));
                float h11 = float(height_at(gx + 1, gy + 1, pl.plane));
                return (h00 * (1 - fx) * (1 - fy) + h10 * fx * (1 - fy) +
                        h01 * (1 - fx) * fy + h11 * fx * fy) *
                       8.f;
            };

            for(size_t fi = 0; fi < mdl.faces.size(); ++fi)
            {
                const auto& fc = mdl.faces[fi];
                if(fc[0] == fc[1] || fc[1] == fc[2] || fc[0] == fc[2])
                    continue;
                u32 raw = fi < mdl.colors.size() ? mdl.colors[fi] : 0;
                int tex = raw & FACE_TEXTURED ? int(raw & 0xFFFF) : -1;
                // client alpha: 0 = opaque, larger = more transparent
                u8 opacity =
                    u8(255 - (fi < mdl.alphas.size() ? mdl.alphas[fi] : 0));

                float us[3] = {}, vs[3] = {};
                if(tex >= 0)
                    model_face_uvs(mdl, fi, us, vs);
                Color avg = face_color(raw);

                for(int k = 0; k < 3; ++k)
                {
                    const auto& v     = mdl.verts[fc[k]];
                    int         light = lights[fc[k]];
                    Color       col;
                    if(tex >= 0)
                        // client multiplies texels by the face lightness
                        col = Color{
                            u8(avg.r * light >> 7),
                            u8(avg.g * light >> 7),
                            u8(avg.b * light >> 7)};
                    else
                        col = hsl_to_rgb(adjust_hsl_lightness(u16(raw), light));

                    float wx = ox + float(v[0]);
                    float wy = oy + float(v[2]);
                    float base =
                        (def.contoured_ground ? ground_at(wx, wy) : gz) + lift;
                    // model: x=east, y=vertical (negative=up), z=north
                    mesh->indices.push_back(add_vertex(
                        wx,
                        wy,
                        base - float(v[1]),
                        col,
                        opacity,
                        us[k],
                        vs[k]));
                }
                mesh->tri_texture.push_back(tex);
                mesh->tri_overlay.push_back(0);
                mesh->tri_loc.push_back(pl.loc_id);
                mesh->tri_class.push_back(tri_class);
            }
        }
        if(chunks.back().indices.empty())
            chunks.pop_back();
        return chunks;
    }

    // ── collision skirts ────────────────────────────────────────────

    // Invisible collision-only geometry: vertical quads along the boundary
    // between blocked tiles (terrain settings bit 1 — cliff faces, water)
    // and walkable ones, so triangle-mesh physics stops bipeds where the
    // game's tile flags do (steep "impassable" hills are ordinary walkable
    // slopes to a physics engine). Never rendered; consumed by the physics
    // repack (every triangle TriClass::clip, both windings so collision
    // works from either side).
    Mesh build_clip_mesh(int rx, int ry, int plane)
    {
        Mesh mesh;

        // blocked state of a tile: 1 = blocked, 0 = walkable,
        // -1 = no map data (world edge — emit nothing against those)
        auto blocked = [&](int wx, int wy) -> int {
            // bridge decks (plane-1 data walked at ground level) use the
            // deck's flags, not the water underneath
            if(plane == 0 && is_bridge(wx, wy))
            {
                const Tile* deck = tile_at(wx, wy, 1);
                return deck ? (deck->settings & 1) : -1;
            }
            const Tile* t = tile_at(wx, wy, plane);
            return t ? (t->settings & 1) : -1;
        };

        auto quad =
            [&](float x0, float y0, float x1, float y1, float zb, float zt) {
                auto add = [&](float x, float y, float z) -> u16 {
                    mesh.vertices.push_back(
                        Vertex{x, y, z, 0, 0, 0, 255, 0.f, 0.f});
                    return u16(mesh.vertices.size() - 1);
                };
                u16       a = add(x0, y0, zb), b = add(x1, y1, zb);
                u16       c = add(x1, y1, zt), d = add(x0, y0, zt);
                const u16 idx[] = {a, b, c, a, c, d, a, c, b, a, d, c};
                mesh.indices.insert(
                    mesh.indices.end(), std::begin(idx), std::end(idx));
                for(int i = 0; i < 4; ++i)
                {
                    mesh.tri_texture.push_back(-1);
                    mesh.tri_overlay.push_back(0);
                    mesh.tri_loc.push_back(-1);
                    mesh.tri_class.push_back(TriClass::clip);
                }
            };

        // neighbour delta + the two tile corners forming the shared edge
        struct Edge
        {
            int dx, dy, cx0, cy0, cx1, cy1;
        };

        static constexpr Edge edges[] = {
            {-1, 0, 0, 0, 0, 1}, // west
            {1, 0, 1, 0, 1, 1},  // east
            {0, -1, 0, 0, 1, 0}, // south
            {0, 1, 0, 1, 1, 1},  // north
        };

        for(int x = 0; x < REGION_SIZE; ++x)
            for(int y = 0; y < REGION_SIZE; ++y)
            {
                int wx = rx * REGION_SIZE + x, wy = ry * REGION_SIZE + y;
                if(blocked(wx, wy) != 1)
                    continue;
                for(const Edge& e : edges)
                {
                    if(blocked(wx + e.dx, wy + e.dy) != 0)
                        continue; // skirt only blocked → walkable boundaries
                    int h0 = height_at(wx + e.cx0, wy + e.cy0, plane) * 8;
                    int h1 = height_at(wx + e.cx1, wy + e.cy1, plane) * 8;
                    // sunk below the terrain to close seams, and one storey
                    // (240 units) above the highest corner so a biped can
                    // neither step nor hop over
                    quad(
                        float(wx + e.cx0) * 128.f,
                        float(wy + e.cy0) * 128.f,
                        float(wx + e.cx1) * 128.f,
                        float(wy + e.cy1) * 128.f,
                        float(std::min(h0, h1)) - 16.f,
                        float(std::max(h0, h1)) + 240.f);
                }
            }
        return mesh;
    }

    // ── map links (stairs, ladders, cave entrances) ─────────────────

    std::vector<MapLink> find_links()
    {
        // action classification (case-insensitive)
        auto lower = [](std::string s) {
            for(char& c : s)
                c = char(std::tolower((unsigned char)c));
            return s;
        };

        enum Dir
        {
            none,
            up,
            down,
            both
        };

        auto classify = [&](const std::string& action) -> Dir {
            std::string a = lower(action);
            if(a == "climb-up" || a == "go-up" || a == "walk-up")
                return up;
            if(a == "climb-down" || a == "go-down" || a == "walk-down")
                return down;
            if(a == "climb")
                return both;
            if(a == "enter" || a == "crawl-through" || a == "crawl-into")
                return down; // cave entrances
            return none;
        };

        // collect every placement of a link-capable loc, spatially bucketed
        struct LinkLoc
        {
            Placement pl;
            int       dir; // bitmask
        };

        std::vector<LinkLoc>                      locs;
        std::unordered_map<u64, std::vector<u32>> grid; // bucket → indices
        auto                                      bucket = [](int x, int y) {
            return u64(u32(x >> 4)) << 32 | u32(y >> 4);
        };

        for(const auto& ref : m_regions)
            for(const auto& pl : placements(ref))
            {
                if(pl.loc_id < 0 || pl.loc_id >= (int)m_loc_defs.size())
                    continue;
                int dir = loc_link_mask(m_loc_defs[pl.loc_id]);
                if(dir == 0)
                    continue;
                grid[bucket(pl.world_x, pl.world_y)].push_back(
                    u32(locs.size()));
                locs.push_back({pl, dir});
            }

        // is there a loc with direction bit `want` near (x, y, plane)?
        auto near = [&](int x, int y, int plane, int want, int radius) {
            for(int by = (y - radius) >> 4; by <= (y + radius) >> 4; ++by)
                for(int bx = (x - radius) >> 4; bx <= (x + radius) >> 4; ++bx)
                {
                    auto it = grid.find(u64(u32(bx)) << 32 | u32(by));
                    if(it == grid.end())
                        continue;
                    for(u32 i : it->second)
                    {
                        const LinkLoc& o = locs[i];
                        if((o.dir & want) && o.pl.plane == plane &&
                           std::abs(o.pl.world_x - x) <= radius &&
                           std::abs(o.pl.world_y - y) <= radius)
                            return true;
                    }
                }
            return false;
        };

        constexpr int dungeon_offset = 6400; // region_y + 100
        constexpr int radius         = 10;

        std::vector<MapLink> links;
        auto                 emit = [&](const LinkLoc& l,
                        MapLink::Kind  kind,
                        int            tx,
                        int            ty,
                        int            tp,
                        int            want_reverse) {
            const LocDef& def = m_loc_defs[l.pl.loc_id];
            std::string   action;
            for(const auto& a : def.actions)
            {
                Dir  d     = classify(a);
                bool is_up = kind == MapLink::Kind::plane_up ||
                             kind == MapLink::Kind::dungeon_up;
                if(d == both || (is_up ? d == up : d == down))
                {
                    action = a;
                    break;
                }
            }
            MapLink link{
                kind,
                l.pl.loc_id,
                def.name,
                action,
                l.pl.world_x,
                l.pl.world_y,
                l.pl.plane,
                tx,
                ty,
                tp,
                near(tx, ty, tp, want_reverse, radius)};
            link.shape    = l.pl.type;
            link.rotation = l.pl.rotation;
            if(const std::vector<int>* ids = loc_model_ids(def, l.pl.type))
                link.model_ids = *ids;

            // footprint trigger box at the placement (same math as
            // build_locs_meshes: sizes swap on odd rotation, ground from
            // the footprint corners)
            int size_x = def.size_x, size_y = def.size_y;
            if(l.pl.rotation & 1)
                std::swap(size_x, size_y);
            int ground = height_at(l.pl.world_x, l.pl.world_y, l.pl.plane);
            for(int cy = 0; cy <= size_y; ++cy)
                for(int cx = 0; cx <= size_x; ++cx)
                    ground = std::min(
                        ground,
                        height_at(
                            l.pl.world_x + cx, l.pl.world_y + cy, l.pl.plane));
            link.aabb_min[0] = float(l.pl.world_x) * 128.f;
            link.aabb_min[1] = float(l.pl.world_y) * 128.f;
            link.aabb_min[2] = float(ground) * 8.f;
            link.aabb_max[0] = float(l.pl.world_x + size_x) * 128.f;
            link.aabb_max[1] = float(l.pl.world_y + size_y) * 128.f;
            link.aabb_max[2] = float(ground) * 8.f + LINK_AABB_HEIGHT;
            links.push_back(std::move(link));
        };

        for(const auto& l : locs)
        {
            int  x = l.pl.world_x, y = l.pl.world_y, p = l.pl.plane;
            bool underground = y >= dungeon_offset;

            if(l.dir & 1) // up
            {
                if(underground && p == 0)
                    emit(
                        l,
                        MapLink::Kind::dungeon_up,
                        x,
                        y - dungeon_offset,
                        0,
                        2);
                else if(p < NUM_PLANES - 1)
                    emit(l, MapLink::Kind::plane_up, x, y, p + 1, 2);
            }
            if(l.dir & 2) // down
            {
                if(p > 0)
                    emit(l, MapLink::Kind::plane_down, x, y, p - 1, 1);
                else if(
                    !underground &&
                    find_region(
                        x / REGION_SIZE, (y + dungeon_offset) / REGION_SIZE))
                    emit(
                        l,
                        MapLink::Kind::dungeon_down,
                        x,
                        y + dungeon_offset,
                        0,
                        1);
            }
        }
        return links;
    }

  private:
    using u64 = uint64_t;

    static u32 key(int rx, int ry)
    {
        return u32(rx) << 16 | u32(ry);
    }

    // Decompressed map square (terrain, or the loc placements when locs).
    // Classic layout: numeric file ids from map_index under 4/; beta layout:
    // maps/m{x}_{y} (terrain) and maps/l{x}_{y} (locs).
    std::vector<u8> map_square(const RegionRef& ref, bool locs)
    {
        std::string path =
            m_beta ? m_cache_dir + "/maps/" + (locs ? "l" : "m") +
                         std::to_string(ref.region_x) + "_" +
                         std::to_string(ref.region_y)
                   : m_cache_dir + "/4/" +
                         std::to_string(
                             locs ? ref.map_file_id : ref.land_file_id) +
                         ".dat";
        return mapsquare_decompress(read_file(path));
    }

    // Parsed terrain planes of a region; nullptr when the region has no
    // map data. Cached (including negative results).
    const Planes* region_planes(int rx, int ry)
    {
        u32  k  = key(rx, ry);
        auto it = m_planes.find(k);
        if(it != m_planes.end())
            return it->second ? it->second.get() : nullptr;

        std::unique_ptr<Planes> planes;
        if(const RegionRef* ref = find_region(rx, ry))
        {
            try
            {
                auto raw = map_square(*ref, false);
                planes   = std::make_unique<Planes>(parse_terrain(raw, rx, ry));
            } catch(const std::exception&)
            {
            } // missing/corrupt = no data
        }
        auto [ins, ok] = m_planes.emplace(k, std::move(planes));
        return ins->second ? ins->second.get() : nullptr;
    }

    std::string                                      m_cache_dir;
    bool                                             m_beta = false;
    ObModelArchive                                   m_ob_models;
    std::string                                      m_signature;
    std::vector<Underlay>                            m_underlays;
    std::vector<FloorClass>                          m_floor_classes;
    std::vector<LocDef>                              m_loc_defs;
    std::vector<RegionRef>                           m_regions;
    MapBounds                                        m_bounds;
    MapBounds                                        m_surface_bounds;
    std::unordered_map<u32, const RegionRef*>        m_region_lookup;
    std::unordered_map<u32, std::unique_ptr<Planes>> m_planes;
    std::unordered_map<int, RsModel>                 m_models;
    std::unordered_map<u32, Color>                   m_color_cache;
    TextureColors                                    m_textures;
};

} // namespace rs2
