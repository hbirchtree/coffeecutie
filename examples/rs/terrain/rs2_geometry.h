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

#include "rs2_cache.h"

#include <cctype>
#include <memory>

namespace rs2 {

struct Vertex
{
    float x, y, z;
    u8    r, g, b; // baked flat colour (texture average for textured faces)
    float u = 0.f, v = 0.f;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<u32>    indices;     // triangle list
    std::vector<i32>    tri_texture; // per triangle: texture id, -1 = flat
    // per triangle: flo.dat overlay id (1-indexed) that produced it, 0 =
    // underlay/model geometry. Underlay names ("water", "lava", …) let a
    // renderer pick shaders per material class.
    std::vector<i32>    tri_overlay;
};

struct RegionGeometry
{
    int  region_x = 0, region_y = 0, plane = 0;
    Mesh terrain;
    Mesh locs;
};

// A traversal link derived from loc actions. The cache stores no teleport
// table (that is server data); links are reconstructed from the
// "Climb-up"/"Climb-down"/"Enter" actions on loc definitions plus the RS2
// convention that dungeons lie 6400 tiles north of their surface spot
// (region_y + 100). `paired` = a loc with the opposite action was found
// near the destination, which makes the link near-certain.
struct MapLink
{
    enum class Kind { plane_up, plane_down, dungeon_down, dungeon_up };
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
    std::vector<int> model_ids;
};

inline const char* to_string(MapLink::Kind k)
{
    switch(k)
    {
    case MapLink::Kind::plane_up: return "plane_up";
    case MapLink::Kind::plane_down: return "plane_down";
    case MapLink::Kind::dungeon_down: return "dungeon_down";
    default: return "dungeon_up";
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
    {0, 0, 0, 0},      // 0 (unused)
    {0, 0, 0, 0},      // 1  SW
    {64, 0, 1, 0},     // 2  S edge mid
    {128, 0, 1, 1},    // 3  SE
    {128, 64, 2, 1},   // 4  E edge mid
    {128, 128, 2, 2},  // 5  NE
    {64, 128, 3, 2},   // 6  N edge mid
    {0, 128, 3, 3},    // 7  NW
    {0, 64, 3, 0},     // 8  W edge mid
    {64, 32, 1, 0},    // 9  inset S mid
    {96, 64, 2, 1},    // 10 inset E mid
    {64, 96, 3, 2},    // 11 inset N mid
    {32, 64, 3, 0},    // 12 inset W mid
    {32, 32, 0, 0},    // 13 inset SW
    {96, 32, 1, 1},    // 14 inset SE
    {96, 96, 2, 2},    // 15 inset NE
    {32, 96, 3, 3},    // 16 inset NW
};

class RegionLoader
{
  public:
    explicit RegionLoader(std::string cache_dir) :
        m_cache_dir(std::move(cache_dir))
    {
        auto arch2 = jag_load_archive(read_file(m_cache_dir + "/0/2.dat"));

        auto flo = jag_extract(arch2, "flo.dat");
        if(!flo)
            throw std::runtime_error("flo.dat not found in archive 2");
        m_underlays = parse_flo_dat(*flo);

        auto loc_dat = jag_extract(arch2, "loc.dat");
        auto loc_idx = jag_extract(arch2, "loc.idx");
        if(loc_dat && loc_idx)
            m_loc_defs = parse_loc_defs(*loc_dat, *loc_idx);

        auto arch5 = jag_load_archive(read_file(m_cache_dir + "/0/5.dat"));
        auto mi    = jag_extract(arch5, "map_index");
        if(!mi)
            throw std::runtime_error("map_index not found in archive 5");
        m_regions = parse_map_index(*mi);
        for(const auto& r : m_regions)
            m_region_lookup[key(r.region_x, r.region_y)] = &r;
    }

    const std::vector<RegionRef>& regions() const { return m_regions; }

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
        geo.locs     = build_locs_mesh(*ref, plane);
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

    const std::vector<Underlay>& underlays() const { return m_underlays; }
    const std::vector<LocDef>&   loc_defs() const { return m_loc_defs; }

    // Loc placements of one region (all planes).
    std::vector<Placement> placements(const RegionRef& ref)
    {
        try
        {
            auto raw = gzip_file(
                m_cache_dir + "/4/" + std::to_string(ref.map_file_id) + ".dat");
            return parse_map_locs(raw, ref.region_x, ref.region_y);
        }
        catch(const std::exception&)
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
            auto raw = read_file(
                m_cache_dir + "/1/" + std::to_string(id) + ".dat");
            if(raw.size() >= 2 && raw[0] == 0x1F && raw[1] == 0x8B)
                raw = gzip_decompress(raw.data(), raw.size());
            parsed = parse_model(raw);
        }
        catch(const std::exception&) {}
        auto [ins, ok] = m_models.emplace(id, std::move(parsed));
        return ins->second.valid ? &ins->second : nullptr;
    }

    struct OverlayInfo
    {
        Color color;        // flat colour (texture average when textured)
        int   texture = -1; // texture id for real texturing
        bool  hidden = false;
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
        }
        else if(o.r == 255 && o.g == 0 && o.b == 255)
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
        Mesh mesh;
        std::unordered_map<u64, u32> vert_index;
        auto add_vertex = [&](float x, float y, float z, Color c, float u,
                              float v) -> u32 {
            u64 k = (u64(u32(i32(x))) << 32) ^ (u64(u32(i32(y))) << 8) ^
                    u64(u32(i32(z))) ^ (u64(c.r) << 56) ^ (u64(c.g) << 48) ^
                    (u64(c.b) << 40);
            k ^= u64(u32(i32(u * 4096.f))) << 24 ^ u64(u32(i32(v * 4096.f)));
            auto [it, fresh] = vert_index.emplace(k, u32(mesh.vertices.size()));
            if(fresh)
                mesh.vertices.push_back({x, y, z, c.r, c.g, c.b, u, v});
            return it->second;
        };

        // Corner colour: underlay of the tile at the corner; boundary
        // corners take the neighbouring region's tile when available so
        // seams match, otherwise clamp into this region.
        auto corner_color = [&](int wx, int wy, int pl) -> Color {
            const Tile* t = tile_at(wx, wy, pl);
            if(!t)
                t = tile_at(
                    std::min(wx, (rx + 1) * REGION_SIZE - 1),
                    std::min(wy, (ry + 1) * REGION_SIZE - 1), pl);
            return t ? underlay_color(t->underlay_id, m_underlays)
                     : Color{128, 128, 128};
        };

        // Emit one tile's template geometry, heights/colours from plane pl.
        auto emit_tile = [&](const Tile& tile, int wx, int wy, int pl) {
            // corners: 0=SW 1=SE 2=NE 3=NW
            const int cdx[4] = {0, 1, 1, 0};
            const int cdy[4] = {0, 0, 1, 1};
            float hs[4];
            Color cs[4];
            for(int k = 0; k < 4; ++k)
            {
                hs[k] = float(height_at(wx + cdx[k], wy + cdy[k], pl)) * 8.f;
                cs[k] = corner_color(wx + cdx[k], wy + cdy[k], pl);
            }

            int         tpl = 0, rot = 0;
            OverlayInfo ov;
            if(tile.overlay_id != 0)
            {
                tpl = tile.overlay_shape + 1;
                rot = tile.overlay_rotation;
                ov  = overlay_info(tile.overlay_id);
            }

            // template vertices: rotate the code, then place
            struct TV { float x, y, z; Color under; };
            TV tvs[8];
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

                const TileShapeVertex& vp = k_tile_shape_vertex_pos[code];
                tvs[i] = {
                    float(wx) * 128.f + float(vp.px),
                    float(wy) * 128.f + float(vp.py),
                    (hs[vp.ca] + hs[vp.cb]) * 0.5f,
                    Color{u8((cs[vp.ca].r + cs[vp.cb].r) / 2),
                          u8((cs[vp.ca].g + cs[vp.cb].g) / 2),
                          u8((cs[vp.ca].b + cs[vp.cb].b) / 2)}};
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
                        tv.x, tv.y, tv.z, is_overlay ? ov.color : tv.under,
                        u, vv));
                }
                mesh.tri_texture.push_back(textured ? ov.texture : -1);
                mesh.tri_overlay.push_back(is_overlay ? tile.overlay_id : 0);
            }
        };

        for(int ty = 0; ty < REGION_SIZE; ++ty)
            for(int tx = 0; tx < REGION_SIZE; ++tx)
            {
                int wx = rx * REGION_SIZE + tx;
                int wy = ry * REGION_SIZE + ty;
                const Tile* tile = tile_at(wx, wy, plane);
                if(!tile)
                    continue;

                bool bridge = (plane == 0 || plane == 1) && is_bridge(wx, wy);
                if(plane == 1 && bridge)
                    continue; // deck already drawn as part of plane 0
                // upper planes: only tiles that actually have a floor
                if(plane > 0 && tile->underlay_id == 0 &&
                   tile->overlay_id == 0)
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
    Mesh build_locs_mesh(const RegionRef& ref, int plane)
    {
        Mesh mesh;
        // dedupe on (position, colour, uv); positions are integer-derived
        std::unordered_map<u64, u32> vert_index;
        auto add_vertex = [&](float x, float y, float z, Color c, float u,
                              float v) -> u32 {
            u64 k = (u64(u32(i32(x))) << 32) ^ (u64(u32(i32(y))) << 8) ^
                    u64(u32(i32(z))) ^ (u64(c.r) << 56) ^ (u64(c.g) << 48) ^
                    (u64(c.b) << 40);
            k ^= u64(u32(i32(u * 4096.f))) << 24 ^ u64(u32(i32(v * 4096.f)));
            auto [it, fresh] = vert_index.emplace(k, u32(mesh.vertices.size()));
            if(fresh)
                mesh.vertices.push_back({x, y, z, c.r, c.g, c.b, u, v});
            return it->second;
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
            // Skip floor decorations (type 22) — flat ground-cover patches.
            if(pl.type == 22)
                continue;
            if(pl.loc_id < 0 || pl.loc_id >= (int)m_loc_defs.size())
                continue;
            const LocDef& def = m_loc_defs[pl.loc_id];

            RsModel mdl;
            if(!build_loc_model(
                   def, pl.type, pl.rotation, [&](int id) { return model(id); },
                   mdl))
                continue;

            // World origin: centre of the tile footprint (client:
            // entityX = tileX*128 + sizeX*64), sizes swapped on odd rotations.
            int size_x = def.size_x, size_y = def.size_y;
            if(pl.rotation & 1)
                std::swap(size_x, size_y);

            float ox = float(pl.world_x) * 128.f + float(size_x) * 64.f;
            float oy = float(pl.world_y) * 128.f + float(size_y) * 64.f;

            // Ground height: average of the footprint's centre corners
            // (client centerHeight computation).
            int sx = pl.world_x + (size_x >> 1);
            int ex = pl.world_x + ((size_x + 1) >> 1);
            int sy = pl.world_y + (size_y >> 1);
            int ey = pl.world_y + ((size_y + 1) >> 1);
            float gz = float(height_at(sx, sy, pl.plane) +
                             height_at(ex, sy, pl.plane) +
                             height_at(sx, ey, pl.plane) +
                             height_at(ex, ey, pl.plane)) *
                       8.f / 4.f;

            // client model lighting on the final transformed model,
            // quantised to 8-step buckets to keep material counts sane
            std::vector<int> lights = model_vertex_lights(mdl);
            for(int& l : lights)
                l = std::min(126, std::max(2, l)) & ~7;

            // contoured ground (loc op 21, cave walls etc.): every vertex
            // follows the bilinearly interpolated heightmap at its own
            // world position, so adjacent models connect seamlessly
            auto ground_at = [&](float wxf, float wyf) -> float {
                int   gx = int(std::floor(wxf / 128.f));
                int   gy = int(std::floor(wyf / 128.f));
                float fx = wxf / 128.f - float(gx);
                float fy = wyf / 128.f - float(gy);
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
                        col = Color{u8(avg.r * light >> 7),
                                    u8(avg.g * light >> 7),
                                    u8(avg.b * light >> 7)};
                    else
                        col = hsl_to_rgb(
                            adjust_hsl_lightness(u16(raw), light));

                    float wx = ox + float(v[0]);
                    float wy = oy + float(v[2]);
                    float base =
                        def.contoured_ground ? ground_at(wx, wy) : gz;
                    // model: x=east, y=vertical (negative=up), z=north
                    mesh.indices.push_back(add_vertex(
                        wx, wy, base - float(v[1]), col, us[k], vs[k]));
                }
                mesh.tri_texture.push_back(tex);
                mesh.tri_overlay.push_back(0);
            }
        }
        return mesh;
    }

    // ── map links (stairs, ladders, cave entrances) ─────────────────

    std::vector<MapLink> find_links()
    {
        // action classification (case-insensitive)
        auto lower = [](std::string s) {
            for(char& c : s) c = char(std::tolower((unsigned char)c));
            return s;
        };
        enum Dir { none, up, down, both };
        auto classify = [&](const std::string& action) -> Dir {
            std::string a = lower(action);
            if(a == "climb-up" || a == "go-up" || a == "walk-up") return up;
            if(a == "climb-down" || a == "go-down" || a == "walk-down")
                return down;
            if(a == "climb") return both;
            if(a == "enter" || a == "crawl-through" || a == "crawl-into")
                return down; // cave entrances
            return none;
        };
        auto def_dir = [&](const LocDef& def) -> int { // bitmask: 1=up 2=down
            int m = 0;
            for(const auto& a : def.actions)
            {
                Dir d = classify(a);
                if(d == up) m |= 1;
                if(d == down) m |= 2;
                if(d == both) m |= 3;
            }
            return m;
        };

        // collect every placement of a link-capable loc, spatially bucketed
        struct LinkLoc
        {
            Placement pl;
            int       dir; // bitmask
        };
        std::vector<LinkLoc>                       locs;
        std::unordered_map<u64, std::vector<u32>>  grid; // bucket → indices
        auto bucket = [](int x, int y) {
            return u64(u32(x >> 4)) << 32 | u32(y >> 4);
        };

        for(const auto& ref : m_regions)
            for(const auto& pl : placements(ref))
            {
                if(pl.loc_id < 0 || pl.loc_id >= (int)m_loc_defs.size())
                    continue;
                int dir = def_dir(m_loc_defs[pl.loc_id]);
                if(dir == 0)
                    continue;
                grid[bucket(pl.world_x, pl.world_y)].push_back(
                    u32(locs.size()));
                locs.push_back({pl, dir});
            }

        // is there a loc with direction bit `want` near (x, y, plane)?
        auto near = [&](int x, int y, int plane, int want, int radius) {
            for(int by = (y - radius) >> 4; by <= (y + radius) >> 4; ++by)
                for(int bx = (x - radius) >> 4; bx <= (x + radius) >> 4;
                    ++bx)
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
        auto emit = [&](const LinkLoc& l, MapLink::Kind kind, int tx, int ty,
                        int tp, int want_reverse) {
            const LocDef& def = m_loc_defs[l.pl.loc_id];
            std::string   action;
            for(const auto& a : def.actions)
            {
                Dir d = classify(a);
                bool is_up = kind == MapLink::Kind::plane_up ||
                             kind == MapLink::Kind::dungeon_up;
                if(d == both || (is_up ? d == up : d == down))
                {
                    action = a;
                    break;
                }
            }
            MapLink link{
                kind, l.pl.loc_id, def.name, action, l.pl.world_x,
                l.pl.world_y, l.pl.plane, tx, ty, tp,
                near(tx, ty, tp, want_reverse, radius)};
            link.shape    = l.pl.type;
            link.rotation = l.pl.rotation;
            if(const std::vector<int>* ids = loc_model_ids(def, l.pl.type))
                link.model_ids = *ids;
            links.push_back(std::move(link));
        };

        for(const auto& l : locs)
        {
            int x = l.pl.world_x, y = l.pl.world_y, p = l.pl.plane;
            bool underground = y >= dungeon_offset;

            if(l.dir & 1) // up
            {
                if(underground && p == 0)
                    emit(l, MapLink::Kind::dungeon_up, x, y - dungeon_offset,
                         0, 2);
                else if(p < NUM_PLANES - 1)
                    emit(l, MapLink::Kind::plane_up, x, y, p + 1, 2);
            }
            if(l.dir & 2) // down
            {
                if(p > 0)
                    emit(l, MapLink::Kind::plane_down, x, y, p - 1, 1);
                else if(!underground &&
                        find_region(x / REGION_SIZE,
                                    (y + dungeon_offset) / REGION_SIZE))
                    emit(l, MapLink::Kind::dungeon_down, x,
                         y + dungeon_offset, 0, 1);
            }
        }
        return links;
    }

  private:
    using u64 = uint64_t;

    static u32 key(int rx, int ry) { return u32(rx) << 16 | u32(ry); }

    std::vector<u8> gzip_file(const std::string& path)
    {
        auto raw = read_file(path);
        return gzip_decompress(raw.data(), raw.size());
    }

    // Parsed terrain planes of a region; nullptr when the region has no
    // map data. Cached (including negative results).
    const Planes* region_planes(int rx, int ry)
    {
        u32 k   = key(rx, ry);
        auto it = m_planes.find(k);
        if(it != m_planes.end())
            return it->second ? it->second.get() : nullptr;

        std::unique_ptr<Planes> planes;
        if(const RegionRef* ref = find_region(rx, ry))
        {
            try
            {
                auto raw = gzip_file(
                    m_cache_dir + "/4/" +
                    std::to_string(ref->land_file_id) + ".dat");
                planes = std::make_unique<Planes>(parse_terrain(raw, rx, ry));
            }
            catch(const std::exception&) {} // missing/corrupt = no data
        }
        auto [ins, ok] = m_planes.emplace(k, std::move(planes));
        return ins->second ? ins->second.get() : nullptr;
    }

    std::string                                        m_cache_dir;
    std::vector<Underlay>                              m_underlays;
    std::vector<LocDef>                                m_loc_defs;
    std::vector<RegionRef>                             m_regions;
    std::unordered_map<u32, const RegionRef*>          m_region_lookup;
    std::unordered_map<u32, std::unique_ptr<Planes>>   m_planes;
    std::unordered_map<int, RsModel>                   m_models;
    std::unordered_map<u32, Color>                     m_color_cache;
    TextureColors                                      m_textures;
};

} // namespace rs2
