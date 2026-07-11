/*
 * RS2 cache parsing — low level.
 *
 * Everything that turns raw cache bytes into parsed structures:
 *   - JAG archives (index 0) + BZip2/gzip decompression
 *   - flo.dat underlays, loc.dat/loc.idx definitions (config archive 2)
 *   - map_index (archive 5), terrain tile streams + loc placements (index 4)
 *   - classic "old format" models (index 1)
 *   - texture palette averages (archive 6)
 *   - HSL→RGB via the exact client palette math
 *
 * Reference (BSD): https://github.com/RuneChamps/OSRS-World-Map
 */
#pragma once

#include <bzlib.h> // vcpkg bzip2 for JAG archives
#include <zlib.h>  // bundled zlib for raw deflate

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rs2 {

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using i32 = int32_t;

// ─────────────────────────────────────────────────────────── file helpers ──

inline std::vector<u8> read_file(const std::string& path)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if(!f)
        throw std::runtime_error("cannot open: " + path);
    // A directory opens without failing on Linux, but tellg() then returns
    // a garbage size — guard so a bad path throws instead of attempting a
    // multi-exabyte allocation.
    auto size = f.tellg();
    if(size < 0 || size > std::streamoff(1) << 31)
        throw std::runtime_error("bad file size: " + path);
    f.seekg(0);
    std::vector<u8> buf(size);
    f.read(reinterpret_cast<char*>(buf.data()), size);
    return buf;
}

inline u32 read_u24_be(const u8* p)
{
    return (u32(p[0]) << 16) | (u32(p[1]) << 8) | p[2];
}

inline u32 read_u32_be(const u8* p)
{
    return (u32(p[0]) << 24) | (u32(p[1]) << 16) | (u32(p[2]) << 8) | p[3];
}

inline u16 read_u16_be(const u8* p)
{
    return u16((p[0] << 8) | p[1]);
}

// ─────────────────────────────────────────────────── BZip2 (libbz2) ──

inline std::vector<u8> bzip2_decompress(const u8* data, size_t len)
{
    // JAG bzip2 data does NOT include the "BZh9" stream header; feed it
    // first, then the payload.
    char header[4] = {'B', 'Z', 'h', '9'};

    bz_stream s{};
    if(BZ2_bzDecompressInit(&s, 0, 0) != BZ_OK)
        throw std::runtime_error("BZ2_bzDecompressInit failed");

    std::vector<u8> out;
    u8  buf[65536];
    int ret = BZ_OK;
    for(int pass = 0; pass < 2 && ret == BZ_OK; ++pass)
    {
        s.next_in = pass == 0 ? header
                              : const_cast<char*>(
                                    reinterpret_cast<const char*>(data));
        s.avail_in = pass == 0 ? 4u : (unsigned)len;
        do
        {
            s.next_out  = reinterpret_cast<char*>(buf);
            s.avail_out = sizeof(buf);
            ret         = BZ2_bzDecompress(&s);
            if(ret != BZ_OK && ret != BZ_STREAM_END)
            {
                BZ2_bzDecompressEnd(&s);
                throw std::runtime_error("bzip2 decompression failed");
            }
            out.insert(out.end(), buf, buf + sizeof(buf) - s.avail_out);
        } while(ret == BZ_OK && s.avail_in > 0);
    }
    BZ2_bzDecompressEnd(&s);
    if(ret != BZ_STREAM_END)
        throw std::runtime_error("bzip2 stream truncated");
    return out;
}

// ─────────────────────────────────────────────── raw DEFLATE (gzip) ──

inline std::vector<u8> gzip_decompress(const u8* data, size_t len)
{
    if(len < 18 || data[0] != 0x1f || data[1] != 0x8b)
        throw std::runtime_error("not a gzip stream");

    // Skip gzip header: 10 fixed bytes + optional fields
    size_t hdr = 10;
    u8 flg = data[3];
    if(flg & 0x04) // FEXTRA
    {
        if(hdr + 2 > len) throw std::runtime_error("truncated gzip extra");
        u16 xlen = u16(data[hdr]) | u16(data[hdr + 1] << 8);
        hdr += 2 + xlen;
    }
    if(flg & 0x08) // FNAME  — null-terminated
    {
        while(hdr < len && data[hdr]) ++hdr;
        ++hdr;
    }
    if(flg & 0x10) // FCOMMENT
    {
        while(hdr < len && data[hdr]) ++hdr;
        ++hdr;
    }
    if(flg & 0x02) hdr += 2; // FHCRC

    if(hdr + 8 > len) throw std::runtime_error("gzip body too short");
    size_t body_len = len - hdr - 8; // strip 8-byte CRC32 + ISIZE trailer

    // Inflate raw deflate stream
    z_stream z = {};
    z.next_in  = const_cast<u8*>(data + hdr);
    z.avail_in = static_cast<uInt>(body_len);
    if(inflateInit2(&z, -15) != Z_OK)
        throw std::runtime_error("inflateInit2 failed");

    std::vector<u8> out;
    u8 buf[65536];
    int ret;
    do
    {
        z.next_out  = buf;
        z.avail_out = sizeof(buf);
        ret         = inflate(&z, Z_NO_FLUSH);
        if(ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
        {
            inflateEnd(&z);
            throw std::runtime_error("inflate error");
        }
        out.insert(out.end(), buf, buf + sizeof(buf) - z.avail_out);
    } while(ret != Z_STREAM_END);

    inflateEnd(&z);
    return out;
}

// ──────────────────────────────────────────────────────── JAG archive ──

inline u32 jag_hash(const std::string& name)
{
    u32 h = 0;
    for(char c : name)
    {
        if(c >= 'a' && c <= 'z') c -= 32; // uppercase
        h = h * 61 + u32(c) - 32;
    }
    return h;
}

// Returns the decompressed JAG archive body from a raw .dat file
inline std::vector<u8> jag_load_archive(const std::vector<u8>& raw)
{
    if(raw.size() < 6)
        throw std::runtime_error("JAG file too short");
    u32 decomp = read_u24_be(raw.data());
    u32 comp   = read_u24_be(raw.data() + 3);
    if(decomp == comp)
        return std::vector<u8>(raw.begin() + 6, raw.end());
    return bzip2_decompress(raw.data() + 6, raw.size() - 6);
}

// Extract a named file from a decompressed JAG archive body
inline std::optional<std::vector<u8>> jag_extract(
    const std::vector<u8>& body, const std::string& filename)
{
    if(body.size() < 2) return std::nullopt;
    u16 num = read_u16_be(body.data());

    size_t idx_off = 2;
    size_t dat_off = 2 + size_t(num) * 10;

    u32 target = jag_hash(filename);
    for(u16 i = 0; i < num; ++i, idx_off += 10)
    {
        if(idx_off + 10 > body.size()) break;
        u32 h  = read_u32_be(body.data() + idx_off);
        u32 ds = read_u24_be(body.data() + idx_off + 4);
        u32 cs = read_u24_be(body.data() + idx_off + 7);

        if(h == target)
        {
            if(dat_off + cs > body.size()) return std::nullopt;
            if(ds == cs)
                return std::vector<u8>(
                    body.begin() + dat_off, body.begin() + dat_off + cs);
            return bzip2_decompress(body.data() + dat_off, cs);
        }
        dat_off += cs;
    }
    return std::nullopt;
}

// ────────────────────────────────────────────────────────── flo.dat ──

struct Underlay
{
    u8          r{128}, g{128}, b{128};
    int         texture{-1};
    bool        is_overlay{false}; // op 3 (rev ≤ 377): entry is an overlay
    std::string name;              // op 6: "water", "lava", "road", …
    // HSL decomposition for underlay blending (client FloorType.setHsl):
    // hue is pre-weighted by hue_mult so grey tiles don't drag the hue.
    int hue{0}, sat{0}, light{0}, hue_mult{1};
};

// client FloorType.setHsl: decompose RGB for the blend accumulators
inline void underlay_set_hsl(Underlay& u)
{
    double r = u.r / 256.0, g = u.g / 256.0, b = u.b / 256.0;
    double mn = std::min({r, g, b}), mx = std::max({r, g, b});
    double hue = 0.0, sat = 0.0, light = (mx + mn) / 2.0;
    if(mx != mn)
    {
        sat = light < 0.5 ? (mx - mn) / (mx + mn)
                          : (mx - mn) / (2.0 - mx - mn);
        if(mx == r)
            hue = (g - b) / (mx - mn);
        else if(mx == g)
            hue = 2.0 + (b - r) / (mx - mn);
        else
            hue = 4.0 + (r - g) / (mx - mn);
    }
    hue /= 6.0;
    u.sat   = std::min(255, std::max(0, int(sat * 256.0)));
    u.light = std::min(255, std::max(0, int(light * 256.0)));
    u.hue_mult =
        std::max(1, int(512.0 * (sat * (light > 0.5 ? 1.0 - light : light))));
    u.hue = int(u.hue_mult * hue);
}

// client ColorUtil.packHsl: 8-bit HSL components → u16 palette index
inline u16 pack_hsl(int hue, int saturation, int lightness)
{
    if(lightness > 179) saturation /= 2;
    if(lightness > 192) saturation /= 2;
    if(lightness > 217) saturation /= 2;
    if(lightness > 243) saturation /= 2;
    return u16(((saturation / 32) << 7) + ((hue / 4) << 10) + lightness / 2);
}

// Texture ids the dat-era client scrolls every frame (water droplets, lava)
constexpr int ANIMATED_TEXTURES[] = {17, 24};

inline std::vector<Underlay> parse_flo_dat(const std::vector<u8>& data)
{
    if(data.size() < 2) return {};
    u16 count = read_u16_be(data.data());
    std::vector<Underlay> ul(count);

    size_t pos = 2;
    for(u16 i = 0; i < count; ++i)
    {
        while(pos < data.size())
        {
            u8 op = data[pos++];
            if(op == 0 || op == 0xff)
                break;
            else if(op == 1 && pos + 3 <= data.size()) // 24-bit RGB colour
            {
                ul[i].r = data[pos];
                ul[i].g = data[pos + 1];
                ul[i].b = data[pos + 2];
                pos += 3;
            }
            else if(op == 2 && pos < data.size()) // texture index
                ul[i].texture = data[pos++];
            else if(op == 3) // rev ≤ 377: marks the entry as an overlay
                ul[i].is_overlay = true;
            else if(op == 5) // hide-underlay flag, no data
                ;
            else if(op == 6) // name: bytes until 0x0a
            {
                while(pos < data.size() && data[pos] != 0x0a)
                    ul[i].name += char(data[pos++]);
                if(pos < data.size()) ++pos; // skip 0x0a
            }
            else if(op == 7 && pos + 3 <= data.size()) // secondary RGB
                pos += 3;
        }
    }
    for(auto& u : ul)
        underlay_set_hsl(u);
    return ul;
}

// ────────────────────────────────────────────── map_index (archive 5) ──

struct RegionRef
{
    u8  region_x, region_y;
    u16 land_file_id, map_file_id;
    u8  members;
};

inline std::vector<RegionRef> parse_map_index(const std::vector<u8>& data)
{
    // 7 bytes per region entry (big-endian shorts)
    size_t count = data.size() / 7;
    std::vector<RegionRef> regions;
    regions.reserve(count);
    for(size_t i = 0; i < count; ++i)
    {
        const u8* p = data.data() + i * 7;
        RegionRef r;
        r.region_x    = p[0];
        r.region_y    = p[1];
        r.land_file_id  = read_u16_be(p + 2);
        r.map_file_id   = read_u16_be(p + 4);
        r.members       = p[6];
        regions.push_back(r);
    }
    return regions;
}

// ────────────────────────────────────────────────────── terrain tiles ──

constexpr int REGION_SIZE = 64;
constexpr int NUM_PLANES  = 4;

struct Tile
{
    int  height{0};
    int  underlay_id{0}; // 1-indexed into flo.dat; 0 = none
    int  overlay_id{0};  // 1-indexed; 0 = none
    int  overlay_shape{0};    // tile shape template - 1 (client tileShapes)
    int  overlay_rotation{0}; // shape rotation, 0-3
    int  settings{0};
    bool height_explicit{false}; // true = opcode-1 height, false = procedural
};

using Plane  = std::array<std::array<Tile, REGION_SIZE>, REGION_SIZE>;
using Planes = std::array<Plane, NUM_PLANES>;

// ── RS2 procedural terrain height (Perlin-style) ──────────────────────────
// Tiles without explicit height (opcode 1 not received) get their height from
// this noise function using absolute world tile coordinates.
// Reference: decompiled RS2 client ~build 317-600; also OSRS World Map (BSD).

inline int noise(int x, int y)
{
    // Classic RS2 integer hash noise, returns 0-255
    int n = x + y * 57;
    n     = (n << 13) ^ n;
    return ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) >> 19 & 0xFF;
}

inline double smooth_noise(int x, int y)
{
    double corners = (noise(x-1,y-1) + noise(x+1,y-1) +
                      noise(x-1,y+1) + noise(x+1,y+1)) / 16.0;
    double sides   = (noise(x-1,y)   + noise(x+1,y) +
                      noise(x,  y-1) + noise(x,  y+1)) / 8.0;
    return corners + sides + noise(x, y) / 4.0;
}

inline double interpolated_noise(int x, int y, int scale)
{
    int    ix = x / scale, fx = x % scale;
    int    iy = y / scale, fy = y % scale;
    if(fx < 0) { fx += scale; --ix; }
    if(fy < 0) { fy += scale; --iy; }

    auto fade = [](double t) { return (1.0 - std::cos(t * M_PI)) * 0.5; };

    double v1 = smooth_noise(ix,   iy);
    double v2 = smooth_noise(ix+1, iy);
    double v3 = smooth_noise(ix,   iy+1);
    double v4 = smooth_noise(ix+1, iy+1);

    double tx = fade((double)fx / scale);
    double ty = fade((double)fy / scale);
    double i1 = v1 + tx * (v2 - v1);
    double i2 = v3 + tx * (v4 - v3);
    return i1 + ty * (i2 - i1);
}

// Procedural base height (tile units) for plane 0 at world tile (wx, wy).
// Result is clamped to [0, 255]; average terrain ≈ 35 tile units = 280 game units.
inline int ground_height(int wx, int wy)
{
    // scale=4 and scale=2 only — scale=1 is per-tile noise that looks jagged.
    // Amplitudes calibrated for gentle rolling hills (±20 tile units from base 35).
    double n1 = interpolated_noise(wx + 45365, wy + 91923, 4) - 128.0;
    double n2 = interpolated_noise(wx + 10294, wy + 37821, 2) - 128.0;
    int h = static_cast<int>(std::round(35.0 + n1 * 0.15 + n2 * 0.075));
    return std::max(0, std::min(255, h));
}

// RS2 terrain opcode stream: per plane, per x, per y.
// region_x/y are the RS2 region coordinates (world tile origin = region * 64).
inline Planes parse_terrain(const std::vector<u8>& raw, int region_x, int region_y)
{
    Planes planes{};

    size_t pos = 0;
    for(int plane = 0; plane < NUM_PLANES; ++plane)
        for(int x = 0; x < REGION_SIZE; ++x)
            for(int y = 0; y < REGION_SIZE; ++y)
            {
                Tile& t = planes[plane][x][y];
                while(pos < raw.size())
                {
                    u8 v = raw[pos++];
                    if(v == 0)
                        break;
                    else if(v == 1)
                    {
                        int h = raw[pos++];
                        if(h == 1) h = 0; // client quirk
                        // Upper-plane heights are RELATIVE to the plane
                        // below (client: tileHeights[level-1] - height*8
                        // with negative-up = below + h upward).
                        t.height = plane == 0
                                       ? h
                                       : planes[plane - 1][x][y].height + h;
                        t.height_explicit = true;
                        break;
                    }
                    else if(v <= 49)
                    {
                        // opcode encodes the overlay tile shape + rotation
                        t.overlay_id       = raw[pos++];
                        t.overlay_shape    = (v - 2) / 4;
                        t.overlay_rotation = (v - 2) & 3;
                    }
                    else if(v <= 81)
                        t.settings = v - 49;
                    else
                        t.underlay_id = v - 81; // 1-indexed
                }

                if(!t.height_explicit)
                {
                    if(plane == 0)
                        t.height = ground_height(region_x * REGION_SIZE + x,
                                                 region_y * REGION_SIZE + y);
                    else
                        // default: one level (240 units = 30 tile units)
                        // ABOVE the plane below
                        t.height = planes[plane-1][x][y].height + 30;
                }
            }
    return planes;
}

// ─────────────────────────────────────────────── map loc placements ──

// unsigned smart — 1-byte (0-127) or 2-byte (128-32767) varint used by the
// RS2 map-loc placement stream.
inline int read_usmart(const u8* d, size_t& pos)
{
    int v = d[pos++] & 0xFF;
    if(v < 128) return v;
    return ((v << 8) | (d[pos++] & 0xFF)) - 32768;
}
inline int read_usmart(const std::vector<u8>& d, size_t& pos)
{
    return read_usmart(d.data(), pos);
}

struct Placement
{
    int loc_id;
    int world_x, world_y; // absolute tile coordinates
    int plane, type, rotation;
};

// Parse the map loc placement stream (from index 4, map_file_id).
// Format (RS2 build ~317-600): delta-encoded loc IDs, packed tile positions,
// then a type/rotation byte per placement.
inline std::vector<Placement> parse_map_locs(
    const std::vector<u8>& raw, int region_x, int region_y)
{
    std::vector<Placement> result;
    size_t pos = 0;
    int loc_id = -1;
    while(pos < raw.size())
    {
        int delta = read_usmart(raw, pos);
        if(delta == 0) break;
        loc_id += delta;

        int packed = 0;
        while(pos < raw.size())
        {
            int pos_delta = read_usmart(raw, pos);
            if(pos_delta == 0) break;
            packed += pos_delta - 1;

            int local_z  = packed & 0x3F;         // tile y within region (north-south)
            int local_x  = (packed >> 6) & 0x3F;  // tile x within region (east-west)
            int plane    = packed >> 12;

            if(pos >= raw.size()) break;
            u8  info     = raw[pos++];
            int type     = info >> 2;
            int rotation = info & 3;

            result.push_back({
                loc_id,
                region_x * REGION_SIZE + local_x,
                region_y * REGION_SIZE + local_z,
                plane, type, rotation});
        }
    }
    return result;
}

// ──────────────────────────────────────────────── loc definitions ──
// World objects are "locs" (locations); their config is loc.dat + loc.idx in
// JAG archive 2. obj.dat is *item* definitions — not what map placements
// reference. Layout per OSRS-World-Map IndexedDatTypeLoader + LocType:
//   loc.idx: u16 count, then count × u16 entry length
//   loc.dat: u16 count, then entries back to back (entry i starts at
//            2 + sum(lengths[0..i)))
// The idx offsets make each definition self-contained: an unknown opcode can
// only desync one entry, never the whole stream.

struct LocDef
{
    // models[i] = model ids for shape types[i]. If types is empty (opcode 5),
    // models holds a single list used only for shape 10 (NORMAL).
    std::vector<std::vector<int>> models;
    std::vector<int>              types;
    int  size_x = 1, size_y = 1; // footprint in tiles
    bool is_rotated = false;     // op 62: model is mirrored
    bool contoured_ground = false; // op 21: vertices follow the heightmap
    int  model_size_x = 128, model_size_h = 128, model_size_y = 128;
    int  offset_x = 0, offset_h = 0, offset_y = 0;
    std::vector<std::pair<u16, u16>> recolor; // face HSL from → to
    std::string              name;    // op 2
    std::vector<std::string> actions; // ops 30-38 ("Climb-up", "Open", …)
};

// Decode one loc.dat entry in [pos, end). Opcode table from
// OSRS-World-Map LocType.decodeOpcode (dat-era: u16 model ids, strings
// terminated by 0x0a).
inline void parse_loc_entry(
    const std::vector<u8>& d, size_t pos, size_t end, LocDef& def)
{
    auto read_string = [&]() -> std::string {
        std::string s;
        while(pos < end && d[pos] != 0x0a)
            s += char(d[pos++]);
        if(pos < end) ++pos;
        return s;
    };
    auto skip_string = [&] { (void)read_string(); };

    while(pos < end)
    {
        u8 op = d[pos++];
        if(op == 0) break;
        switch(op)
        {
        case 1: {
            int n = d[pos++];
            for(int j = 0; j < n && pos + 3 <= end; ++j)
            {
                def.models.push_back({(int)read_u16_be(d.data() + pos)});
                def.types.push_back(d[pos + 2]);
                pos += 3;
            }
            break;
        }
        case 2: def.name = read_string(); break;
        case 3: skip_string(); break;
        case 5: {
            int n = d[pos++];
            def.types.clear();
            def.models.assign(1, {});
            for(int j = 0; j < n && pos + 2 <= end; ++j, pos += 2)
                def.models[0].push_back((int)read_u16_be(d.data() + pos));
            break;
        }
        case 14: def.size_x = d[pos++]; break;
        case 15: def.size_y = d[pos++]; break;
        case 17: case 18: break;
        case 19: pos += 1; break;
        case 21: def.contoured_ground = true; break;
        case 22: case 23: break;
        case 24: pos += 2; break; // seq id
        case 25: case 27: break;
        case 28: case 29: pos += 1; break;
        case 30: case 31: case 32: case 33: case 34:
        case 35: case 36: case 37: case 38: {
            std::string a = read_string();
            if(!a.empty() && a != "hidden")
                def.actions.push_back(std::move(a));
            break;
        }
        case 39: pos += 1; break;
        case 40: {
            int n = d[pos++];
            for(int j = 0; j < n && pos + 4 <= end; ++j, pos += 4)
                def.recolor.emplace_back(
                    read_u16_be(d.data() + pos), read_u16_be(d.data() + pos + 2));
            break;
        }
        case 41: pos += size_t(d[pos]) * 4 + 1; break; // retexture (later revs)
        case 44: case 45: pos += 2; break;
        case 60: case 61: pos += 2; break;
        case 62: def.is_rotated = true; break;
        case 64: break;
        case 65: def.model_size_x = read_u16_be(d.data() + pos); pos += 2; break;
        case 66: def.model_size_h = read_u16_be(d.data() + pos); pos += 2; break;
        case 67: def.model_size_y = read_u16_be(d.data() + pos); pos += 2; break;
        case 68: pos += 2; break;
        case 69: pos += 1; break;
        case 70: def.offset_x = (int16_t)read_u16_be(d.data() + pos); pos += 2; break;
        case 71: def.offset_h = (int16_t)read_u16_be(d.data() + pos); pos += 2; break;
        case 72: def.offset_y = (int16_t)read_u16_be(d.data() + pos); pos += 2; break;
        case 73: case 74: break;
        case 75: pos += 1; break;
        case 77: { // varbit transforms
            pos += 4;
            if(pos < end)
            {
                int n = d[pos++];
                pos += size_t(n + 1) * 2;
            }
            break;
        }
        default:
            // Unknown opcode — abandon this entry (idx keeps others intact)
            return;
        }
    }
}

inline std::vector<LocDef> parse_loc_defs(
    const std::vector<u8>& dat, const std::vector<u8>& idx)
{
    if(idx.size() < 2 || dat.size() < 2) return {};
    int count = (int)read_u16_be(idx.data());
    std::vector<LocDef> defs(count);

    size_t off = 2; // both files start with a 2-byte count
    for(int i = 0; i < count && 2 + size_t(i) * 2 + 2 <= idx.size(); ++i)
    {
        size_t len = read_u16_be(idx.data() + 2 + size_t(i) * 2);
        if(off + len > dat.size()) break;
        parse_loc_entry(dat, off, off + len, defs[i]);
        off += len;
    }
    return defs;
}

// ─────────────────────────────────────────────────── RS2 model parser ──

// Per-face colour: low 16 bits = RS2 HSL, or texture id when the
// FACE_TEXTURED bit is set (old-format textured faces store the texture id
// in the colour field; plain HSL values can legitimately use bit 15, so the
// texture marker needs a bit outside the u16).
constexpr u32 FACE_TEXTURED = 0x10000;

struct RsModel
{
    std::vector<std::array<int,3>> verts;  // x, y (negative = up), z
    std::vector<std::array<int,3>> faces;  // vertex indices
    std::vector<u32>               colors; // per-face HSL or FACE_TEXTURED|id
    std::vector<u8>                alphas; // per-face: 0 = opaque (client)
    // texture mapping: per textured face, the P/M/N triangle defining
    // texture space. tex_coord[i] indexes tex_pmn, or -1 = the face's own
    // vertices are P/M/N (identity mapping).
    std::vector<std::array<int,3>> tex_pmn;
    std::vector<i32>               tex_coord;
    bool valid = false;
};

// Signed smart used by vertex deltas AND face index deltas:
// byte < 0x80 → byte-64; else u16-49152.
// (OSRS-World-Map ByteBuffer.readSmart2)
inline int read_smart2(const u8* d, size_t& pos)
{
    if(d[pos] < 128) return int(d[pos++]) - 64;
    int v = ((int(d[pos]) << 8) | d[pos + 1]) - 49152;
    pos += 2;
    return v;
}

// Classic RS2 model format ("old" format, no version trailer), ported from
// OSRS-World-Map ModelData.decodeOld. 18-byte footer:
//   u16 vertexCount, u16 faceCount, u8 texturedFaceCount, u8 usesTextures,
//   u8 priority (255 = per-face), u8 hasAlpha, u8 hasFaceSkins,
//   u8 hasVertexSkins, u16 xLen, u16 yLen, u16 zLen, u16 faceIndexLen
// Section order from byte 0:
//   vertex flags [nv], face compression types [nf], face priorities [nf]?,
//   face skins [nf]?, face texture flags [nf]?, vertex skins [nv]?,
//   face alphas [nf]?, face index deltas [faceIndexLen],
//   face colors [nf*2], texture mapping [ntex*6], x [xLen], y [yLen], z [zLen]
inline RsModel parse_model(const std::vector<u8>& raw)
{
    RsModel m;
    if(raw.size() < 18) return m;

    const u8* f    = raw.data() + raw.size() - 18;
    int nv         = read_u16_be(f);
    int nf         = read_u16_be(f + 2);
    int ntex       = f[4];
    int uses_tex   = f[5];
    int priority   = f[6];
    int has_alpha  = f[7];
    int has_fskins = f[8];
    int has_vskins = f[9];
    int x_len      = read_u16_be(f + 10);
    int y_len      = read_u16_be(f + 12);
    int z_len      = read_u16_be(f + 14);
    int fi_len     = read_u16_be(f + 16);

    if(nv <= 0 || nf <= 0) return m;

    size_t off         = 0;
    size_t off_vflags  = off; off += nv;
    size_t off_ctypes  = off; off += nf; // face compression types
    off += (priority == 255) ? nf : 0;   // face priorities (unused here)
    off += has_fskins ? nf : 0;
    size_t off_texflag = off; off += uses_tex ? nf : 0;
    off += has_vskins ? nv : 0;
    size_t off_falpha  = off; off += has_alpha ? nf : 0;
    size_t off_fidx    = off; off += fi_len;
    size_t off_fcolor  = off; off += size_t(nf) * 2;
    size_t off_texmap  = off; off += size_t(ntex) * 6; // texture P/M/N triples
    size_t off_vx      = off; off += x_len;
    size_t off_vy      = off; off += y_len;
    size_t off_vz      = off; off += z_len;

    if(off > raw.size() - 18) return m;

    const u8* d = raw.data();

    // ── Vertices: per-axis delta streams selected by flag bits ──
    m.verts.resize(nv);
    {
        size_t pf = off_vflags, px = off_vx, py = off_vy, pz = off_vz;
        int x = 0, y = 0, z = 0;
        for(int i = 0; i < nv; ++i)
        {
            int flags = d[pf++];
            if(flags & 1) x += read_smart2(d, px);
            if(flags & 2) y += read_smart2(d, py);
            if(flags & 4) z += read_smart2(d, pz);
            m.verts[i] = {x, y, z};
        }
    }

    // ── Face indices: compression type array + signed delta stream.
    // All deltas accumulate against the LAST decoded index (lastIndex),
    // not against the per-corner values.
    m.faces.resize(nf);
    {
        size_t pt = off_ctypes, pi = off_fidx;
        int a = 0, b = 0, c = 0, last = 0;
        for(int i = 0; i < nf; ++i)
        {
            int type = d[pt++];
            switch(type)
            {
            case 1:
                a = read_smart2(d, pi) + last;
                b = read_smart2(d, pi) + a;
                c = read_smart2(d, pi) + b;
                last = c;
                break;
            case 2:
                b = c;
                c = read_smart2(d, pi) + last;
                last = c;
                break;
            case 3:
                a = c;
                c = read_smart2(d, pi) + last;
                last = c;
                break;
            case 4:
                std::swap(a, b);
                c = read_smart2(d, pi) + last;
                last = c;
                break;
            }
            if(a >= 0 && b >= 0 && c >= 0 && a < nv && b < nv && c < nv)
                m.faces[i] = {a, b, c};
        }
    }

    // ── Face alphas (0 = opaque, larger = more transparent) ──
    m.alphas.assign(nf, 0);
    if(has_alpha)
        for(int i = 0; i < nf; ++i)
            m.alphas[i] = d[off_falpha + i];

    // ── Texture P/M/N triangles ──
    m.tex_pmn.resize(ntex);
    {
        size_t p = off_texmap;
        for(int i = 0; i < ntex; ++i, p += 6)
            m.tex_pmn[i] = {
                (int)read_u16_be(d + p), (int)read_u16_be(d + p + 2),
                (int)read_u16_be(d + p + 4)};
    }

    // ── Face colors (u16 HSL). If the model uses textures, a per-face flag
    // byte follows: bit 1 = texture-mapped face, in which case the colour
    // field holds the texture id and bits 2+ index the texture triangle.
    m.colors.resize(nf);
    m.tex_coord.assign(nf, -1);
    {
        size_t pc = off_fcolor, ptf = off_texflag;
        for(int i = 0; i < nf; ++i)
        {
            u32 col = read_u16_be(d + pc); pc += 2;
            if(uses_tex)
            {
                int flag = d[ptf++];
                if(flag & 2)
                {
                    col |= FACE_TEXTURED;
                    int tc = flag >> 2;
                    // decodeOld: a mapping equal to the face's own vertices
                    // is the identity — drop it
                    if(tc >= 0 && tc < ntex &&
                       !(m.faces[i][0] == m.tex_pmn[tc][0] &&
                         m.faces[i][1] == m.tex_pmn[tc][1] &&
                         m.faces[i][2] == m.tex_pmn[tc][2]))
                        m.tex_coord[i] = tc;
                }
            }
            m.colors[i] = col;
        }
    }

    m.valid = true;
    return m;
}

// Per-corner UVs of face fi (must be textured). PMN triangle projection,
// ported from OSRS-World-Map TextureMapper.computeTextureCoords (render
// type 0 — the only type in the old model format).
inline void model_face_uvs(const RsModel& m, size_t fi, float* uu, float* vv)
{
    const auto& fc = m.faces[fi];
    int p = fc[0], mm = fc[1], n = fc[2];
    if(m.tex_coord[fi] >= 0)
    {
        const auto& t = m.tex_pmn[m.tex_coord[fi]];
        p = t[0]; mm = t[1]; n = t[2];
        if(p >= (int)m.verts.size() || mm >= (int)m.verts.size() ||
           n >= (int)m.verts.size())
        {
            p = fc[0]; mm = fc[1]; n = fc[2];
        }
    }

    auto vx = [&](int i) { return double(m.verts[i][0]); };
    auto vy = [&](int i) { return double(m.verts[i][1]); };
    auto vz = [&](int i) { return double(m.verts[i][2]); };

    double mx = vx(mm) - vx(p), my = vy(mm) - vy(p), mz = vz(mm) - vz(p);
    double nx = vx(n) - vx(p),  ny = vy(n) - vy(p),  nz = vz(n) - vz(p);
    double d0x = vx(fc[0]) - vx(p), d0y = vy(fc[0]) - vy(p), d0z = vz(fc[0]) - vz(p);
    double d1x = vx(fc[1]) - vx(p), d1y = vy(fc[1]) - vy(p), d1z = vz(fc[1]) - vz(p);
    double d2x = vx(fc[2]) - vx(p), d2y = vy(fc[2]) - vy(p), d2z = vz(fc[2]) - vz(p);

    double cx = my * nz - mz * ny;
    double cy = mz * nx - mx * nz;
    double cz = mx * ny - my * nx;

    double ux = ny * cz - nz * cy;
    double uy = nz * cx - nx * cz;
    double uz = nx * cy - ny * cx;
    double du = ux * mx + uy * my + uz * mz;
    if(du != 0.0)
    {
        double inv = 1.0 / du;
        uu[0] = float((ux * d0x + uy * d0y + uz * d0z) * inv);
        uu[1] = float((ux * d1x + uy * d1y + uz * d1z) * inv);
        uu[2] = float((ux * d2x + uy * d2y + uz * d2z) * inv);
    }
    else
        uu[0] = uu[1] = uu[2] = 0.f;

    double wx = my * cz - mz * cy;
    double wy = mz * cx - mx * cz;
    double wz = mx * cy - my * cx;
    double dv = wx * nx + wy * ny + wz * nz;
    if(dv != 0.0)
    {
        double inv = 1.0 / dv;
        vv[0] = float((wx * d0x + wy * d0y + wz * d0z) * inv);
        vv[1] = float((wx * d1x + wy * d1y + wz * d1z) * inv);
        vv[2] = float((wx * d2x + wy * d2y + wz * d2z) * inv);
    }
    else
        vv[0] = vv[1] = vv[2] = 0.f;

    // client snap: near-unit spans clamp the larger coord to exactly 1
    auto snap = [](float& a, float b) {
        if(a - b > 0.99f && a - b < 1.1f) a = 1.f;
    };
    snap(uu[1], uu[0]); snap(uu[2], uu[1]); snap(uu[0], uu[2]);
    snap(uu[0], uu[1]); snap(uu[1], uu[2]); snap(uu[2], uu[0]);
}

// ─────────────────────────────────────────────────── color conversion ──

struct Color { u8 r, g, b; };

// Resolve underlay colour for a tile (falls back to mid-grey)
inline Color underlay_color(int uid, const std::vector<Underlay>& ul)
{
    if(uid > 0 && uid <= int(ul.size()))
    {
        const auto& u = ul[uid - 1];
        return {u.r, u.g, u.b};
    }
    return {128, 128, 128};
}

// RS2 HSL u16 → RGB, exact client palette math (OSRS-World-Map
// ColorUtil.buildPalette with brightness 0.8).
// Encoding: bits 10-15 = hue (0-63), bits 7-9 = saturation (0-7),
//           bits 0-6  = lightness (0-127).
inline Color hsl_to_rgb(u16 hsl)
{
    int    hue_sat = hsl >> 7;
    double h = (hue_sat >> 3) / 64.0 + 0.0078125;
    double s = (hue_sat & 7) / 8.0 + 0.0625;
    double l = (hsl & 0x7F) / 128.0;

    double r = l, g = l, b = l;
    if(s != 0.0)
    {
        double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        double p = 2.0 * l - q;
        auto channel = [&](double t) {
            if(t > 1.0) t -= 1.0;
            if(t < 0.0) t += 1.0;
            if(6.0 * t < 1.0) return p + (q - p) * 6.0 * t;
            if(2.0 * t < 1.0) return q;
            if(3.0 * t < 2.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
            return p;
        };
        r = channel(h + 1.0 / 3.0);
        g = channel(h);
        b = channel(h - 1.0 / 3.0);
    }

    // client gamma: pow(channel, brightness=0.8)
    auto bright = [](double c) {
        int v = int(std::pow(std::max(0.0, c), 0.8) * 256.0);
        return (u8)std::min(255, std::max(0, v));
    };
    return {bright(r), bright(g), bright(b)};
}

// ───────────────────────────────────────────────── texture sprites ──

struct SpriteRgba
{
    int             width = 0, height = 0;
    std::vector<u8> rgba; // width*height*4, transparent palette entry 0
};

// client gamma for texel colours
inline u8 brighten_channel(int c)
{
    int v = int(std::pow(c / 256.0, 0.8) * 256.0);
    return (u8)std::min(255, std::max(0, v));
}

// Full sprite decode (SpriteLoader.loadIndexedSpriteDatId, offset 0):
// sprite dat starts with u16 offset into the shared index.dat, which holds
//   u16 canvas w, u16 canvas h, u8 paletteSize, (paletteSize-1) × u24 RGB,
//   u8 xOffset, u8 yOffset, u16 subWidth, u16 subHeight, u8 pixelOrder
// pixel indices follow in the sprite dat (order 0 = row major, 1 = column).
inline std::optional<SpriteRgba> load_sprite_rgba(
    const std::vector<u8>& body, const std::vector<u8>& idx, int id)
{
    auto sprite = jag_extract(body, std::to_string(id) + ".dat");
    if(!sprite || sprite->size() < 2)
        return std::nullopt;

    size_t ioff = read_u16_be(sprite->data());
    if(ioff + 5 > idx.size())
        return std::nullopt;

    int pal_size = idx[ioff + 4];
    std::vector<std::array<u8, 4>> palette(std::max(pal_size, 1),
                                           {0, 0, 0, 0});
    size_t p = ioff + 5;
    for(int i = 1; i < pal_size; ++i, p += 3)
    {
        if(p + 3 > idx.size())
            return std::nullopt;
        palette[i] = {brighten_channel(idx[p]), brighten_channel(idx[p + 1]),
                      brighten_channel(idx[p + 2]), 255};
    }

    if(p + 7 > idx.size())
        return std::nullopt;
    int sw   = read_u16_be(idx.data() + p + 2);
    int sh   = read_u16_be(idx.data() + p + 4);
    int type = idx[p + 6];

    size_t count = size_t(sw) * sh;
    if(sprite->size() < 2 + count || sw <= 0 || sh <= 0)
        return std::nullopt;

    SpriteRgba out;
    out.width  = sw;
    out.height = sh;
    out.rgba.resize(count * 4);
    const u8* px = sprite->data() + 2;
    for(size_t i = 0; i < count; ++i)
    {
        size_t dst = type == 1 ? (i % sh) * sw + i / sh : i;
        int    pi  = px[i];
        const auto& c = palette[pi < pal_size ? pi : 0];
        std::copy(c.begin(), c.end(), out.rgba.begin() + dst * 4);
    }
    return out;
}

// ──────────────────────────────────────────────── minimal PNG writer ──

inline void write_png_rgba(
    const std::string& path, int w, int h, const std::vector<u8>& rgba)
{
    // raw scanlines with filter byte 0
    std::vector<u8> raw(size_t(w) * 4 * h + h);
    for(int y = 0; y < h; ++y)
    {
        u8* row = raw.data() + size_t(y) * (size_t(w) * 4 + 1);
        row[0]  = 0;
        std::copy_n(rgba.data() + size_t(y) * w * 4, size_t(w) * 4, row + 1);
    }

    uLongf comp_len = compressBound(raw.size());
    std::vector<u8> comp(comp_len);
    if(compress2(comp.data(), &comp_len, raw.data(), raw.size(), 9) != Z_OK)
        throw std::runtime_error("png deflate failed");
    comp.resize(comp_len);

    std::ofstream f(path, std::ios::binary);
    if(!f)
        throw std::runtime_error("cannot write: " + path);

    auto be32 = [](u32 v) {
        return std::array<u8, 4>{u8(v >> 24), u8(v >> 16), u8(v >> 8), u8(v)};
    };
    auto chunk = [&](const char* tag, const std::vector<u8>& data) {
        auto len = be32(u32(data.size()));
        f.write((const char*)len.data(), 4);
        std::vector<u8> td(4 + data.size());
        std::copy_n(tag, 4, td.begin());
        std::copy(data.begin(), data.end(), td.begin() + 4);
        f.write((const char*)td.data(), td.size());
        auto crc = be32(u32(crc32(0, td.data(), uInt(td.size()))));
        f.write((const char*)crc.data(), 4);
    };

    static const u8 magic[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n'};
    f.write((const char*)magic, 8);

    std::vector<u8> ihdr;
    for(u8 b : be32(u32(w))) ihdr.push_back(b);
    for(u8 b : be32(u32(h))) ihdr.push_back(b);
    ihdr.insert(ihdr.end(), {8, 6, 0, 0, 0}); // 8-bit RGBA
    chunk("IHDR", ihdr);
    chunk("IDAT", comp);
    chunk("IEND", {});
}

// ─────────────────────────────────────────── texture average colours ──

// Textures live in JAG archive 6 of index 0: numbered sprite files plus a
// shared index.dat. For flat-coloured output we approximate each texture by
// its palette average (what the reference's getAverageHsl does),
// gamma-corrected like every other colour.
struct TextureColors
{
    std::vector<u8>              body; // decompressed archive 6
    std::optional<std::vector<u8>> index;
    std::unordered_map<int, Color> cache;
    bool loaded = false;

    void load(const std::string& cache_dir)
    {
        loaded = true;
        try
        {
            auto raw = read_file(cache_dir + "/0/6.dat");
            body     = jag_load_archive(raw);
            index    = jag_extract(body, "index.dat");
        }
        catch(const std::exception& e)
        {
            fprintf(stderr, "  texture archive unavailable: %s\n", e.what());
        }
    }

    Color get(const std::string& cache_dir, int id)
    {
        if(!loaded) load(cache_dir);
        auto it = cache.find(id);
        if(it != cache.end()) return it->second;

        Color result{192, 192, 192};
        if(index)
        {
            auto sprite = jag_extract(body, std::to_string(id) + ".dat");
            // sprite dat starts with u16 offset into index.dat:
            //   u16 width, u16 height, u8 paletteSize,
            //   (paletteSize-1) × u24 RGB (entry 0 = transparent)
            if(sprite && sprite->size() >= 2)
            {
                size_t ioff = read_u16_be(sprite->data());
                const auto& idx = *index;
                if(ioff + 5 <= idx.size())
                {
                    int pal_size = idx[ioff + 4];
                    size_t p     = ioff + 5;
                    u32 r = 0, g = 0, b = 0;
                    for(int i = 0; i + 1 < pal_size && p + 3 <= idx.size();
                        ++i, p += 3)
                    {
                        r += idx[p];
                        g += idx[p + 1];
                        b += idx[p + 2];
                    }
                    if(pal_size > 0)
                    {
                        // reference averages over the full palette,
                        // including the transparent 0 entry
                        auto bright = [&](u32 sum) {
                            double c = (sum / double(pal_size)) / 256.0;
                            int v = int(std::pow(std::max(0.0, c), 0.8) * 256.0);
                            return (u8)std::min(255, std::max(0, v));
                        };
                        result = {bright(r), bright(g), bright(b)};
                    }
                }
            }
        }
        cache.emplace(id, result);
        return result;
    }

    // full texture image (for PNG export / real texturing)
    std::optional<SpriteRgba> sprite(const std::string& cache_dir, int id)
    {
        if(!loaded) load(cache_dir);
        if(!index) return std::nullopt;
        return load_sprite_rgba(body, *index, id);
    }
};

// ──────────────────────────────────────────────────── model lighting ──

// Adjust an HSL colour's lightness by a light value (0..127 nominal);
// client ModelData.adjustLightness.
inline u16 adjust_hsl_lightness(u16 hsl, int light)
{
    int l = ((hsl & 127) * light) >> 7;
    l     = std::min(126, std::max(2, l));
    return u16((hsl & 0xff80) + l);
}

// Per-vertex lightness for a final (transformed) model — client
// ModelData.light with the dat-era constants: ambient 64, contrast 768,
// light direction (-50, -10, -50); revisions ≤ 445 ignore per-loc
// ambient/contrast. Vertex normals are the client's: per-face normal
// normalised to 256 and accumulated per corner, averaged by face count.
inline std::vector<int> model_vertex_lights(const RsModel& m)
{
    constexpr int lx = -50, ly = -10, lz = -50;
    // |light| = int(sqrt(50²+10²+50²)) = 71; intensity = 71*768 >> 8
    constexpr int intensity = (71 * 768) >> 8;

    struct N { int x = 0, y = 0, z = 0, count = 0; };
    std::vector<N> normals(m.verts.size());

    for(const auto& fc : m.faces)
    {
        const auto& a = m.verts[fc[0]];
        const auto& b = m.verts[fc[1]];
        const auto& c = m.verts[fc[2]];
        int dx1 = b[0] - a[0], dy1 = b[1] - a[1], dz1 = b[2] - a[2];
        int dx2 = c[0] - a[0], dy2 = c[1] - a[1], dz2 = c[2] - a[2];
        int nx = dy1 * dz2 - dy2 * dz1;
        int ny = dz1 * dx2 - dz2 * dx1;
        int nz = dx1 * dy2 - dx2 * dy1;
        while(nx > 8192 || ny > 8192 || nz > 8192 || nx < -8192 ||
              ny < -8192 || nz < -8192)
        {
            nx >>= 1;
            ny >>= 1;
            nz >>= 1;
        }
        int len = int(std::sqrt(double(nx) * nx + double(ny) * ny +
                                double(nz) * nz));
        if(len <= 0) len = 1;
        nx = nx * 256 / len;
        ny = ny * 256 / len;
        nz = nz * 256 / len;
        for(int k = 0; k < 3; ++k)
        {
            N& n = normals[fc[k]];
            n.x += nx;
            n.y += ny;
            n.z += nz;
            ++n.count;
        }
    }

    std::vector<int> lights(m.verts.size(), 64);
    for(size_t i = 0; i < normals.size(); ++i)
    {
        const N& n = normals[i];
        if(n.count > 0)
            lights[i] =
                64 + (lx * n.x + ly * n.y + lz * n.z) / (intensity * n.count);
    }
    return lights;
}

// ───────────────────────────────────────────────── loc model assembly ──

// Model id list a def uses for a placement shape type, or nullptr.
inline const std::vector<int>* loc_model_ids(const LocDef& def, int shape)
{
    if(def.types.empty())
    {
        // opcode-5 def: single list, NORMAL (10) shape only
        if(shape != 10 || def.models.empty() || def.models[0].empty())
            return nullptr;
        return &def.models[0];
    }
    for(size_t i = 0; i < def.types.size(); ++i)
        if(def.types[i] == shape)
            return def.models[i].empty() ? nullptr : &def.models[i];
    return nullptr;
}

// Assemble the model for one placement, following OSRS-World-Map
// LocModelLoader.getLocModelData: pick the model list for the placement's
// shape type, merge multi-model lists, mirror/rotate, recolor, resize,
// translate. Returns false when this def has no model for that shape.
// get_model must return a parsed model for a model id (or nullptr).
template<typename GetModel>
inline bool build_loc_model(
    const LocDef& def,
    int           shape,
    int           rotation,
    GetModel&&    get_model,
    RsModel&      out)
{
    const std::vector<int>* model_ids = loc_model_ids(def, shape);
    if(!model_ids)
        return false;

    // Merge (concatenate) the list's models
    out = RsModel{};
    for(int id : *model_ids)
    {
        const RsModel* part = get_model(id);
        if(!part || !part->valid)
            return false;
        int base     = (int)out.verts.size();
        int tex_base = (int)out.tex_pmn.size();
        out.verts.insert(out.verts.end(), part->verts.begin(), part->verts.end());
        for(const auto& fc : part->faces)
            out.faces.push_back({base + fc[0], base + fc[1], base + fc[2]});
        out.colors.insert(
            out.colors.end(), part->colors.begin(), part->colors.end());
        out.alphas.insert(
            out.alphas.end(), part->alphas.begin(), part->alphas.end());
        for(const auto& t : part->tex_pmn)
            out.tex_pmn.push_back({base + t[0], base + t[1], base + t[2]});
        for(i32 tc : part->tex_coord)
            out.tex_coord.push_back(tc < 0 ? -1 : tc + tex_base);
    }

    if(def.is_rotated) // mirror: negate z, reverse winding
    {
        for(auto& v : out.verts)
            v[2] = -v[2];
        for(auto& fc : out.faces)
            std::swap(fc[0], fc[2]);
    }

    switch(rotation & 3) // exact client integer rotations
    {
    case 1: // rotate90: x' = z, z' = -x
        for(auto& v : out.verts) { int t = v[0]; v[0] = v[2]; v[2] = -t; }
        break;
    case 2:
        for(auto& v : out.verts) { v[0] = -v[0]; v[2] = -v[2]; }
        break;
    case 3: // rotate270: z' = x, x' = -z
        for(auto& v : out.verts) { int t = v[2]; v[2] = v[0]; v[0] = -t; }
        break;
    }

    // Recolor face HSLs; on rev ≤ 377 the same pairs also retexture
    // (reference applies recolor() + retexture() with identical ids).
    for(auto& [from, to] : def.recolor)
        for(auto& col : out.colors)
            if(col == from || col == (FACE_TEXTURED | from))
                col = (col & FACE_TEXTURED) | to;

    if(def.model_size_x != 128 || def.model_size_h != 128 ||
       def.model_size_y != 128)
        for(auto& v : out.verts)
        {
            v[0] = v[0] * def.model_size_x / 128;
            v[1] = v[1] * def.model_size_h / 128;
            v[2] = v[2] * def.model_size_y / 128;
        }

    if(def.offset_x || def.offset_h || def.offset_y)
        for(auto& v : out.verts)
        {
            v[0] += def.offset_x;
            v[1] += def.offset_h;
            v[2] += def.offset_y;
        }

    out.valid = true;
    return true;
}

} // namespace rs2
