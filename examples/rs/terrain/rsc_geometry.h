/*
 * RSC region geometry — mid level.
 *
 * Mirrors the rs2::RegionLoader interface (and reuses its Vertex/Mesh/
 * TriClass/RegionGeometry types verbatim) so renderers and physics consume
 * RSC worlds exactly like RS2 ones:
 *   - terrain: per-tile colour gouraud quads (palette or decoration), split
 *     into the client's diagonal triangles
 *   - walls: config-driven quads on tile edges (TriClass wall/door/scenery
 *     by blocking flag + command), physical geometry for Bullet as-is
 *   - objects: .loc planted .ob3 models (fan-triangulated N-gons)
 *   - clip: invisible skirts along blocked-tile boundaries (water,
 *     full-block decoration), same contract as rs2::build_clip_mesh
 *
 * Coordinates: X = east, Y = north, Z = up, 128 units per tile (identical
 * to RS2 — RSC's anInt585 is also 128); heights = raw * 3 game units.
 * World tile x = sector_x * 48 + tx, so RSC "regions" are virtual 64-tile
 * windows over the 48-tile sector grid.
 */
#pragma once

#include "rsc_cache.h"
#include "rs2_geometry.h"

#include <hash-library/sha256.h>

#include <set>

namespace rsc {

using rs2::Mesh;
using rs2::RegionGeometry;
using rs2::RegionRef;
using rs2::TriClass;
using rs2::Vertex;

constexpr int REGION_SIZE = rs2::REGION_SIZE; // 64-tile virtual regions

class RegionLoader
{
  public:
    explicit RegionLoader(std::string cache_dir)
    {
        auto files = identify_cache(cache_dir);

        SHA256 sig;
        sig.add(files.config.data(), files.config.size());

        auto str_data = rs2::jag_extract(files.config, "string.dat");
        auto int_data = rs2::jag_extract(files.config, "integer.dat");
        if(!str_data || !int_data)
            throw std::runtime_error("RSC config missing string/integer.dat");
        if(!m_data.load(*str_data, *int_data))
            throw std::runtime_error("RSC GameData parse failed");

        // models archive: the candidate holding "<object model>.ob3"
        {
            std::vector<std::string> probe;
            for(size_t i = 0; i < m_data.objects.size() && probe.size() < 8;
                ++i)
                if(!m_data.objects[i].model.empty())
                    probe.push_back(m_data.objects[i].model);
            if(const auto* body =
                   find_archive_with(files.candidates, probe, ".ob3"))
                m_models_archive = *body;
        }
        if(!m_models_archive.empty())
            sig.add(m_models_archive.data(), m_models_archive.size());
        m_signature = sig.getHash();

        // textures archive: candidate with "<texture name>.dat" — RSC
        // sprites share the RS2 indexed-sprite layout, so the RS2 decoder
        // applies
        {
            std::vector<std::string> probe(
                m_data.texture_names.begin(),
                m_data.texture_names.begin() +
                    std::min<size_t>(8, m_data.texture_names.size()));
            if(const auto* body =
                   find_archive_with(files.candidates, probe, ".dat"))
            {
                m_textures_archive = *body;
                m_texture_index = rs2::jag_extract(*body, "index.dat");
            }
        }

        // decode every sector up front (a full dump is only a few hundred)
        std::unordered_map<u32, char> hei_names, dat_names, loc_names;
        for(int p = 0; p < 4; ++p)
            for(int x = 40; x < 70; ++x)
                for(int y = 30; y < 200; ++y)
                {
                    char base[16];
                    snprintf(
                        base, sizeof(base), "m%d%d%d%d%d", p, x / 10, x % 10,
                        y / 10, y % 10);
                    u32 k = key3(p, x, y);
                    m_hei_hash[rs2::jag_hash(std::string(base) + ".hei")] = k;
                    m_dat_hash[rs2::jag_hash(std::string(base) + ".dat")] = k;
                    m_loc_hash[rs2::jag_hash(std::string(base) + ".loc")] = k;
                }

        auto for_entries = [&](const std::vector<u8>& body, auto&& fn) {
            if(body.size() < 2)
                return;
            u16    num = read_u16_be(body.data());
            size_t idx = 2, dat = 2 + size_t(num) * 10;
            for(u16 i = 0; i < num && idx + 10 <= body.size(); ++i, idx += 10)
            {
                u32 h  = rs2::read_u32_be(body.data() + idx);
                u32 ds = (u32(body[idx + 4]) << 16) |
                         (u32(body[idx + 5]) << 8) | body[idx + 6];
                u32 cs = (u32(body[idx + 7]) << 16) |
                         (u32(body[idx + 8]) << 8) | body[idx + 9];
                if(dat + cs <= body.size())
                {
                    std::vector<u8> d(
                        body.begin() + dat, body.begin() + dat + cs);
                    if(ds != cs)
                        d = rs2::bzip2_decompress(d.data(), d.size());
                    fn(h, d);
                }
                dat += cs;
            }
        };

        for(const auto& body : files.land)
            for_entries(body, [&](u32 h, const std::vector<u8>& d) {
                auto it = m_hei_hash.find(h);
                if(it == m_hei_hash.end())
                    return;
                try
                {
                    parse_hei(d, m_sectors[it->second]);
                }
                catch(const std::exception&) {}
            });
        for(const auto& body : files.maps)
            for_entries(body, [&](u32 h, const std::vector<u8>& d) {
                try
                {
                    if(auto it = m_dat_hash.find(h); it != m_dat_hash.end())
                        parse_sector_dat(d, m_sectors[it->second]);
                    else if(auto it2 = m_loc_hash.find(h);
                            it2 != m_loc_hash.end())
                        parse_sector_loc(d, m_sectors[it2->second]);
                }
                catch(const std::exception&) {}
            });

        // virtual 64-tile regions covering every populated plane-0 sector
        std::set<u32> region_keys;
        for(const auto& [k, s] : m_sectors)
        {
            int p = int(k >> 16), sx = int((k >> 8) & 0xff), sy = int(k & 0xff);
            if(p != 0)
                continue;
            int wx0 = sx * SECTOR_SIZE, wy0 = sy * SECTOR_SIZE;
            for(int rx = wx0 / REGION_SIZE;
                rx <= (wx0 + SECTOR_SIZE - 1) / REGION_SIZE; ++rx)
                for(int ry = wy0 / REGION_SIZE;
                    ry <= (wy0 + SECTOR_SIZE - 1) / REGION_SIZE; ++ry)
                    region_keys.insert(u32(rx) << 16 | u32(ry));
        }
        for(u32 k : region_keys)
        {
            RegionRef r{};
            r.region_x = u8(k >> 16);
            r.region_y = u8(k & 0xff);
            m_regions.push_back(r);
        }
        for(const auto& r : m_regions)
        {
            m_region_lookup[u32(r.region_x) << 16 | r.region_y] = &r;
            if(m_bounds.max_region_x < m_bounds.min_region_x)
                m_bounds = {r.region_x, r.region_y, r.region_x, r.region_y};
            m_bounds.min_region_x =
                std::min<int>(m_bounds.min_region_x, r.region_x);
            m_bounds.min_region_y =
                std::min<int>(m_bounds.min_region_y, r.region_y);
            m_bounds.max_region_x =
                std::max<int>(m_bounds.max_region_x, r.region_x);
            m_bounds.max_region_y =
                std::max<int>(m_bounds.max_region_y, r.region_y);
        }
    }

    const std::vector<RegionRef>& regions() const { return m_regions; }
    const rs2::MapBounds& bounds() const { return m_bounds; }
    const rs2::MapBounds& surface_bounds() const { return m_bounds; }
    const std::string& cache_signature() const { return m_signature; }

    const RegionRef* find_region(int rx, int ry) const
    {
        auto it = m_region_lookup.find(u32(rx) << 16 | u32(ry));
        return it == m_region_lookup.end() ? nullptr : it->second;
    }

    std::optional<RegionGeometry> load(int rx, int ry, int plane)
    {
        if(!find_region(rx, ry))
            return std::nullopt;
        RegionGeometry geo;
        geo.region_x = rx;
        geo.region_y = ry;
        geo.plane    = plane;
        geo.terrain  = build_terrain_mesh(rx, ry, plane);
        geo.locs     = build_walls_and_objects(rx, ry, plane);
        geo.clip     = build_clip_mesh(rx, ry, plane);
        return geo;
    }

    // Decoration id semantics (tri_overlay carries decoration ids)
    rs2::FloorClass floor_class(i32 overlay_id) const
    {
        if(overlay_id <= 0 || overlay_id > i32(m_data.tiles.size()))
            return rs2::FloorClass::none;
        // tile type 2 = liquid; decoration 12 is the lava hole variant
        if(overlay_id == 12)
            return rs2::FloorClass::lava;
        if(m_data.tiles[overlay_id - 1].type == 2)
            return rs2::FloorClass::water;
        return rs2::FloorClass::other;
    }

    rs2::Color face_color(u32 col)
    {
        if(col & rs2::FACE_TEXTURED)
            return texture_colour(int(col & 0xFFFF));
        return rs2::hsl_to_rgb(u16(col));
    }

    std::optional<rs2::SpriteRgba> texture_sprite(int id)
    {
        if(m_textures_archive.empty() || !m_texture_index ||
           id < 0 || id >= int(m_data.texture_names.size()))
            return std::nullopt;
        auto sprite = rs2::jag_extract(
            m_textures_archive, m_data.texture_names[id] + ".dat");
        if(!sprite)
            return std::nullopt;
        // RSC texture sprites use the same indexed layout as RS2 media
        // sprites; entry name replaces the numeric id
        return load_named_sprite(*sprite);
    }

    std::vector<rs2::MapLink> find_links() { return {}; }

  private:
    static u32 key3(int p, int sx, int sy)
    {
        return u32(p) << 16 | u32(sx) << 8 | u32(sy);
    }

    const Sector* sector_at(int plane, int wtx, int wty, int& local) const
    {
        if(wtx < 0 || wty < 0)
            return nullptr;
        int sx = wtx / SECTOR_SIZE, sy = wty / SECTOR_SIZE;
        auto it = m_sectors.find(key3(plane, sx, sy));
        if(it == m_sectors.end())
            return nullptr;
        local = (wtx - sx * SECTOR_SIZE) * SECTOR_SIZE +
                (wty - sy * SECTOR_SIZE);
        return &it->second;
    }

    // Tile-corner height in game units; upper storeys float one wall height
    // (192 units — the standard wallObjectHeight) per plane above ground.
    float height_at(int wtx, int wty, int plane) const
    {
        int          local;
        int          hplane = plane == 3 ? 3 : 0; // storeys share the heightmap
        const Sector* s     = sector_at(hplane, wtx, wty, local);
        float        base   = s ? float(s->height[local]) * 3.f : 0.f;
        if(plane == 1 || plane == 2)
            base += 192.f * float(plane);
        return base;
    }

    struct TileInfo
    {
        bool       present = false;
        rs2::Color colour{0, 0, 0};
        bool       draw = true;
        int        decoration = 0; // tile def id (1-indexed), 0 = none
        bool       blocked = false;
    };

    TileInfo tile_info(int wtx, int wty, int plane)
    {
        TileInfo info;
        int      local;
        const Sector* s = sector_at(plane, wtx, wty, local);
        if(!s)
            return info;
        info.present = true;
        info.colour  = terrain_palette(s->colour[local]);
        if(plane == 1 || plane == 2)
            info.draw = false; // storeys only have decorated floors

        int deco = s->decoration[local];
        if(deco > 0 && deco <= int(m_data.tiles.size()))
        {
            const GameData::TileDef& def = m_data.tiles[deco - 1];
            info.decoration = deco;
            info.draw       = true;
            info.colour     = fill_colour(def.fill);
            info.blocked    = def.blocking || def.type == 2;
        }
        return info;
    }

    rs2::Color fill_colour(i32 fill)
    {
        if(fill == COLOUR_TRANSPARENT)
            return {0, 0, 0};
        if(fill < 0)
            return fill_to_rgb(fill);
        return texture_colour(int(fill));
    }

    rs2::Color texture_colour(int id)
    {
        auto it = m_texture_colours.find(id);
        if(it != m_texture_colours.end())
            return it->second;
        rs2::Color c{160, 160, 160};
        if(auto sprite = texture_sprite(id))
        {
            u32 r = 0, g = 0, b = 0, n = 0;
            for(size_t i = 0; i + 3 < sprite->rgba.size(); i += 4)
                if(sprite->rgba[i + 3])
                {
                    r += sprite->rgba[i];
                    g += sprite->rgba[i + 1];
                    b += sprite->rgba[i + 2];
                    ++n;
                }
            if(n)
                c = {u8(r / n), u8(g / n), u8(b / n)};
        }
        m_texture_colours.emplace(id, c);
        return c;
    }

    std::optional<rs2::SpriteRgba> load_named_sprite(
        const std::vector<u8>& sprite) const
    {
        // identical layout to rs2 sprites: u16 offset into the shared
        // index.dat, palette + dimensions there, pixels here
        if(!m_texture_index || sprite.size() < 2)
            return std::nullopt;
        const auto& idx  = *m_texture_index;
        size_t      ioff = read_u16_be(sprite.data());
        if(ioff + 5 > idx.size())
            return std::nullopt;
        int pal_size = idx[ioff + 4];
        std::vector<std::array<u8, 4>> palette(
            std::max(pal_size, 1), {0, 0, 0, 0});
        size_t p = ioff + 5;
        for(int i = 1; i < pal_size; ++i, p += 3)
        {
            if(p + 3 > idx.size())
                return std::nullopt;
            palette[i] = {idx[p], idx[p + 1], idx[p + 2], 255};
        }
        if(p + 7 > idx.size())
            return std::nullopt;
        int sw   = read_u16_be(idx.data() + p + 2);
        int sh   = read_u16_be(idx.data() + p + 4);
        int type = idx[p + 6];
        size_t count = size_t(sw) * sh;
        if(sprite.size() < 2 + count || sw <= 0 || sh <= 0)
            return std::nullopt;
        rs2::SpriteRgba out;
        out.width  = sw;
        out.height = sh;
        out.rgba.resize(count * 4);
        const u8* px = sprite.data() + 2;
        for(size_t i = 0; i < count; ++i)
        {
            size_t dst = type == 1 ? (i % sh) * sw + i / sh : i;
            int    pi  = px[i];
            const auto& c = palette[pi < pal_size ? pi : 0];
            std::copy(c.begin(), c.end(), out.rgba.begin() + dst * 4);
        }
        return out;
    }

    // ── meshes ───────────────────────────────────────────────────────

    Mesh build_terrain_mesh(int rx, int ry, int plane)
    {
        Mesh mesh;
        rs2::VertexMap<u32> dedupe;
        auto add = [&](float x, float y, float z, rs2::Color c) -> u16 {
            Vertex v{x, y, z, c.r, c.g, c.b, 255, 0.f, 0.f};
            auto [it, fresh] =
                dedupe.emplace(rs2::vertex_key(v), u32(mesh.vertices.size()));
            if(fresh)
                mesh.vertices.push_back(v);
            return u16(it->second);
        };

        for(int x = 0; x < REGION_SIZE; ++x)
            for(int y = 0; y < REGION_SIZE; ++y)
            {
                int wtx = rx * REGION_SIZE + x, wty = ry * REGION_SIZE + y;
                TileInfo t = tile_info(wtx, wty, plane);
                if(!t.present || !t.draw)
                    continue;

                float h00 = height_at(wtx, wty, plane);
                float h10 = height_at(wtx + 1, wty, plane);
                float h01 = height_at(wtx, wty + 1, plane);
                float h11 = height_at(wtx + 1, wty + 1, plane);

                // simple slope light, same spirit as the RS2 corner light
                auto lit = [&](rs2::Color c, float dh) {
                    float f = std::clamp(1.f - dh / 512.f, 0.7f, 1.25f);
                    return rs2::Color{
                        u8(std::min(255.f, c.r * f)),
                        u8(std::min(255.f, c.g * f)),
                        u8(std::min(255.f, c.b * f))};
                };
                rs2::Color c = lit(t.colour, (h10 - h00) + (h11 - h01));

                float x0 = float(wtx) * 128.f, y0 = float(wty) * 128.f;
                u16 a = add(x0, y0, h00, c);
                u16 b = add(x0 + 128.f, y0, h10, c);
                u16 d = add(x0 + 128.f, y0 + 128.f, h11, c);
                u16 e = add(x0, y0 + 128.f, h01, c);
                // client splits (x,y+1)(x,y)(x+1,y) + (x+1,y)(x+1,y+1)(x,y+1)
                const u16 idx[] = {e, a, b, b, d, e};
                mesh.indices.insert(
                    mesh.indices.end(), std::begin(idx), std::end(idx));
                for(int i = 0; i < 2; ++i)
                {
                    mesh.tri_texture.push_back(-1);
                    mesh.tri_overlay.push_back(t.decoration);
                    mesh.tri_loc.push_back(-1);
                    mesh.tri_class.push_back(TriClass::terrain);
                }
            }
        return mesh;
    }

    const Ob3Model* model(const std::string& name)
    {
        auto it = m_model_cache.find(name);
        if(it != m_model_cache.end())
            return it->second.valid ? &it->second : nullptr;
        Ob3Model parsed;
        if(!m_models_archive.empty())
            if(auto raw =
                   rs2::jag_extract(m_models_archive, name + ".ob3"))
                parsed = parse_ob3(*raw);
        auto [ins, ok] = m_model_cache.emplace(name, std::move(parsed));
        return ins->second.valid ? &ins->second : nullptr;
    }

    std::vector<Mesh> build_walls_and_objects(int rx, int ry, int plane)
    {
        std::vector<Mesh> chunks(1);
        Mesh*             mesh = &chunks.back();
        rs2::VertexMap<u32> dedupe;
        auto ensure_room = [&](size_t incoming) {
            if(!mesh->vertices.empty() &&
               mesh->vertices.size() + incoming > rs2::MESH_MAX_VERTICES)
            {
                chunks.emplace_back();
                mesh = &chunks.back();
                dedupe.clear();
            }
        };
        auto add = [&](float x, float y, float z, rs2::Color c) -> u16 {
            Vertex v{x, y, z, c.r, c.g, c.b, 255, 0.f, 0.f};
            auto [it, fresh] =
                dedupe.emplace(rs2::vertex_key(v), u32(mesh->vertices.size()));
            if(fresh)
                mesh->vertices.push_back(v);
            return u16(it->second);
        };

        auto wall_quad = [&](int id, int x0, int y0, int x1, int y1,
                             int plane_) {
            if(id <= 0 || id > int(m_data.walls.size()))
                return;
            const GameData::WallDef& def = m_data.walls[id - 1];
            if(def.invisible != 0)
                return;
            rs2::Color c = fill_colour(
                def.fill_front == COLOUR_TRANSPARENT ? def.fill_back
                                                     : def.fill_front);
            TriClass cls = TriClass::scenery; // non-blocking = decorative
            std::string cmd = def.command1;
            for(char& ch : cmd) ch = char(std::tolower((unsigned char)ch));
            if(cmd == "open" || cmd == "close")
                cls = TriClass::door;
            else if(def.blocking)
                cls = TriClass::wall;

            float za = height_at(x0, y0, plane_);
            float zb = height_at(x1, y1, plane_);
            float h  = float(def.height);
            ensure_room(8);
            u16 a = add(float(x0) * 128.f, float(y0) * 128.f, za, c);
            u16 b = add(float(x1) * 128.f, float(y1) * 128.f, zb, c);
            u16 ct = add(float(x1) * 128.f, float(y1) * 128.f, zb + h, c);
            u16 d = add(float(x0) * 128.f, float(y0) * 128.f, za + h, c);
            const u16 idx[] = {a, b, ct, a, ct, d, a, ct, b, a, d, ct};
            mesh->indices.insert(
                mesh->indices.end(), std::begin(idx), std::end(idx));
            for(int i = 0; i < 4; ++i)
            {
                mesh->tri_texture.push_back(-1);
                mesh->tri_overlay.push_back(0);
                mesh->tri_loc.push_back(id - 1);
                mesh->tri_class.push_back(cls);
            }
        };

        for(int x = 0; x < REGION_SIZE; ++x)
            for(int y = 0; y < REGION_SIZE; ++y)
            {
                int wtx = rx * REGION_SIZE + x, wty = ry * REGION_SIZE + y;
                int          local;
                const Sector* s = sector_at(plane, wtx, wty, local);
                if(!s)
                    continue;
                // edge conventions from the client scene builder
                wall_quad(s->wall_ew[local], wtx, wty, wtx + 1, wty, plane);
                wall_quad(s->wall_ns[local], wtx, wty, wtx, wty + 1, plane);
                i32 diag = s->wall_diag[local];
                if(diag > 0 && diag < 12000)
                    wall_quad(diag, wtx, wty, wtx + 1, wty + 1, plane);
                else if(diag > 12000 && diag < 24000)
                    wall_quad(
                        diag - 12000, wtx + 1, wty, wtx, wty + 1, plane);
                else if(diag > 48000)
                    place_object(
                        diag - 48001, wtx, wty, s->direction[local], plane,
                        chunks, mesh, dedupe);
            }

        if(chunks.back().indices.empty())
            chunks.pop_back();
        return chunks;
    }

    void place_object(
        int obj_id, int wtx, int wty, int dir, int plane,
        std::vector<Mesh>& chunks, Mesh*& mesh,
        rs2::VertexMap<u32>& dedupe)
    {
        if(obj_id < 0 || obj_id >= int(m_data.objects.size()))
            return;
        const GameData::ObjectDef& def = m_data.objects[obj_id];
        const Ob3Model* mdl = model(def.model);
        if(!mdl)
            return;

        int w = def.width, hgt = def.height;
        if(dir == 0 || dir == 4)
            std::swap(w, hgt); // client swaps on N/S facing
        float ox = float(wtx) * 128.f + float(w) * 64.f;
        float oy = float(wty) * 128.f + float(hgt) * 64.f;
        float gz = 0.f;
        for(int cx = 0; cx <= 1; ++cx)
            for(int cy = 0; cy <= 1; ++cy)
                gz += height_at(wtx + cx * w, wty + cy * hgt, plane);
        gz /= 4.f;

        // client orients planted objects in 45° steps of tileDirection
        float ang = float(dir) * 3.14159265f / 4.f;
        float ca = std::cos(ang), sa = std::sin(ang);

        size_t worst = 0;
        for(const auto& f : mdl->faces)
            worst += (f.size() >= 3 ? (f.size() - 2) * 3 : 0);
        if(!mesh->vertices.empty() &&
           mesh->vertices.size() + worst > rs2::MESH_MAX_VERTICES)
        {
            chunks.emplace_back();
            mesh = &chunks.back();
            dedupe.clear();
        }
        auto add = [&](float x, float y, float z, rs2::Color c) -> u16 {
            Vertex v{x, y, z, c.r, c.g, c.b, 255, 0.f, 0.f};
            auto [it, fresh] =
                dedupe.emplace(rs2::vertex_key(v), u32(mesh->vertices.size()));
            if(fresh)
                mesh->vertices.push_back(v);
            return u16(it->second);
        };

        for(size_t fi = 0; fi < mdl->faces.size(); ++fi)
        {
            const auto& face = mdl->faces[fi];
            if(face.size() < 3)
                continue;
            i32 fill = mdl->fill_front[fi] != COLOUR_TRANSPARENT
                           ? mdl->fill_front[fi]
                           : mdl->fill_back[fi];
            if(fill == COLOUR_TRANSPARENT)
                continue;
            rs2::Color c = fill_colour(fill);

            auto emit = [&](int i0, int i1, int i2) {
                auto tv = [&](int i) {
                    const auto& v = mdl->verts[face[i]];
                    float mx = float(v[0]), mz = float(v[2]);
                    float rxv = mx * ca + mz * sa;
                    float rzv = mz * ca - mx * sa;
                    return add(
                        ox + rxv, oy + rzv,
                        gz - float(v[1]) + float(def.elevation), c);
                };
                u16 a = tv(i0), b = tv(i1), cc = tv(i2);
                mesh->indices.insert(mesh->indices.end(), {a, b, cc});
                mesh->indices.insert(mesh->indices.end(), {a, cc, b});
                for(int k = 0; k < 2; ++k)
                {
                    mesh->tri_texture.push_back(-1);
                    mesh->tri_overlay.push_back(0);
                    mesh->tri_loc.push_back(obj_id);
                    mesh->tri_class.push_back(TriClass::scenery);
                }
            };
            for(size_t i = 1; i + 1 < face.size(); ++i)
                emit(0, int(i), int(i + 1)); // fan triangulation
        }
    }

    // Invisible skirts along blocked↔walkable tile boundaries (water and
    // full-block decoration) — the same physics contract as RS2's
    // build_clip_mesh; blocking walls need none because their quads are
    // already physical TriClass::wall geometry.
    Mesh build_clip_mesh(int rx, int ry, int plane)
    {
        Mesh mesh;
        auto blocked = [&](int wtx, int wty) -> int {
            int          local;
            const Sector* s = sector_at(plane, wtx, wty, local);
            if(!s)
                return -1;
            int deco = s->decoration[local];
            if(deco > 0 && deco <= int(m_data.tiles.size()))
            {
                const auto& def = m_data.tiles[deco - 1];
                if(def.blocking || def.type == 2)
                    return 1;
            }
            return 0;
        };
        auto quad = [&](float x0, float y0, float x1, float y1, float zb,
                        float zt) {
            auto add = [&](float x, float y, float z) -> u16 {
                mesh.vertices.push_back(
                    Vertex{x, y, z, 0, 0, 0, 255, 0.f, 0.f});
                return u16(mesh.vertices.size() - 1);
            };
            u16 a = add(x0, y0, zb), b = add(x1, y1, zb);
            u16 c = add(x1, y1, zt), d = add(x0, y0, zt);
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
        struct Edge
        {
            int dx, dy, cx0, cy0, cx1, cy1;
        };
        static constexpr Edge edges[] = {
            {-1, 0, 0, 0, 0, 1},
            {1, 0, 1, 0, 1, 1},
            {0, -1, 0, 0, 1, 0},
            {0, 1, 0, 1, 1, 1},
        };
        for(int x = 0; x < REGION_SIZE; ++x)
            for(int y = 0; y < REGION_SIZE; ++y)
            {
                int wtx = rx * REGION_SIZE + x, wty = ry * REGION_SIZE + y;
                if(blocked(wtx, wty) != 1)
                    continue;
                for(const Edge& e : edges)
                {
                    if(blocked(wtx + e.dx, wty + e.dy) != 0)
                        continue;
                    float h0 = height_at(wtx + e.cx0, wty + e.cy0, plane);
                    float h1 = height_at(wtx + e.cx1, wty + e.cy1, plane);
                    quad(
                        float(wtx + e.cx0) * 128.f,
                        float(wty + e.cy0) * 128.f,
                        float(wtx + e.cx1) * 128.f,
                        float(wty + e.cy1) * 128.f,
                        std::min(h0, h1) - 16.f, std::max(h0, h1) + 240.f);
                }
            }
        return mesh;
    }

    GameData        m_data;
    std::vector<u8> m_models_archive;
    std::vector<u8> m_textures_archive;
    std::optional<std::vector<u8>> m_texture_index;
    std::string     m_signature;

    std::unordered_map<u32, u32>    m_hei_hash, m_dat_hash, m_loc_hash;
    std::unordered_map<u32, Sector> m_sectors;
    std::vector<RegionRef>          m_regions;
    rs2::MapBounds                  m_bounds;
    std::unordered_map<u32, const RegionRef*>   m_region_lookup;
    std::unordered_map<std::string, Ob3Model>   m_model_cache;
    std::unordered_map<int, rs2::Color>         m_texture_colours;
};

} // namespace rsc
