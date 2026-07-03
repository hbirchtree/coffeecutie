#include <coffee/core/CApplication>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>

#include <blam/dimeter/h2_bitm.h>
#include <blam/dimeter/h2_map.h>
#include <blam/dimeter/h2_sound.h>
#include <blam/volta/blam_swizzle.h>

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
            .bpp      = 16,
            .pf_flags = LUMA,
            .mask_r   = 0x00FF,
            .mask_a   = 0xFF00};
    case format_t::Y8:
    case format_t::AY8:
        return dds_info{.bpp = 8, .pf_flags = LUM, .mask_r = 0xFF};
    case format_t::A8:
        return dds_info{.bpp = 8, .pf_flags = A, .mask_a = 0xFF};
    case format_t::V8U8:
        return dds_info{
            .bpp      = 16,
            .pf_flags = BUMP,
            .mask_r   = 0x00FF,
            .mask_g   = 0xFF00};
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
    append<u32>(out, 0x1007);     /* CAPS|HEIGHT|WIDTH|PIXELFORMAT */
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
    std::string const&                   outdir,
    u32                                  limit)
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

                auto path = outdir + "/" + sanitize(map.tag_name(tag)) +
                            (image_span.size() > 1
                                 ? "_img" + std::to_string(img_idx)
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
    constexpr u16 block_align_per_channel = 36;
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
    append<u16>(out, 4);  /* bits per sample */
    append<u16>(out, 2);  /* cbSize */
    append<u16>(out, 64); /* samples per block */
    append<u32>(out, 0x61746164); /* data */
    append<u32>(out, static_cast<u32>(payload.size()));
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

template<typename V>
u32 extract_sounds(
    blam::dimeter::map_container<V> const& map,
    blam::dimeter::raw_pool const&         pool,
    std::string const&                   outdir,
    u32                                  limit)
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
        u16 channels = sound->encoding == h2::snd::encoding_t::stereo ? 2 : 1;
        if(adpcm && sound->encoding == h2::snd::encoding_t::codec)
            continue;

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

            auto path =
                outdir + "/" + sanitize(map.tag_name(tag)) +
                (perm_count > 1 ? "_perm" + std::to_string(p) : "") +
                (adpcm ? ".wav" : ".wma");
            auto rate = h2::snd::to_hertz(sound->sample_rate);
            if(write_file(
                   path,
                   adpcm ? make_xadpcm_wav(rate, channels, payload)
                         : payload))
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
            map.tag_name(*scenario),
            scenario->size());
    if(auto const* globals = map.globals())
        cBasicPrint(
            "matg:     {0} ({1} bytes)",
            map.tag_name(*globals),
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
        auto& [count, bytes] = class_counts[tag.tag_class.str()];
        count++;
        bytes += static_cast<u64>(tag.size());
    }

    cBasicPrint("Tag classes ({0} null/padding tags skipped):", null_tags);
    for(auto const& [name, stat] : class_counts)
        cBasicPrint("  {0}: {1} tags, {2} bytes", name, stat.first, stat.second);

    cBasicPrint("String id samples:");
    u32 string_count = blam::from_le(header->string_count);
    for(u32 i : {1u, 2u, string_count / 2, string_count - 1})
        cBasicPrint("  [{0}] = {1}", i, map.string(blam::dimeter::string_id{i}));

    return 0;
}

/* --- entry point ------------------------------------------------------ */

template<typename V>
int run_map(
    blam::dimeter::map_container<V> const& map,
    BytesConst const&                    map_data,
    std::string const&                   map_dir,
    std::string const&                   extract_dir,
    u32                                  limit)
{
    print_map(map);
    if(extract_dir.empty())
        return 0;

    namespace h2 = blam::dimeter;
    using namespace platform::url::constructors;

    /* Raw data may live in the global cache files next to the map */
    h2::raw_pool pool;
    pool.files[static_cast<u8>(h2::raw_file_t::local)] =
        Span<const byte_t>(map_data.data, map_data.size);
    std::vector<std::unique_ptr<Resource>> side_files;
    constexpr std::pair<h2::raw_file_t, const char*> side_names[3] = {
        {h2::raw_file_t::mainmenu, "mainmenu.map"},
        {h2::raw_file_t::shared, "shared.map"},
        {h2::raw_file_t::single_player_shared, "single_player_shared.map"},
    };
    for(auto const& [slot, name] : side_names)
    {
        auto res = std::make_unique<Resource>(MkSysUrl(map_dir + "/" + name));
        auto bytes = C_OCAST<BytesConst>(*res);
        if(bytes.data && bytes.size)
        {
            pool.files[static_cast<u8>(slot)] =
                Span<const byte_t>(bytes.data, bytes.size);
            side_files.push_back(std::move(res));
            cBasicPrint("Raw pool: attached {0}", name);
        }
    }

    std::error_code mkdir_ec;
    std::filesystem::create_directories(extract_dir, mkdir_ec);

    cBasicPrint("Extracting bitmaps:");
    u32 bitmaps = extract_bitmaps(map, pool, extract_dir, limit);
    cBasicPrint("Extracting sounds:");
    u32 sounds = extract_sounds(map, pool, extract_dir, limit);
    cBasicPrint("Extracted {0} bitmaps, {1} sounds", bitmaps, sounds);
    return 0;
}

} // namespace

i32 coffee_main(i32 argc, cstring_w* argv)
{
    using namespace platform::url::constructors;
    using blam::dimeter::map_container;

    if(argc < 2)
    {
        cBasicPrint(
            "usage: {0} <map file> [extract dir] [limit]\n"
            "  limit: per-category cap on extracted assets, 0 = everything "
            "(default 10)",
            argv[0]);
        return 1;
    }

    std::string extract_dir = argc > 2 ? argv[2] : "";
    u32         limit = argc > 3 ? static_cast<u32>(std::atoi(argv[3])) : 10;

    std::string map_path(argv[1]);
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

    if(auto map = map_container<blam::dimeter::xbox_version_t>::from_bytes(
           map_data, blam::dimeter::xbox_version);
       map.has_value())
    {
        cBasicPrint("-- Halo 2 Xbox cache file --");
        return run_map(map.value(), map_data, map_dir, extract_dir, limit);
    }

    if(auto map = map_container<blam::dimeter::vista_version_t>::from_bytes(
           map_data, blam::dimeter::vista_version);
       map.has_value())
    {
        cBasicPrint("-- Halo 2 Vista cache file --");
        return run_map(map.value(), map_data, map_dir, extract_dir, limit);
    } else
    {
        cBasicPrint(
            "not a (supported) Halo 2 map: error {0}",
            static_cast<int>(map.error()));
        return 1;
    }
}

COFFEE_APPLICATION_MAIN(coffee_main)
