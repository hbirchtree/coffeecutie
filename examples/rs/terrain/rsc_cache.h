/*
 * RSC (RuneScape Classic) cache parsing — low level.
 *
 * RSC shares the JAG container (and name hash) with RS2, so all byte-level
 * helpers come from rs2_cache.h; this header adds the RSC-specific formats:
 *   - archive identification (dumps ship with scrambled outer filenames)
 *   - GameData: config archive string.dat + integer.dat tables
 *   - map sectors: m{plane}{xx}{yy}.hei (heights+colours), .dat (walls,
 *     roofs, tile decoration, direction), .loc (planted objects)
 *   - .ob3 models (N-gon faces, RGB555 colours)
 *
 * Reference (GPL): https://github.com/2003scape/rsc-client — world.js
 * _loadSection_from4I, game-data.js loadData, game-model.js fromBytes.
 */
#pragma once

#include "rs2_cache.h"

#include <array>
#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace rsc {

using rs2::read_u16_be;
using rs2::u16;
using rs2::u32;
using rs2::u8;
using i16 = int16_t;
using i32 = int32_t;

constexpr int SECTOR_SIZE  = 48;
constexpr int SECTOR_TILES = SECTOR_SIZE * SECTOR_SIZE;

// Face fill convention (client Scene.rgb): value < 0 encodes a flat RGB555
// colour as -1 - (r/8*1024 + g/8*32 + b/8); value >= 0 is a texture id;
// COLOUR_TRANSPARENT means "face not drawn".
constexpr i32 COLOUR_TRANSPARENT = 12345678;

inline rs2::Color fill_to_rgb(i32 fill)
{
    i32 v = -1 - fill;
    return {
        u8(((v >> 10) & 31) * 8), u8(((v >> 5) & 31) * 8), u8((v & 31) * 8)};
}

// The client's 256-entry terrain palette (4 gradient bands of 64), decoded
// straight to RGB instead of the Scene.rgb encoding.
inline rs2::Color terrain_palette(int i)
{
    i &= 0xff;
    int j = i & 63;
    switch(i >> 6)
    {
    case 0: return {u8(255 - j * 4), u8(255 - int(j * 1.75)), u8(255 - j * 4)};
    case 1: return {u8(j * 3), 144, 0};
    case 2: return {u8(192 - int(j * 1.5)), u8(144 - int(j * 1.5)), 0};
    default: return {u8(96 - int(j * 1.5)), u8(48 + int(j * 1.5)), 0};
    }
}

// ─────────────────────────────────────────────── archive identification ──

// RSC dumps ship with scrambled outer filenames; every file is still a
// plain JAG archive, so each is identified by marker entries inside it.
// The models archive can only be told apart from sounds after the config
// strings are known (its entries are "<model name>.ob3").
struct CacheFiles
{
    std::vector<u8> config; // string.dat + integer.dat
    // multiple archives per role (free + members)
    std::vector<std::vector<u8>> land; // m....hei
    std::vector<std::vector<u8>> maps; // m....dat (+ .loc)
    // everything else (models, textures, media, sounds, …) — the models and
    // textures archives can only be recognised once the config strings are
    // parsed, so the loader resolves them from here
    std::vector<std::vector<u8>> candidates;
};

// ─────────────────────────────────────────────────── GameData (config) ──

// Port of game-data.js loadData, keeping only what geometry needs. The
// integer.dat tables must be walked in exact order (items, npcs, textures,
// animations, objects, wall objects, roofs, tiles, …) even though most
// sections are discarded.
struct GameData
{
    struct ObjectDef
    {
        std::string name, command1;
        std::string model;    // model archive entry is "<model>.ob3"
        int width = 1, height = 1; // footprint in tiles
        int type = 0;              // 1/2 = blocks movement
        int elevation = 0;
    };
    struct WallDef
    {
        std::string name, command1;
        int  height = 0;         // game units
        i32  fill_front = 0, fill_back = 0;
        bool blocking  = false; // "adjacent" byte != 0
        int  invisible = 0;     // != 0 → not drawn (5 = interactive tag)
    };
    struct TileDef
    {
        i32  fill = 0;     // decoration colour/texture (fill convention)
        int  type = 0;     // 2 = water/liquid (blocked), 4 = hole/floor
        bool blocking = false;
    };

    std::vector<ObjectDef>   objects;
    std::vector<WallDef>     walls;
    std::vector<int>         roof_height;
    std::vector<TileDef>     tiles;
    std::vector<std::string> texture_names;

    bool load(const std::vector<u8>& str_data, const std::vector<u8>& int_data)
    {
        size_t so = 0, io = 0;
        auto gstr = [&]() -> std::string {
            std::string s;
            while(so < str_data.size() && str_data[so] != 0)
                s += char(str_data[so++]);
            ++so;
            return s;
        };
        auto gb = [&]() -> int { return int_data[io++]; };
        auto gs = [&]() -> int {
            int v = read_u16_be(&int_data[io]);
            io += 2;
            return v;
        };
        auto gi = [&]() -> i32 {
            i32 v = i32(rs2::read_u32_be(&int_data[io]));
            io += 4;
            // negatives are stored offset past 99999999 (client
            // getUnsignedInt quirk — how Scene.rgb fills survive the
            // unsigned serialisation)
            if(v > 99999999)
                v = 99999999 - v;
            return v;
        };
        auto ensure = [&](size_t n) {
            if(io + n > int_data.size())
                throw std::runtime_error("RSC integer.dat truncated");
        };

        // items
        int items = gs();
        for(int i = 0; i < items * 3; ++i) gstr(); // name, description, command
        ensure(size_t(items) * 16);
        for(int i = 0; i < items; ++i) gs();       // picture
        for(int i = 0; i < items; ++i) gi();       // base price
        for(int i = 0; i < items * 2; ++i) gb();   // stackable, unused
        for(int i = 0; i < items; ++i) gs();       // wearable
        for(int i = 0; i < items; ++i) gi();       // mask
        for(int i = 0; i < items * 2; ++i) gb();   // special, members

        // npcs
        int npcs = gs();
        for(int i = 0; i < npcs * 2; ++i) gstr();  // name, description
        ensure(size_t(npcs) * 40);
        for(int i = 0; i < npcs * 5; ++i) gb();    // attack..attackable
        for(int i = 0; i < npcs * 12; ++i) gb();   // sprites
        for(int i = 0; i < npcs * 4; ++i) gi();    // colours
        for(int i = 0; i < npcs * 2; ++i) gs();    // width, height
        for(int i = 0; i < npcs * 3; ++i) gb();    // walk/combat model, anim
        for(int i = 0; i < npcs; ++i) gstr();      // command

        // textures
        int texs = gs();
        for(int i = 0; i < texs; ++i) texture_names.push_back(gstr());
        for(int i = 0; i < texs; ++i) gstr();      // subtype names

        // animations
        int anims = gs();
        for(int i = 0; i < anims; ++i) gstr();
        ensure(size_t(anims) * 8);
        for(int i = 0; i < anims; ++i) gi();       // character colour
        for(int i = 0; i < anims * 4; ++i) gb();   // gender, hasA, hasF, number

        // objects (scenery)
        int objs = gs();
        objects.resize(objs);
        for(int i = 0; i < objs; ++i) objects[i].name = gstr();
        for(int i = 0; i < objs; ++i) gstr();      // description
        for(int i = 0; i < objs; ++i) objects[i].command1 = gstr();
        for(int i = 0; i < objs; ++i) gstr();      // command2
        for(int i = 0; i < objs; ++i) objects[i].model = gstr();
        ensure(size_t(objs) * 4);
        for(int i = 0; i < objs; ++i) objects[i].width = gb();
        for(int i = 0; i < objs; ++i) objects[i].height = gb();
        for(int i = 0; i < objs; ++i) objects[i].type = gb();
        for(int i = 0; i < objs; ++i) objects[i].elevation = gb();

        // wall objects
        int nwalls = gs();
        walls.resize(nwalls);
        for(int i = 0; i < nwalls; ++i) walls[i].name = gstr();
        for(int i = 0; i < nwalls; ++i) gstr();    // description
        for(int i = 0; i < nwalls; ++i) walls[i].command1 = gstr();
        for(int i = 0; i < nwalls; ++i) gstr();    // command2
        ensure(size_t(nwalls) * 12);
        for(int i = 0; i < nwalls; ++i) walls[i].height = gs();
        for(int i = 0; i < nwalls; ++i) walls[i].fill_front = gi();
        for(int i = 0; i < nwalls; ++i) walls[i].fill_back = gi();
        for(int i = 0; i < nwalls; ++i) walls[i].blocking = gb() != 0;
        for(int i = 0; i < nwalls; ++i) walls[i].invisible = gb();

        // roofs
        int roofs = gs();
        ensure(size_t(roofs) * 2);
        roof_height.resize(roofs);
        for(int i = 0; i < roofs; ++i) roof_height[i] = gb();
        for(int i = 0; i < roofs; ++i) gb();       // num vertices

        // tiles (floor decoration defs)
        int ntiles = gs();
        ensure(size_t(ntiles) * 6);
        tiles.resize(ntiles);
        for(int i = 0; i < ntiles; ++i) tiles[i].fill = gi();
        for(int i = 0; i < ntiles; ++i) tiles[i].type = gb();
        for(int i = 0; i < ntiles; ++i) tiles[i].blocking = gb() != 0;

        // spells + prayers follow; not needed. Sanity: counts must be sane
        // and the cursor must not have overrun.
        return io <= int_data.size() && !walls.empty() && !tiles.empty();
    }
};

// ───────────────────────────────────────────────────────── map sectors ──

// One 48×48 sector, all layers merged from .hei + .dat (+ .loc).
struct Sector
{
    // Per-tile, index = tx * 48 + ty (client layout)
    std::array<u8, SECTOR_TILES>  height{};     // game units (raw * 3 later)
    std::array<u8, SECTOR_TILES>  colour{};     // terrain palette index
    std::array<u8, SECTOR_TILES>  wall_ns{};    // wall id + 1 along (x,y)-(x,y+1)
    std::array<u8, SECTOR_TILES>  wall_ew{};    // wall id + 1 along (x,y)-(x+1,y)
    std::array<i32, SECTOR_TILES> wall_diag{};  // id+1; +12000 mirrored; +48000 object
    std::array<u8, SECTOR_TILES>  roof{};       // roof id + 1
    std::array<u8, SECTOR_TILES>  decoration{}; // tile def id + 1
    std::array<u8, SECTOR_TILES>  direction{};  // object/decor direction
};

// .hei: two byte-RLE streams (heights then colours: val < 128 literal, else
// repeat previous literal val-128 times), each followed by a column-major
// accumulate pass (loop tileY outer, tileX inner over index tileX*48+tileY;
// lastVal = (v + lastVal) & 0x7f, store lastVal * 2).
inline size_t rle_2304(const std::vector<u8>& d, size_t off, u8* out)
{
    int t = 0, last = 0;
    while(t < SECTOR_TILES)
    {
        if(off >= d.size())
            throw std::runtime_error("RSC sector RLE truncated");
        int v = d[off++];
        if(v < 128)
        {
            out[t++] = u8(v);
            last     = v;
        }
        else
            for(int i = 0; i < v - 128 && t < SECTOR_TILES; ++i)
                out[t++] = u8(last);
    }
    return off;
}

inline void accumulate_pass(u8* vals, int start)
{
    int last = start;
    for(int ty = 0; ty < SECTOR_SIZE; ++ty)
        for(int tx = 0; tx < SECTOR_SIZE; ++tx)
        {
            int i   = tx * SECTOR_SIZE + ty;
            last    = (vals[i] + last) & 0x7f;
            vals[i] = u8((last * 2) & 0xff);
        }
}

inline void parse_hei(const std::vector<u8>& d, Sector& s)
{
    size_t off = rle_2304(d, 0, s.height.data());
    accumulate_pass(s.height.data(), 64);
    rle_2304(d, off, s.colour.data());
    accumulate_pass(s.colour.data(), 35);
}

// .dat: raw wall layers then RLE roof / decoration / direction. The RLE
// repeat fills 0 for roof and direction but repeats the value for
// decoration (matching the client exactly).
inline void parse_sector_dat(const std::vector<u8>& d, Sector& s)
{
    size_t off = 0;
    if(d.size() < size_t(SECTOR_TILES) * 4)
        throw std::runtime_error("RSC sector .dat too short");
    for(int t = 0; t < SECTOR_TILES; ++t) s.wall_ns[t] = d[off++];
    for(int t = 0; t < SECTOR_TILES; ++t) s.wall_ew[t] = d[off++];
    for(int t = 0; t < SECTOR_TILES; ++t) s.wall_diag[t] = d[off++];
    for(int t = 0; t < SECTOR_TILES; ++t)
    {
        int v = d[off++];
        if(v > 0)
            s.wall_diag[t] = v + 12000;
    }
    auto rle_zero = [&](u8* out) {
        int t = 0;
        while(t < SECTOR_TILES)
        {
            if(off >= d.size())
                throw std::runtime_error("RSC sector RLE truncated");
            int v = d[off++];
            if(v < 128)
                out[t++] = u8(v);
            else
                for(int i = 0; i < v - 128 && t < SECTOR_TILES; ++i)
                    out[t++] = 0;
        }
    };
    rle_zero(s.roof.data());
    off = rle_2304(d, off, s.decoration.data());
    rle_zero(s.direction.data());
}

// .loc: RLE over the diagonal layer; literal val places object id (val - 1)
// as wall_diag = val + 48000, val >= 128 skips val - 128 tiles.
inline void parse_sector_loc(const std::vector<u8>& d, Sector& s)
{
    size_t off = 0;
    int    t   = 0;
    while(t < SECTOR_TILES && off < d.size())
    {
        int v = d[off++];
        if(v < 128)
            s.wall_diag[t++] = v + 48000;
        else
            t += v - 128;
    }
}

// ───────────────────────────────────────────────────────── .ob3 models ──

// Client game-model.js fromBytes. Faces are N-gons with front/back fills;
// 32767 in a fill slot means transparent.
struct Ob3Model
{
    std::vector<std::array<i16, 3>> verts;    // x, y (negative = up), z
    std::vector<std::vector<int>>   faces;    // N-gon vertex indices
    std::vector<i32>                fill_front, fill_back;
    bool valid = false;
};

inline Ob3Model parse_ob3(const std::vector<u8>& d)
{
    Ob3Model m;
    if(d.size() < 4)
        return m;
    size_t off = 0;
    auto   gs  = [&]() -> int {
        int v = read_u16_be(&d[off]);
        off += 2;
        return v;
    };
    auto gss = [&]() -> i16 {
        i16 v = i16(read_u16_be(&d[off]));
        off += 2;
        return v;
    };
    int nv = gs(), nf = gs();
    if(nv <= 0 || nf <= 0 ||
       d.size() < 4 + size_t(nv) * 6 + size_t(nf) * 6)
        return m;

    m.verts.resize(nv);
    for(int i = 0; i < nv; ++i) m.verts[i][0] = gss();
    for(int i = 0; i < nv; ++i) m.verts[i][1] = gss();
    for(int i = 0; i < nv; ++i) m.verts[i][2] = gss();

    std::vector<int> counts(nf);
    for(int i = 0; i < nf; ++i) counts[i] = d[off++];
    m.fill_front.resize(nf);
    m.fill_back.resize(nf);
    for(int i = 0; i < nf; ++i)
    {
        i32 v = gss();
        m.fill_front[i] = v == 32767 ? COLOUR_TRANSPARENT : v;
    }
    for(int i = 0; i < nf; ++i)
    {
        i32 v = gss();
        m.fill_back[i] = v == 32767 ? COLOUR_TRANSPARENT : v;
    }
    off += nf; // intensity flags (lighting) — unused here

    m.faces.resize(nf);
    for(int i = 0; i < nf; ++i)
    {
        m.faces[i].resize(counts[i]);
        for(int j = 0; j < counts[i]; ++j)
        {
            if(nv < 256)
            {
                if(off >= d.size()) return m;
                m.faces[i][j] = d[off++];
            }
            else
            {
                if(off + 2 > d.size()) return m;
                m.faces[i][j] = gs();
            }
        }
    }
    m.valid = true;
    return m;
}

// ─────────────────────────────────────────── cache file identification ──

inline CacheFiles identify_cache(const std::string& dir)
{
    CacheFiles out;
    u32 h_string  = rs2::jag_hash("string.dat");
    u32 h_index   = rs2::jag_hash("index.dat");
    u32 h_compass = rs2::jag_hash("compass.dat");

    struct Probed
    {
        std::vector<u8> body;
        bool has_string = false, has_index = false, has_compass = false;
        int  hei = 0, dat = 0;
        size_t entries = 0;
    };
    std::vector<Probed> pool;

    // sector-name hashes for classification
    std::unordered_map<u32, char> sector_hash;
    for(int p = 0; p < 4; ++p)
        for(int x = 40; x < 70; ++x)
            for(int y = 30; y < 200; ++y)
            {
                char base[16];
                snprintf(
                    base, sizeof(base), "m%d%d%d%d%d", p, x / 10, x % 10,
                    y / 10, y % 10);
                sector_hash[rs2::jag_hash(std::string(base) + ".hei")] = 'h';
                sector_hash[rs2::jag_hash(std::string(base) + ".dat")] = 'd';
            }

    for(const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if(!entry.is_regular_file())
            continue;
        std::vector<u8> raw;
        try
        {
            raw = rs2::read_file(entry.path().string());
        }
        catch(const std::exception&)
        {
            continue;
        }
        if(raw.size() < 6 || (raw[0] == 'P' && raw[1] == 'K'))
            continue; // client jar / non-JAG
        Probed p;
        try
        {
            p.body = rs2::jag_load_archive(raw);
        }
        catch(const std::exception&)
        {
            continue; // not a JAG archive (crc, json metadata, …)
        }
        if(p.body.size() < 2)
            continue;
        u16 num = read_u16_be(p.body.data());
        if(p.body.size() < 2 + size_t(num) * 10)
            continue;
        p.entries = num;
        for(u16 i = 0; i < num; ++i)
        {
            u32 h = rs2::read_u32_be(p.body.data() + 2 + size_t(i) * 10);
            if(h == h_string) p.has_string = true;
            if(h == h_index) p.has_index = true;
            if(h == h_compass) p.has_compass = true;
            auto it = sector_hash.find(h);
            if(it != sector_hash.end())
                (it->second == 'h' ? p.hei : p.dat)++;
        }
        pool.push_back(std::move(p));
    }

    for(auto& p : pool)
        if(p.has_string)
        {
            out.config = std::move(p.body);
            p.entries  = 0;
        }
    if(out.config.empty())
        throw std::runtime_error("RSC config archive not found");

    for(auto& p : pool)
    {
        if(p.entries == 0)
            continue;
        if(p.hei > 0)
            out.land.push_back(std::move(p.body));
        else if(p.dat > 0)
            out.maps.push_back(std::move(p.body));
        else
            out.candidates.push_back(std::move(p.body));
    }

    return out;
}

// Find the archive holding "<name><ext>" entries among the unclassified
// candidates — used with model names for the models archive and texture
// names (+ index.dat) for the textures archive.
inline const std::vector<u8>* find_archive_with(
    const std::vector<std::vector<u8>>& candidates,
    const std::vector<std::string>&     names,
    const char*                         ext)
{
    for(const auto& body : candidates)
        for(const auto& n : names)
            if(rs2::jag_extract(body, n + ext))
                return &body;
    return nullptr;
}

} // namespace rsc
