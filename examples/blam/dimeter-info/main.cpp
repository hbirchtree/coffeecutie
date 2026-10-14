#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>

#include "dump.h"
#include "dump_objects.h"
#include "dump_scenario.h"
#include "dump_ui.h"

#include <blam/dimeter/font.h>

#include <blam/dimeter/bitm.h>
#include <blam/dimeter/geometry.h>
#include <blam/dimeter/map.h>
#include <blam/dimeter/sound.h>
#include <blam/volta/blam_swizzle.h>
#include <coffee/core/coffee_args.h>
#include <cxxopts.hpp>

#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace Coffee;
using semantic::BytesConst;
using semantic::Span;
using libc_types::i64;
using typing::vector_types::Vecf3;

namespace {

const char* cache_type_name(blam::dimeter::cache_type_t type)
{
    using blam::dimeter::cache_type_t;
    switch(blam::from_le(type))
    {
    case cache_type_t::singleplayer:
        return "singleplayer";
    case cache_type_t::multiplayer:
        return "multiplayer";
    case cache_type_t::mainmenu:
        return "mainmenu";
    case cache_type_t::shared:
        return "shared";
    case cache_type_t::single_player_shared:
        return "single_player_shared";
    default:
        return "unknown";
    }
}

std::string sanitize(std::string_view tag_name)
{
    std::string out(tag_name);
    for(char& c : out)
        if(c == '\\' || c == '/' || c == ' ')
            c = '_';
    return out;
}

bool write_file(std::string const& path, std::vector<char> const& data)
{
    std::ofstream out(path, std::ios::binary);
    if(!out)
        return false;
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return out.good();
}

template<typename T>
void append(std::vector<char>& out, T const& value)
{
    auto const* bytes = reinterpret_cast<char const*>(&value);
    out.insert(out.end(), bytes, bytes + sizeof(T));
}

/* --- bitmap extraction ----------------------------------------------- */

struct dds_info
{
    u32 fourcc{0}; /* 0 = uncompressed, described by the mask fields */
    u32 block_bytes{0};
    u32 bpp{0};
    u32 pf_flags{0}; /* DDS pixel format flags for the uncompressed case */
    u32 mask_r{0};
    u32 mask_g{0};
    u32 mask_b{0};
    u32 mask_a{0};
};

std::optional<dds_info> dds_format(blam::bitm::format_t fmt)
{
    using blam::bitm::format_t;
    constexpr u32 RGB  = 0x40;
    constexpr u32 RGBA = 0x41;
    constexpr u32 LUM  = 0x20000; /* DDPF_LUMINANCE */
    constexpr u32 LUMA = 0x20001;
    constexpr u32 A    = 0x2;     /* DDPF_ALPHA */
    constexpr u32 BUMP = 0x80000; /* DDPF_BUMPDUDV, signed channels */
    switch(fmt)
    {
    case format_t::BC1:
        return dds_info{.fourcc = 0x31545844 /*DXT1*/, .block_bytes = 8};
    case format_t::BC2:
        return dds_info{.fourcc = 0x33545844 /*DXT3*/, .block_bytes = 16};
    case format_t::BC3:
        return dds_info{.fourcc = 0x35545844 /*DXT5*/, .block_bytes = 16};
    case format_t::ARGB8:
        return dds_info{
            .bpp      = 32,
            .pf_flags = RGBA,
            .mask_r   = 0x00FF0000,
            .mask_g   = 0x0000FF00,
            .mask_b   = 0x000000FF,
            .mask_a   = 0xFF000000};
    case format_t::XRGB8:
        return dds_info{
            .bpp      = 32,
            .pf_flags = RGB,
            .mask_r   = 0x00FF0000,
            .mask_g   = 0x0000FF00,
            .mask_b   = 0x000000FF};
    case format_t::R5G6B5:
        return dds_info{
            .bpp      = 16,
            .pf_flags = RGB,
            .mask_r   = 0xF800,
            .mask_g   = 0x07E0,
            .mask_b   = 0x001F};
    case format_t::A1RGB5:
        return dds_info{
            .bpp      = 16,
            .pf_flags = RGBA,
            .mask_r   = 0x7C00,
            .mask_g   = 0x03E0,
            .mask_b   = 0x001F,
            .mask_a   = 0x8000};
    case format_t::ARGB4:
        return dds_info{
            .bpp      = 16,
            .pf_flags = RGBA,
            .mask_r   = 0x0F00,
            .mask_g   = 0x00F0,
            .mask_b   = 0x000F,
            .mask_a   = 0xF000};
    case format_t::A8Y8:
        return dds_info{
            .bpp = 16, .pf_flags = LUMA, .mask_r = 0x00FF, .mask_a = 0xFF00};
    case format_t::Y8:
    case format_t::AY8:
        return dds_info{.bpp = 8, .pf_flags = LUM, .mask_r = 0xFF};
    case format_t::A8:
        return dds_info{.bpp = 8, .pf_flags = A, .mask_a = 0xFF};
    case format_t::V8U8:
        return dds_info{
            .bpp = 16, .pf_flags = BUMP, .mask_r = 0x00FF, .mask_g = 0xFF00};
    default:
        /* P8/P8_flat need palette handling, float formats are rare */
        return std::nullopt;
    }
}

u32 base_level_size(dds_info const& info, u32 width, u32 height)
{
    if(info.fourcc)
        return std::max(width / 4u, 1u) * std::max(height / 4u, 1u) *
               info.block_bytes;
    return width * height * (info.bpp / 8);
}

/* Minimal base-level DDS wrapper (optionally 6-face cube map), enough for
 * image viewers/texconv */
std::vector<char> make_dds(
    dds_info const& info,
    u32             width,
    u32             height,
    Span<const u8>  pixels,
    bool            cube_map = false)
{
    std::vector<char> out;
    out.reserve(128 + pixels.size());
    append<u32>(out, 0x20534444); /* 'DDS ' */
    append<u32>(out, 124);
    append<u32>(out, 0x1007); /* CAPS|HEIGHT|WIDTH|PIXELFORMAT */
    append<u32>(out, height);
    append<u32>(out, width);
    append<u32>(out, static_cast<u32>(pixels.size()) / (cube_map ? 6 : 1));
    /* depth, mipmap count, reserved[11] */
    for(int i = 0; i < 13; i++)
        append<u32>(out, 0);
    append<u32>(out, 32); /* pixel format size */
    if(info.fourcc)
    {
        append<u32>(out, 0x4); /* FOURCC */
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
    /* CAPS_TEXTURE, plus COMPLEX + all cube faces for cube maps */
    append<u32>(out, cube_map ? 0x1008 : 0x1000);
    append<u32>(out, cube_map ? 0xFE00 : 0);
    for(int i = 0; i < 3; i++)
        append<u32>(out, 0);
    out.insert(out.end(), pixels.begin(), pixels.end());
    return out;
}

template<typename V>
u32 extract_bitmaps(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::raw_pool const&         pool,
    std::string const&                     outdir,
    u32                                    limit)
{
    namespace h2 = blam::dimeter;
    using blam::from_le;

    auto magic     = map.magic();
    u32  extracted = 0;

    for(auto const& tag : map.tags())
    {
        if(limit && extracted >= limit)
            break;
        if(!tag.valid() || !tag.matches("bitm"))
            continue;
        auto const* bitm = tag.template data<h2::bitm::header_t>(magic);
        if(!bitm)
            continue;
        auto images = bitm->images.data(magic);
        if(images.has_error() || images.value().empty())
            continue;

        auto image_span = images.value();
        for(size_t img_idx = 0; img_idx < image_span.size(); img_idx++)
        {
            if(limit && extracted >= limit)
                break;
            h2::bitm::image_t const& img = image_span[img_idx];

            auto type = from_le(img.type);
            bool cube = type == blam::bitm::type_t::tex_cube;
            if(type != blam::bitm::type_t::tex_2d && !cube)
                continue; /* 3D textures not handled */
            auto fmt = dds_format(from_le(img.format));
            if(!fmt)
                continue;

            u32 width  = static_cast<u32>(from_le(img.width));
            u32 height = static_cast<u32>(from_le(img.height));
            u32 base   = base_level_size(*fmt, width, height);
            u32 faces  = cube ? 6 : 1;

            /* LOD1 is the highest-detail chain; fall back down the LODs
             * until one is present in the files we have */
            for(int lod = 0; lod < 3; lod++)
            {
                u32 lod_size = from_le(img.lod_size[lod]);
                if(lod_size < base * faces)
                    continue;
                auto raw = pool.resolve(img.lod_offset[lod], lod_size);
                if(!raw)
                    continue;
                auto const* raw_bytes =
                    reinterpret_cast<u8 const*>(raw->data());

                /* Cube maps store each face's full mip chain back to back;
                 * pull the base level from the start of each face slice */
                u32 face_stride = cube ? lod_size / 6 : lod_size;
                if(cube && face_stride < base)
                    break;

                std::vector<u8> pixels(static_cast<size_t>(base) * faces);
                for(u32 face = 0; face < faces; face++)
                {
                    auto const* src = raw_bytes + face * face_stride;
                    auto*       dst = pixels.data() + face * base;
                    /* NV2A stores uncompressed textures Morton-swizzled;
                     * DXT is always linear */
                    if(!fmt->fourcc && img.swizzled())
                    {
                        if(!blam::swizzle::deswizzle_bytes(
                               Span<const u8>(src, base),
                               Span<u8>(dst, base),
                               width,
                               height,
                               fmt->bpp / 8))
                            std::memcpy(dst, src, base);
                    } else
                        std::memcpy(dst, src, base);
                }

                auto path =
                    outdir + "/" + sanitize(map.name_of(tag)) +
                    (image_span.size() > 1 ? "_img" + std::to_string(img_idx)
                                           : "") +
                    ".dds";
                if(write_file(
                       path,
                       make_dds(
                           *fmt,
                           width,
                           height,
                           Span<const u8>(pixels.data(), pixels.size()),
                           cube)))
                {
                    cBasicPrint(
                        "  bitm {0}x{1}{2} lod{3} (format {4}, {5} bytes{6}) "
                        "-> {7}",
                        width,
                        height,
                        cube ? " cube" : "",
                        lod + 1,
                        static_cast<u16>(from_le(img.format)),
                        base * faces,
                        img.swizzled() && !fmt->fourcc ? ", deswizzled" : "",
                        path);
                    extracted++;
                }
                break;
            }
        }
    }
    return extracted;
}

/* --- sound extraction ------------------------------------------------ */

/* WAVE with format tag 0x0069 (Xbox ADPCM); ffmpeg and most tools decode
 * this directly */
std::vector<char> make_xadpcm_wav(
    u32 sample_rate, u16 channels, std::vector<char> const& payload)
{
    constexpr u16     block_align_per_channel = 36;
    std::vector<char> out;
    out.reserve(payload.size() + 60);
    append<u32>(out, 0x46464952); /* RIFF */
    append<u32>(out, 46 + static_cast<u32>(payload.size()));
    append<u32>(out, 0x45564157); /* WAVE */
    append<u32>(out, 0x20746D66); /* fmt  */
    append<u32>(out, 20);
    append<u16>(out, 0x0069);
    append<u16>(out, channels);
    append<u32>(out, sample_rate);
    /* 64 samples per 36-byte block per channel */
    append<u32>(out, sample_rate * channels * block_align_per_channel / 64);
    append<u16>(out, block_align_per_channel * channels);
    append<u16>(out, 4);          /* bits per sample */
    append<u16>(out, 2);          /* cbSize */
    append<u16>(out, 64);         /* samples per block */
    append<u32>(out, 0x61746164); /* data */
    append<u32>(out, static_cast<u32>(payload.size()));
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

template<typename V>
u32 extract_sounds(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::raw_pool const&         pool,
    std::string const&                     outdir,
    u32                                    limit)
{
    namespace h2 = blam::dimeter;
    using blam::from_le;

    auto magic = map.magic();

    /* All sample metadata lives in the map's single ugh! gestalt */
    h2::snd::gestalt const* gestalt = nullptr;
    for(auto const& tag : map.tags())
        if(tag.valid() && tag.matches("ugh!"))
        {
            gestalt = tag.template data<h2::snd::gestalt>(magic);
            break;
        }
    if(!gestalt)
    {
        cBasicPrint("  no ugh! gestalt, skipping sounds");
        return 0;
    }

    auto pitch_ranges = gestalt->pitch_ranges.data(magic);
    auto permutations = gestalt->permutations.data(magic);
    auto chunks       = gestalt->chunks.data(magic);
    if(pitch_ranges.has_error() || permutations.has_error() ||
       chunks.has_error())
    {
        cBasicPrint("  gestalt blocks out of bounds, skipping sounds");
        return 0;
    }

    /* Separate quotas so WMA sounds show up even though they cluster late in
     * the tag table; limit 0 = everything */
    u32 quota     = limit ? limit : std::numeric_limits<u32>::max();
    u32 extracted = 0, adpcm_left = quota, wma_left = quota;
    for(auto const& tag : map.tags())
    {
        if(adpcm_left == 0 && wma_left == 0)
            break;
        if(!tag.valid() || !tag.matches("snd!"))
            continue;
        auto const* sound = tag.template data<h2::snd::sound>(magic);
        if(!sound)
            continue;
        bool adpcm = sound->compression == h2::snd::compression_t::xbox_adpcm;
        /* WMA permutations are complete ASF files stored verbatim */
        bool wma = sound->compression == h2::snd::compression_t::wma;
        if(!adpcm && !wma)
            continue;
        if((adpcm && adpcm_left == 0) || (wma && wma_left == 0))
            continue;
        /* "codec" encoding is a WMA file in the chunks whatever the
         * compression byte says */
        u16 channels = sound->encoding == h2::snd::encoding_t::stereo ? 2 : 1;
        if(sound->encoding == h2::snd::encoding_t::codec)
            adpcm = false, wma = true;

        auto range_idx = from_le(sound->pitch_range_index);
        if(range_idx < 0 ||
           static_cast<size_t>(range_idx) >= pitch_ranges.value().size())
            continue;
        auto const& range = pitch_ranges.value()[range_idx];

        /* Every permutation is a distinct take/variation of the sound */
        i16  first_perm = from_le(range.first_permutation);
        i16  perm_count = from_le(range.permutation_count);
        bool wrote_any  = false;
        for(i16 p = 0; p < perm_count; p++)
        {
            auto perm_idx = static_cast<size_t>(first_perm) + p;
            if(first_perm < 0 || perm_idx >= permutations.value().size())
                break;
            auto const& perm = permutations.value()[perm_idx];

            std::vector<char> payload;
            payload.reserve(perm.sample_size());
            i16 first = from_le(perm.first_chunk);
            i16 count = from_le(perm.chunk_count);
            for(i16 c = first; c < first + count; c++)
            {
                if(c < 0 || static_cast<size_t>(c) >= chunks.value().size())
                    break;
                auto const& chunk = chunks.value()[c];
                auto        raw   = pool.resolve(chunk.offset, chunk.size());
                if(!raw)
                    break;
                payload.insert(payload.end(), raw->begin(), raw->end());
            }
            if(payload.empty() || payload.size() != perm.sample_size())
                continue;

            auto path = outdir + "/" + sanitize(map.name_of(tag)) +
                        (perm_count > 1 ? "_perm" + std::to_string(p) : "") +
                        (adpcm ? ".wav" : ".wma");
            auto rate = h2::snd::to_hertz(sound->sample_rate);
            if(write_file(
                   path,
                   adpcm ? make_xadpcm_wav(rate, channels, payload) : payload))
            {
                cBasicPrint(
                    "  snd! {0} Hz {1}ch {2} perm {3}/{4} ({5} bytes, {6} "
                    "chunks) -> {7}",
                    rate,
                    channels,
                    adpcm ? "xbox_adpcm" : "wma",
                    p + 1,
                    perm_count,
                    payload.size(),
                    count,
                    path);
                extracted++;
                wrote_any = true;
            }
        }
        if(wrote_any && (adpcm ? adpcm_left : wma_left) > 0)
            (adpcm ? adpcm_left : wma_left)--;
    }
    return extracted;
}

/* --- mesh extraction --------------------------------------------------- */

/* Growing OBJ document; v/vt indices are global, so sections can be
 * appended as named groups */
struct obj_writer
{
    std::string body;
    u32         vertex_base{1};

    void mtllib(std::string const& name)
    {
        body += "mtllib " + name + "\n";
    }

    void group(std::string const& name)
    {
        body += "o " + name + "\n";
    }

    void usemtl(std::string const& name)
    {
        body += "usemtl " + name + "\n";
    }

    void vertex(f32 x, f32 y, f32 z)
    {
        char line[96];
        std::snprintf(line, sizeof(line), "v %g %g %g\n", x, y, z);
        body += line;
    }

    void texcoord(f32 u, f32 v)
    {
        char line[64];
        std::snprintf(line, sizeof(line), "vt %g %g\n", u, 1.f - v);
        body += line;
    }

    void triangle(u32 a, u32 b, u32 c)
    {
        char line[96];
        std::snprintf(
            line,
            sizeof(line),
            "f %u/%u %u/%u %u/%u\n",
            vertex_base + a,
            vertex_base + a,
            vertex_base + b,
            vertex_base + b,
            vertex_base + c,
            vertex_base + c);
        body += line;
    }

    void end_group(u32 vertex_count)
    {
        vertex_base += vertex_count;
    }

    bool write(std::string const& path) const
    {
        std::ofstream out(path, std::ios::binary);
        out << "# extracted by DimeterInfo\n" << body;
        return out.good();
    }
};

/* --- materials --------------------------------------------------------- */

/* Extracts a bitmap tag's first image as DDS for MTL references; returns
 * the written filename */
template<typename V>
std::optional<std::string> write_material_dds(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::raw_pool const&         pool,
    std::string const&                     outdir,
    blam::dimeter::tag_t const&            bitm_tag)
{
    namespace h2 = blam::dimeter;
    using blam::from_le;

    auto const* bitm = bitm_tag.template data<h2::bitm::header_t>(map.magic());
    if(!bitm)
        return std::nullopt;
    auto images = bitm->images.data(map.magic());
    if(images.has_error() || images.value().empty())
        return std::nullopt;
    h2::bitm::image_t const& img = images.value()[0];
    if(from_le(img.type) != blam::bitm::type_t::tex_2d)
        return std::nullopt;
    auto fmt = dds_format(from_le(img.format));
    if(!fmt)
        return std::nullopt;

    u32 width  = static_cast<u32>(from_le(img.width));
    u32 height = static_cast<u32>(from_le(img.height));
    u32 base   = base_level_size(*fmt, width, height);

    for(int lod = 0; lod < 3; lod++)
    {
        if(from_le(img.lod_size[lod]) < base)
            continue;
        auto raw = pool.resolve(img.lod_offset[lod], base);
        if(!raw)
            continue;
        auto const*     src = reinterpret_cast<u8 const*>(raw->data());
        std::vector<u8> pixels(base);
        if(!fmt->fourcc && img.swizzled())
        {
            if(!blam::swizzle::deswizzle_bytes(
                   Span<const u8>(src, base),
                   Span<u8>(pixels.data(), base),
                   width,
                   height,
                   fmt->bpp / 8))
                std::memcpy(pixels.data(), src, base);
        } else
            std::memcpy(pixels.data(), src, base);

        auto filename = sanitize(map.name_of(bitm_tag)) + ".dds";
        if(write_file(
               outdir + "/" + filename,
               make_dds(
                   *fmt,
                   width,
                   height,
                   Span<const u8>(pixels.data(), pixels.size()))))
            return filename;
        break;
    }
    return std::nullopt;
}

/* Resolves shad tags to MTL entries (diffuse map + uv tiling), extracting
 * the referenced bitmaps on first use */
struct material_db
{
    std::string                mtl;
    std::map<u16, std::string> by_tag; /* shad tag index -> mtl name */

    template<typename V>
    std::string resolve(
        blam::dimeter::map_container<V> const& map,
        blam::dimeter::raw_pool const&         pool,
        std::string const&                     outdir,
        blam::dimeter::datum_index             shad_id)
    {
        namespace h2 = blam::dimeter;
        using blam::from_le;

        auto const* shad_tag = map.tag_at(shad_id);
        if(!shad_tag)
            return "";
        if(auto it = by_tag.find(shad_id.index()); it != by_tag.end())
            return it->second;

        auto name = sanitize(map.name_of(*shad_tag));
        by_tag.emplace(shad_id.index(), name);
        mtl += "newmtl " + name + "\nKd 0.8 0.8 0.8\n";

        auto const* shad =
            shad_tag->template data<h2::geo::shad_header_t>(map.magic());
        if(shad)
        {
            /* Base map from the runtime properties (the postprocess bitmap
             * slots start with bump/detail maps); uv scale from the
             * postprocess tiling entry whose bitmap slot matches it */
            h2::tag_t const* bitm_tag      = nullptr;
            u32              diffuse_datum = 0;
            f32              scale_u = 1.f, scale_v = 1.f;
            auto             rt = shad->runtime_properties.data(map.magic());
            if(!rt.has_error() && !rt.value().empty())
            {
                bitm_tag      = map.tag_at(rt.value()[0].diffuse.tag_id);
                diffuse_datum = blam::from_le(rt.value()[0].diffuse.tag_id.raw);
            }
            auto props = shad->properties.data(map.magic());
            if(!props.has_error() && !props.value().empty())
            {
                auto const& p       = props.value()[0];
                auto        bitmaps = p.bitmaps.data(map.magic());
                auto        tiling  = p.tiling.data(map.magic());
                if(!bitmaps.has_error() && !tiling.has_error())
                    for(size_t s = 0; s < bitmaps.value().size(); s++)
                        if(from_le(bitmaps.value()[s].bitmap.id.raw) ==
                               diffuse_datum &&
                           s < tiling.value().size())
                        {
                            scale_u = from_le(tiling.value()[s].x);
                            scale_v = from_le(tiling.value()[s].y);
                            break;
                        }
            }
            if(bitm_tag && bitm_tag->matches("bitm"))
                if(auto dds = write_material_dds(map, pool, outdir, *bitm_tag))
                {
                    char line[512];
                    std::snprintf(
                        line,
                        sizeof(line),
                        "map_Kd -s %g %g 1 %s\n",
                        scale_u != 0.f ? scale_u : 1.f,
                        scale_v != 0.f ? scale_v : 1.f,
                        dds->c_str());
                    mtl += line;
                }
        }
        mtl += "\n";
        return name;
    }
};

/* --- section geometry --------------------------------------------------- */

struct section_streams
{
    blam::dimeter::geo::resource_t const* index{nullptr};
    blam::dimeter::geo::resource_t const* position{nullptr};
    blam::dimeter::geo::resource_t const* texcoord{nullptr};
    blam::dimeter::geo::resource_t const* submeshes{nullptr};
};

section_streams find_streams(
    semantic::Span<blam::dimeter::geo::resource_t const> resources)
{
    section_streams out;
    if(!resources.empty())
        out.submeshes = &resources[0];
    for(auto const& r : resources)
    {
        if(r.type0() == 32 && !out.index)
            out.index = &r;
        else if(r.type0() == 56 && r.type1() == 0 && !out.position)
            out.position = &r;
        else if(r.type0() == 56 && r.type1() == 1 && !out.texcoord)
            out.texcoord = &r;
    }
    return out;
}

/* Convert a strip or list index range to plain triangles, dropping
 * degenerates */
void emit_triangles(obj_writer& obj, Span<const u16> indices, bool strip)
{
    if(!strip)
    {
        for(size_t i = 0; i + 2 < indices.size(); i += 3)
            obj.triangle(indices[i], indices[i + 1], indices[i + 2]);
        return;
    }
    for(size_t i = 2; i < indices.size(); i++)
    {
        u16 a = indices[i - 2], b = indices[i - 1], c = indices[i];
        if(a == b || b == c || a == c)
            continue;
        if(i % 2)
            obj.triangle(b, a, c);
        else
            obj.triangle(a, b, c);
    }
}

/* Shared blob walk for both BSP (f32 verts) and model (i16-normalized
 * verts) sections; bounds != nullptr selects the model decode, instance
 * != nullptr places the section in world space */
template<typename Section, blam::dimeter::heap_t Heap>
bool append_section(
    obj_writer&                                   obj,
    blam::dimeter::raw_pool const&                pool,
    blam::dimeter::magic_ptr_base<Heap> const&    magic,
    Section const&                                section,
    std::string const&                        name,
    blam::dimeter::geo::model_bounds_t const* bounds,
    blam::dimeter::geo::bsp_instance_t const* instance  = nullptr,
    std::vector<std::string> const*           materials = nullptr)
{
    namespace geo = blam::dimeter::geo;
    using blam::from_le;

    u16 vertex_count = section.vertex_count();
    i32 data_size    = section.data_size();
    if(!vertex_count || data_size <= 0)
        return false;

    auto block = pool.resolve(section.data, static_cast<u32>(data_size));
    if(!block)
        return false;
    auto resources = section.resources.data(magic);
    if(resources.has_error())
        return false;

    auto streams = find_streams(resources.value());
    if(!streams.index || !streams.position)
        return false;

    i32 base        = section.base_address();
    u16 index_count = geo::block_index_count(*block);

    auto in_block = [&](i32 offset, i32 size) {
        return offset >= 0 && size >= 0 &&
               static_cast<i64>(base) + offset + size <=
                   static_cast<i64>(block->size());
    };
    if(!index_count || !in_block(streams.index->offset(), index_count * 2) ||
       !in_block(streams.position->offset(), streams.position->size()))
        return false;

    auto const* bytes = reinterpret_cast<u8 const*>(block->data());

    obj.group(name);

    /* Instance transform: 4x3 row-major, rows 0-2 = scaled basis vectors,
     * row 3 = translation; uniform scale applies to the vertex first */
    f32 inst_scale = 1.f;
    f32 m[12]      = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    if(instance)
    {
        inst_scale = blam::from_le(instance->scale);
        for(int i = 0; i < 12; i++)
            m[i] = blam::from_le(instance->transform[i]);
    }
    auto place = [&](f32 x, f32 y, f32 z) {
        x *= inst_scale;
        y *= inst_scale;
        z *= inst_scale;
        obj.vertex(
            x * m[0] + y * m[3] + z * m[6] + m[9],
            x * m[1] + y * m[4] + z * m[7] + m[10],
            x * m[2] + y * m[5] + z * m[8] + m[11]);
    };

    u32  stride = streams.position->size() / vertex_count;
    bool has_uv =
        streams.texcoord &&
        in_block(streams.texcoord->offset(), streams.texcoord->size());
    for(u32 v = 0; v < vertex_count; v++)
    {
        auto const* p = bytes + base + streams.position->offset() + v * stride;
        if(bounds)
        {
            i16 q[3];
            std::memcpy(q, p, 6);
            auto expand = [](i16 value, f32 min, f32 max) {
                f32 n = (static_cast<f32>(value) + 32768.f) / 65535.f;
                return min + n * (max - min);
            };
            place(
                expand(
                    from_le(q[0]),
                    from_le(bounds->x_min),
                    from_le(bounds->x_max)),
                expand(
                    from_le(q[1]),
                    from_le(bounds->y_min),
                    from_le(bounds->y_max)),
                expand(
                    from_le(q[2]),
                    from_le(bounds->z_min),
                    from_le(bounds->z_max)));
        } else
        {
            f32 pos[3];
            std::memcpy(pos, p, 12);
            place(from_le(pos[0]), from_le(pos[1]), from_le(pos[2]));
        }
    }
    for(u32 v = 0; v < vertex_count; v++)
    {
        if(!has_uv)
        {
            obj.texcoord(0.f, 0.f);
            continue;
        }
        auto const* p =
            bytes + base + streams.texcoord->offset() + v * (bounds ? 4 : 8);
        if(bounds)
        {
            i16 q[2];
            std::memcpy(q, p, 4);
            auto expand = [](i16 value, f32 min, f32 max) {
                f32 n = (static_cast<f32>(value) + 32768.f) / 65535.f;
                return min + n * (max - min);
            };
            obj.texcoord(
                expand(
                    from_le(q[0]),
                    from_le(bounds->u_min),
                    from_le(bounds->u_max)),
                expand(
                    from_le(q[1]),
                    from_le(bounds->v_min),
                    from_le(bounds->v_max)));
        } else
        {
            f32 uv[2];
            std::memcpy(uv, p, 8);
            obj.texcoord(from_le(uv[0]), from_le(uv[1]));
        }
    }

    std::vector<u16> indices(index_count);
    std::memcpy(
        indices.data(),
        bytes + base + streams.index->offset(),
        index_count * 2);
    for(auto& idx : indices)
        idx = from_le(idx);
    auto index_span = Span<const u16>(indices.data(), indices.size());
    /* Strip vs list is a per-section property */
    bool strip = static_cast<u32>(section.face_count()) * 3 != index_count;

    /* Per-submesh draw ranges carry the shader index; emit one usemtl per
     * range when a material table is available */
    bool split = false;
    if(materials && streams.submeshes &&
       in_block(streams.submeshes->offset(), streams.submeshes->size()))
    {
        auto const* subs = reinterpret_cast<geo::submesh_t const*>(
            bytes + base + streams.submeshes->offset());
        size_t sub_count = streams.submeshes->size() / sizeof(geo::submesh_t);
        for(size_t s = 0; s < sub_count; s++)
        {
            u32 start  = subs[s].index_start();
            u32 len    = subs[s].index_length();
            i16 shader = subs[s].shader.value();
            if(start + len > index_count || !len)
                continue;
            if(shader >= 0 && static_cast<size_t>(shader) < materials->size() &&
               !(*materials)[shader].empty())
                obj.usemtl((*materials)[shader]);
            emit_triangles(obj, index_span.subspan(start, len), strip);
            split = true;
        }
    }
    if(!split)
        emit_triangles(obj, index_span, strip);

    obj.end_group(vertex_count);
    return true;
}

template<typename V>
u32 extract_bsp_meshes(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::raw_pool const&         pool,
    std::string const&                     outdir)
{
    namespace h2  = blam::dimeter;
    namespace geo = h2::geo;
    using blam::from_le;

    /* BSP tag meta lives in its own region with its own virtual base;
     * the region list comes from the scenario */
    auto const* scnr_tag = map.scenario();
    if(!scnr_tag)
        return 0;
    auto const* scnr = scnr_tag->template data<geo::scnr_bsps_t>(map.magic());
    if(!scnr)
        return 0;
    auto bsp_refs = scnr->structure_bsps.data(map.magic());
    if(bsp_refs.has_error())
        return 0;

    u32 extracted = 0;
    for(auto const& ref : bsp_refs.value())
    {
        auto        located   = h2::locate_bsp(map, ref);
        auto const* bsp       = located.body;
        auto        bsp_magic = located.magic;
        if(!bsp)
            continue;

        auto const* bsp_tag = map.tag_at(ref.bsp.tag_id);
        auto        name    = bsp_tag ? std::string(map.name_of(*bsp_tag))
                                      : "bsp_" + std::to_string(extracted);

        /* Materials: one MTL entry per shader table slot, extracting the
         * diffuse bitmaps alongside */
        material_db              db;
        std::vector<std::string> materials;
        auto                     shader_refs = bsp->shaders.data(bsp_magic);
        if(!shader_refs.has_error())
            for(auto const& sref : shader_refs.value())
                materials.push_back(
                    db.resolve(map, pool, outdir, sref.shader.tag_id));

        obj_writer obj;
        obj.mtllib(sanitize(name) + ".mtl");
        u32 sections = 0;

        auto clusters = bsp->clusters.data(bsp_magic);
        if(!clusters.has_error())
            for(size_t i = 0; i < clusters.value().size(); i++)
                sections += append_section(
                    obj,
                    pool,
                    bsp_magic,
                    clusters.value()[i],
                    "cluster_" + std::to_string(i),
                    nullptr,
                    nullptr,
                    &materials);

        /* Instanced geometry: place each instance of its section in world
         * space; sections never referenced still get dumped at identity */
        auto              geom      = bsp->sections.data(bsp_magic);
        auto              instances = bsp->instances.data(bsp_magic);
        std::vector<bool> instanced;
        if(!geom.has_error())
            instanced.assign(geom.value().size(), false);
        if(!geom.has_error() && !instances.has_error())
        {
            for(size_t i = 0; i < instances.value().size(); i++)
            {
                auto const& inst = instances.value()[i];
                auto        sidx = inst.section.value();
                if(sidx < 0 || static_cast<size_t>(sidx) >= geom.value().size())
                    continue;
                instanced[sidx] = true;
                auto iname      = std::string(map.string(inst.name));
                if(iname.empty())
                    iname = "instance_" + std::to_string(i);
                sections += append_section(
                    obj,
                    pool,
                    bsp_magic,
                    geom.value()[sidx],
                    "instance_" + sanitize(iname),
                    nullptr,
                    &inst,
                    &materials);
            }
        }
        if(!geom.has_error())
            for(size_t i = 0; i < geom.value().size(); i++)
                if(!instanced[i])
                    sections += append_section(
                        obj,
                        pool,
                        bsp_magic,
                        geom.value()[i],
                        "orphan_section_" + std::to_string(i),
                        nullptr,
                        nullptr,
                        &materials);

        auto path = outdir + "/" + sanitize(name) + ".obj";
        if(sections && obj.write(path))
        {
            std::ofstream mtl_out(
                outdir + "/" + sanitize(name) + ".mtl", std::ios::binary);
            mtl_out << db.mtl;
            cBasicPrint(
                "  sbsp {0} sections ({1} bytes) -> {2}",
                sections,
                obj.body.size(),
                path);
            extracted++;
        }
    }
    return extracted;
}

template<typename V>
u32 extract_model_meshes(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::raw_pool const&         pool,
    std::string const&                     outdir,
    u32                                    limit)
{
    namespace geo = blam::dimeter::geo;

    auto magic     = map.magic();
    u32  extracted = 0;
    for(auto const& tag : map.tags())
    {
        if(limit && extracted >= limit)
            break;
        if(!tag.valid() || !tag.matches("mode"))
            continue;
        auto const* model = tag.template data<geo::model_header_t>(magic);
        if(!model)
            continue;
        auto bounds = model->bounds.data(magic);
        if(bounds.has_error() || bounds.value().empty())
            continue;

        auto sections = model->sections.data(magic);
        if(sections.has_error() || sections.value().empty())
            continue;

        material_db              db;
        std::vector<std::string> materials;
        auto                     shader_refs = model->shaders.data(magic);
        if(!shader_refs.has_error())
            for(auto const& sref : shader_refs.value())
                materials.push_back(
                    db.resolve(map, pool, outdir, sref.shader.tag_id));

        auto       name = sanitize(map.name_of(tag));
        obj_writer obj;
        obj.mtllib(name + ".mtl");
        u32 written = 0;
        for(size_t i = 0; i < sections.value().size(); i++)
            written += append_section(
                obj,
                pool,
                magic,
                sections.value()[i],
                "section_" + std::to_string(i),
                &bounds.value()[0],
                nullptr,
                &materials);

        auto path = outdir + "/" + name + ".obj";
        if(written && obj.write(path))
        {
            std::ofstream mtl_out(
                outdir + "/" + name + ".mtl", std::ios::binary);
            mtl_out << db.mtl;
            cBasicPrint(
                "  mode {0} sections ({1} bytes) -> {2}",
                written,
                obj.body.size(),
                path);
            extracted++;
        }
    }
    return extracted;
}

/* --- info dump -------------------------------------------------------- */

template<typename V>
int print_map(blam::dimeter::map_container<V> const& map)
{
    auto const* header = map.header;

    cBasicPrint(
        "Map:      {0} ({1})", map.name(), cache_type_name(header->map_type));
    cBasicPrint("Scenario: {0}", map.scenario_name());
    cBasicPrint("Build:    {0}", map.build());
    cBasicPrint(
        "Meta:     offset={0}, index stream={1} bytes, tag data={2} bytes",
        blam::from_le(header->meta_offset),
        blam::from_le(header->tag_table_size),
        blam::from_le(header->tag_data_size));
    cBasicPrint(
        "Masks:    index=0x{0:x}, meta=0x{1:x}", map.index_mask, map.meta_mask);

    auto groups = map.groups();
    auto tags   = map.tags();
    cBasicPrint(
        "Index:    {0} tag groups, {1} tags, {2} string ids",
        groups.size(),
        tags.size(),
        blam::from_le(header->string_count));

    if(auto const* scenario = map.scenario())
        cBasicPrint(
            "scnr:     {0} ({1} bytes)",
            map.name_of(*scenario),
            scenario->size());
    if(auto const* globals = map.globals())
        cBasicPrint(
            "matg:     {0} ({1} bytes)",
            map.name_of(*globals),
            globals->size());

    std::map<std::string, std::pair<u32, u64>> class_counts;
    u32                                        null_tags = 0;
    for(auto const& tag : tags)
    {
        if(!tag.valid())
        {
            null_tags++;
            continue;
        }
        auto& [count, bytes] = class_counts[std::string(tag.tag_class.str())];
        count++;
        bytes += static_cast<u64>(tag.size());
    }

    cBasicPrint("Tag classes ({0} null/padding tags skipped):", null_tags);
    for(auto const& [name, stat] : class_counts)
        cBasicPrint(
            "  {0}: {1} tags, {2} bytes", name, stat.first, stat.second);

    cBasicPrint("String id samples:");
    u32 string_count = blam::from_le(header->string_count);
    for(u32 i : {1u, 2u, string_count / 2, string_count - 1})
        cBasicPrint(
            "  [{0}] = {1}", i, map.string(blam::dimeter::string_id{i}));

    return 0;
}

/* --- tag selection ----------------------------------------------------- */

struct options_t
{
    std::string class_filter;
    std::string name_filter;
    bool        list{false};
    bool        dump{false};
    bool        scenario{false};
    std::string collision_obj;
    std::string cluster_at;
    bool        scripts{false};
    std::string lightmap_dir;
    std::string extract_dir;
    u32         limit{10};
};

template<typename V>
bool tag_selected(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::tag_t const&            tag,
    options_t const&                       options,
    bool                                   needs_data = true)
{
    /* sbsp entries carry no pointer; their bodies live in BSP regions */
    if(needs_data && !tag.valid())
        return false;
    if(!options.class_filter.empty() &&
       tag.tag_class.str() != options.class_filter)
        return false;
    if(!options.name_filter.empty() &&
       map.name_of(tag).find(options.name_filter) == std::string_view::npos)
        return false;
    return true;
}

/* sbsp bodies live in their own regions, reached through the scenario */
template<typename V, typename Fn>
void for_each_bsp(blam::dimeter::map_container<V> const& map, Fn&& fn)
{
    namespace geo        = blam::dimeter::geo;
    auto const* scnr_tag = map.scenario();
    auto const* scnr =
        scnr_tag ? scnr_tag->template data<geo::scnr_bsps_t>(map.magic())
                 : nullptr;
    if(!scnr)
        return;
    auto refs = scnr->structure_bsps.data(map.magic());
    if(refs.has_error())
        return;
    for(auto const& ref : refs.value())
        if(auto located = blam::dimeter::locate_bsp(map, ref); located.body)
            fn(ref, *located.body, located.magic);
}

/* World + placed instance collision as one OBJ, surfaces on vertices at
 * infinity left out */
template<typename V>
bool write_collision_obj(
    blam::dimeter::map_container<V> const& map, std::string const& path)
{
    namespace h2  = blam::dimeter;
    namespace geo = blam::dimeter::geo;
    using blam::from_le;

    obj_writer obj;
    u32        groups = 0;
    auto       append = [&](h2::coll::bsp_t<h2::heap_t::bsp> const& bsp,
                      h2::bsp_magic const&                 magic,
                      f32 const*             m, /* 4x3 row-major or null */
                      f32                    scale,
                      std::string const&     name) {
        auto surfaces = bsp.surfaces.data(magic);
        auto edges    = bsp.edges.data(magic);
        auto vertices = bsp.vertices.data(magic);
        if(surfaces.has_error() || edges.has_error() || vertices.has_error() ||
           vertices.value().empty())
            return;
        obj.group(name);
        std::vector<bool> finite;
        for(auto const& v : vertices.value())
        {
            f32 x = from_le(v.point[0]), y = from_le(v.point[1]),
                z = from_le(v.point[2]);
            finite.push_back(
                std::isfinite(x) && std::isfinite(y) && std::isfinite(z));
            if(!finite.back())
                x = y = z = 0.f;
            if(m)
            {
                x *= scale;
                y *= scale;
                z *= scale;
                obj.vertex(
                    x * m[0] + y * m[3] + z * m[6] + m[9],
                    x * m[1] + y * m[4] + z * m[7] + m[10],
                    x * m[2] + y * m[5] + z * m[8] + m[11]);
            } else
                obj.vertex(x, y, z);
            obj.texcoord(0.f, 0.f);
        }
        std::vector<u32> polygon;
        for(u32 si = 0; si < surfaces.value().size(); si++)
        {
            if(surfaces.value()[si].has(h2::coll::surface_flags_t::invalid))
                continue;
            polygon.clear();
            bool usable = true;
            h2::coll::walk_surface(
                surfaces.value(), edges.value(), si, [&](i16 vertex) {
                    if(vertex >= 0 && static_cast<size_t>(vertex) < finite.size() &&
                       finite[vertex])
                        polygon.push_back(static_cast<u32>(vertex));
                    else
                        usable = false;
                });
            if(!usable)
                continue;
            for(size_t i = 2; i < polygon.size(); i++)
                obj.triangle(polygon[0], polygon[i - 1], polygon[i]);
        }
        obj.end_group(static_cast<u32>(vertices.value().size()));
        groups++;
    };

    for_each_bsp(
        map,
        [&](geo::structure_bsp_ref_t const& ref,
            geo::bsp_header_t const&        bsp,
            h2::bsp_magic const&            magic) {
            auto const* bsp_tag = map.tag_at(ref.bsp.tag_id);
            auto        name    = bsp_tag ? sanitize(map.name_of(*bsp_tag))
                                          : "bsp_" + std::to_string(groups);
            if(auto coll = bsp.collision.data(magic); !coll.has_error())
                for(auto const& c : coll.value())
                    append(c, magic, nullptr, 1.f, name + "_world");
            auto geom      = bsp.sections.data(magic);
            auto instances = bsp.instances.data(magic);
            if(geom.has_error() || instances.has_error())
                return;
            for(auto const& [i, inst] : stl_types::enumerate(instances.value()))
            {
                auto sidx = inst.section.value();
                if(sidx < 0 || static_cast<size_t>(sidx) >= geom.value().size())
                    continue;
                std::array<f32, 12> m;
                for(size_t k = 0; k < m.size(); k++)
                    m[k] = from_le(inst.transform[k]);
                auto iname = std::string(map.string(inst.name));
                if(iname.empty())
                    iname = "instance_" + std::to_string(i);
                append(
                    geom.value()[sidx].collision,
                    magic,
                    m.data(),
                    from_le(inst.scale),
                    name + "_" + sanitize(iname));
            }
        });
    if(!groups)
        return false;
    bool ok = obj.write(path);
    cBasicPrint("Collision: {0} groups -> {1}", groups, path);
    return ok;
}

/* Which collision leaf and cluster a point falls in, per BSP */
template<typename V>
void probe_cluster(blam::dimeter::map_container<V> const& map, Vecf3 point)
{
    using blam::from_le;
    for_each_bsp(map, [&](auto const& ref, auto const& bsp, auto magic) {
        auto const* tag = map.tag_at(ref.bsp.tag_id);
        auto        coll = bsp.collision.data(magic);
        auto        leaves = bsp.leaves.data(magic);
        if(coll.has_error() || coll.value().empty() || leaves.has_error())
            return;
        auto leaf =
            coll.value()[0].find_leaf(point.x, point.y, point.z, magic);
        i32 cluster = -1;
        if(leaf && *leaf < leaves.value().size())
            cluster = leaves.value()[*leaf].cluster.value();
        cBasicPrint(
            "{0}: ({1}, {2}, {3}) -> leaf {4} cluster {5}",
            tag ? map.name_of(*tag) : std::string_view{},
            point.x,
            point.y,
            point.z,
            leaf ? static_cast<i64>(*leaf) : -1,
            cluster);
        if(cluster < 0)
            return;
        auto clusters = bsp.clusters.data(magic);
        if(clusters.has_error() ||
           static_cast<size_t>(cluster) >= clusters.value().size())
            return;
        auto const& c = clusters.value()[cluster];
        cBasicPrint(
            "  bounds x[{0}, {1}] y[{2}, {3}] z[{4}, {5}]",
            from_le(c.x_min),
            from_le(c.x_max),
            from_le(c.y_min),
            from_le(c.y_max),
            from_le(c.z_min),
            from_le(c.z_max));
    });
}

/* Every cluster's lightmap as a PPM, named by BSP and cluster */
template<typename V>
void write_lightmaps(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::raw_pool const&         pool,
    std::string const&                     dir)
{
    namespace h2 = blam::dimeter;
    u32 bsp_idx = 0, written = 0;
    for_each_bsp(
        map,
        [&](h2::geo::structure_bsp_ref_t const& ref,
            h2::geo::bsp_header_t const&,
            h2::bsp_magic const& magic) {
            u32         bi   = bsp_idx++;
            auto const* ltmp = h2::locate_bsp(map, ref).lightmap;
            if(!ltmp)
                return;
            auto groups = ltmp->groups.data(magic);
            if(groups.has_error() || groups.value().empty())
                return;
            auto const& group    = groups.value()[0];
            auto        infos    = group.cluster_render_info.data(magic);
            auto        palettes = group.section_palette.data(magic);
            auto const* bitm_tag = map.tag_at(group.bitmap_group.tag_id);
            auto const* bitm =
                bitm_tag
                    ? bitm_tag->template data<h2::bitm::header_t>(map.magic())
                    : nullptr;
            if(infos.has_error() || palettes.has_error() || !bitm)
                return;
            auto images = bitm->images.data(map.magic());
            if(images.has_error())
                return;
            for(auto const& [ci, info] : stl_types::enumerate(infos.value()))
            {
                i16 ii = info.bitmap.value();
                i8  pi = info.palette.value();
                if(ii < 0 || static_cast<size_t>(ii) >= images.value().size() ||
                   pi < 0 || static_cast<size_t>(pi) >= palettes.value().size())
                    continue;
                auto image = h2::ltmp::decode(
                    pool, images.value()[ii], palettes.value()[pi]);
                if(!image)
                    continue;
                std::vector<char> ppm;
                auto header = fmt::format("P6\n{} {}\n255\n", image->width, image->height);
                ppm.insert(ppm.end(), header.begin(), header.end());
                for(size_t p = 0; p < image->rgba.size(); p += 4)
                    ppm.insert(
                        ppm.end(),
                        {static_cast<char>(image->rgba[p]),
                         static_cast<char>(image->rgba[p + 1]),
                         static_cast<char>(image->rgba[p + 2])});
                if(write_file(
                       fmt::format("{}/bsp{}_cluster{:03}_img{}_pal{}.ppm", dir, bi, ci, ii, pi),
                       ppm))
                    written++;
            }
        });
    cBasicPrint("Lightmaps: {0} written to {1}", written, dir);
}

/* --- entry point ------------------------------------------------------ */

template<typename V>
int run_map(
    blam::dimeter::map_container<V> const& map_in,
    BytesConst const&                      map_data,
    std::string const&                     map_dir,
    options_t const&                       options)
{
    namespace h2 = blam::dimeter;
    using namespace platform::url::constructors;

    h2::raw_pool pool;
    pool.attach(
        h2::raw_file_t::local, Span<const byte_t>(map_data.data, map_data.size));
    std::vector<std::unique_ptr<Resource>>           side_files;
    constexpr std::pair<h2::raw_file_t, const char*> side_names[3] = {
        {h2::raw_file_t::mainmenu, "mainmenu.map"},
        {h2::raw_file_t::shared, "shared.map"},
        {h2::raw_file_t::single_player_shared, "single_player_shared.map"},
    };
    for(auto const& [slot, name] : side_names)
    {
        auto res   = std::make_unique<Resource>(MkSysUrl(map_dir + "/" + name));
        auto bytes = C_OCAST<BytesConst>(*res);
        if(bytes.data && bytes.size)
        {
            pool.attach(slot, Span<const byte_t>(bytes.data, bytes.size));
            side_files.push_back(std::move(res));
            cBasicPrint("Raw pool: attached {0}", name);
        }
    }
    /* Vista borrows weapons, templates and the like from shared.map */
    auto map        = map_in;
    auto map_shared = h2::load_shared(map, pool);
    if(map_shared)
    {
        map.shared = &*map_shared;
        cBasicPrint("Shared tags: attached, {0} tags", map_shared->tags().size());
    }

    bool const selecting = options.list || options.dump;
    if(!selecting && !options.scenario)
        print_map(map);

    if(options.list)
        for(auto const& [i, tag] : stl_types::enumerate(map.tags()))
            if(tag_selected(map, tag, options))
                fmt::print(
                    "{:5} [{}] {} ({} bytes)\n",
                    i,
                    tag.tag_class.str(),
                    map.name_of(tag),
                    tag.size());

    if(options.dump)
    {
        for(auto const& tag : map.tags())
            if(tag_selected(map, tag, options))
                dimeter_info::dump_any(map, tag);
        for_each_bsp(
            map,
            [&](h2::geo::structure_bsp_ref_t const& ref,
                h2::geo::bsp_header_t const&        bsp,
                h2::bsp_magic const&                magic) {
                auto const* bsp_tag = map.tag_at(ref.bsp.tag_id);
                if(!bsp_tag || !tag_selected(map, *bsp_tag, options, false))
                    return;
                fmt::print(
                    "{} [sbsp] region {:#x}\n",
                    map.name_of(*bsp_tag),
                    ref.meta_address());
                dimeter_info::dump_sbsp(map, bsp, magic);
                dimeter_info::dump_sbsp_extras(map, bsp, magic);
                if(auto const* ltmp = h2::locate_bsp(map, ref).lightmap)
                {
                    fmt::print("{} [ltmp]\n", map.name_of(*bsp_tag));
                    dimeter_info::dump_ltmp(map, *ltmp, magic);
                }
            });
    }

    if(options.scenario)
        if(auto const* scnr = map.scenario())
            dimeter_info::dump_any(map, *scnr);
    if(options.scripts)
        if(auto const* scnr_tag = map.scenario())
            if(auto const* scnr = scnr_tag->template data<h2::scn::scenario_t>(
                   map.magic()))
                dimeter_info::dump_scripts(map, *scnr, true);

    if(!options.collision_obj.empty())
        write_collision_obj(map, options.collision_obj);
    if(!options.cluster_at.empty())
    {
        Vecf3 point{};
        if(std::sscanf(
               options.cluster_at.c_str(),
               "%f,%f,%f",
               &point.x,
               &point.y,
               &point.z) == 3)
            probe_cluster(map, point);
    }

    if(options.extract_dir.empty() && options.lightmap_dir.empty())
        return 0;

    /* Raw data may live in the global cache files next to the map */

    std::error_code mkdir_ec;
    if(!options.lightmap_dir.empty())
    {
        std::filesystem::create_directories(options.lightmap_dir, mkdir_ec);
        write_lightmaps(map, pool, options.lightmap_dir);
    }
    if(options.extract_dir.empty())
        return 0;
    std::filesystem::create_directories(options.extract_dir, mkdir_ec);

    cBasicPrint("Extracting bitmaps:");
    u32 bitmaps = extract_bitmaps(map, pool, options.extract_dir, options.limit);
    cBasicPrint("Extracting sounds:");
    u32 sounds = extract_sounds(map, pool, options.extract_dir, options.limit);
    cBasicPrint("Extracting BSP meshes:");
    u32 bsps = extract_bsp_meshes(map, pool, options.extract_dir);
    cBasicPrint("Extracting model meshes:");
    u32 models =
        extract_model_meshes(map, pool, options.extract_dir, options.limit);
    cBasicPrint(
        "Extracted {0} bitmaps, {1} sounds, {2} BSPs, {3} models",
        bitmaps,
        sounds,
        bsps,
        models);
    return 0;
}

/* Header, metrics and a few glyphs as ASCII art */
i32 dump_font(std::string const& path)
{
    using namespace platform::url::constructors;
    Resource file(MkSysUrl(path));
    auto     bytes = C_OCAST<BytesConst>(file);
    auto     font  = blam::dimeter::font::font_t::from_bytes(
        Span<const byte_t>(bytes.data, bytes.size));
    if(!font)
    {
        cBasicPrint("{0}: not a Halo 2 font", path);
        return 1;
    }
    fmt::print(
        "{}: {} glyphs, ascent {} descent {} leading {}, {} kerning pairs, "
        "max compressed {} bytes\n",
        path,
        font->glyphs.size(),
        font->ascent(),
        font->descent(),
        blam::from_le(font->header->leading),
        font->kerning.size(),
        blam::from_le(font->header->max_compressed_size));
    for(char32_t c : std::u32string(U"HA2g!"))
    {
        auto const* g = font->glyph(c);
        if(!g)
        {
            fmt::print("'{}': no glyph\n", static_cast<char>(c));
            continue;
        }
        blam::dimeter::font::image_t image;
        bool ok = font->decode(*g, image);
        fmt::print(
            "'{}': advance {} size {}x{} origin ({}, {}) {} bytes{}\n",
            static_cast<char>(c),
            blam::from_le(g->advance),
            blam::from_le(g->width),
            blam::from_le(g->height),
            blam::from_le(g->origin_x),
            blam::from_le(g->origin_y),
            blam::from_le(g->compressed_size),
            ok ? "" : " (decode failed)");
        static constexpr const char* shades = " .:-=+*#%@";
        for(u32 y = 0; y < image.height; y++)
        {
            std::string row;
            for(u32 x = 0; x < image.width; x++)
            {
                u8 a = image.rgba[(y * image.width + x) * 4 + 3];
                row.push_back(shades[a * 9 / 255]);
            }
            fmt::print("  |{}|\n", row);
        }
    }
    for(auto const& k : font->kerning)
        if(k.first == 'A' || k.first == 'T')
            fmt::print("  kern '{}{}' {}\n", static_cast<char>(k.first), static_cast<char>(k.second), blam::from_le(k.adjust));
    return 0;
}

} // namespace

i32 coffee_main(i32, cstring_w*)
{
    using namespace platform::url::constructors;
    using blam::dimeter::map_container;

    cxxopts::Options opts(
        "Halo 2 Map Info", "Inspect and extract from a Halo 2 cache file");
    opts.add_options("Help")("h,help", "Show this help info");
    opts.positional_help("<map file>");
    opts.add_options("Tag selection")
        //
        ("c,class", "Only tags of this class, e.g. shad", cxxopts::value<std::string>())
        //
        ("n,name", "Only tags whose name contains this", cxxopts::value<std::string>())
        //
        ("l,list", "List matching tags")
        //
        ("d,dump",
         "Dump decoded data of matching tags (bitm, shad, stem, mode, scnr, "
         "sbsp, sky, hlmt, objects)")
        //
        ("scenario", "Dump the scenario: skies, BSPs, scenery, spawns")
        //
        ("collision-obj",
         "Write the collision BSPs (world + instances) as an OBJ",
         cxxopts::value<std::string>())
        //
        ("dump-lightmaps",
         "Write every cluster's lightmap as a PPM into this directory",
         cxxopts::value<std::string>())
        //
        ("cluster-at",
         "Which cluster holds the point x,y,z",
         cxxopts::value<std::string>())
        //
        ("scripts", "Decompile the scenario's scripts and globals")
        //
        ("font",
         "Dump a Halo 2 font file (fonts/<name>) instead of a map",
         cxxopts::value<std::string>())
        //
        ;
    opts.add_options("Extraction")
        //
        ("e,extract",
         "Extract bitmaps (DDS), sounds (WAV/WMA) and meshes (OBJ) here",
         cxxopts::value<std::string>())
        //
        ("limit",
         "Per-category cap on extracted assets, 0 = everything",
         cxxopts::value<int>()->default_value("10"))
        //
        ;

    auto& args      = Coffee::GetInitArgs();
    auto  arguments = opts.parse(args.size(), args.data());
    if(Coffee::BaseArgParser::PerformDefaults(opts, args) >= 0)
        return 0;

    if(auto font = arguments.as_optional<std::string>("font"))
        return dump_font(*font);

    /* The launcher may put a resource directory first; the map is the
     * first positional that is a file */
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
        .dump         = arguments.count("dump") > 0,
        .scenario     = arguments.count("scenario") > 0,
        .collision_obj =
            arguments.as_optional<std::string>("collision-obj").value_or(""),
        .cluster_at =
            arguments.as_optional<std::string>("cluster-at").value_or(""),
        .scripts = arguments.count("scripts") > 0,
        .lightmap_dir =
            arguments.as_optional<std::string>("dump-lightmaps").value_or(""),
        .extract_dir = arguments.as_optional<std::string>("extract").value_or(""),
        .limit       = static_cast<u32>(arguments["limit"].as<int>()),
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
    if(auto map = map_container<blam::dimeter::xbox_version_t>::from_bytes(
           map_data, blam::dimeter::xbox_version);
       map.has_value())
    {
        cBasicPrint("-- Halo 2 Xbox cache file --");
        rc = run_map(map.value(), map_data, map_dir, options);
    } else if(auto map =
                  map_container<blam::dimeter::vista_version_t>::from_bytes(
                      map_data, blam::dimeter::vista_version);
              map.has_value())
    {
        cBasicPrint("-- Halo 2 Vista cache file --");
        rc = run_map(map.value(), map_data, map_dir, options);
    } else
        cBasicPrint(
            "not a (supported) Halo 2 map: error {0}",
            static_cast<int>(map.error()));

    /* quick_exit skips stdio teardown, which would drop a short dump */
    fflush(stdout);
    std::quick_exit(rc);
}

COFFEE_APPLICATION_MAIN_CUSTOM(coffee_main, 0x1 | 0x2)
