#include <blam/dimeter/h2_file_header.h>
#include <blam/dimeter/h2_tag_index.h>
#include <blam/transcode/map_transcode.h>
#include <blam/volta/blam_atlas.h>
#include <blam/volta/blam_file_header.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_versions.h>

#include <emscripten/emscripten.h>
#include <magic_enum/magic_enum.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#if !defined(BLAM_HAS_COMPRESSION)
#error "BlamMapUpload needs CoreZ to inflate Xbox maps"
#endif

/* The JS half of this tool lives in shell.html. It reads the first
 * header_size bytes of each dropped file, asks blam_upload_identify() what it
 * is, and only hands the whole file over to blam_upload_decompress() when it
 * is a compressed map. Everything else goes into IndexedDB untouched. For
 * Halo 2 maps it also reads the 32-byte tag index the header points to and
 * passes it to blam_upload_identify_halo2().
 *
 * Halo PC, Custom Edition and Xbox maps can have their bitmaps transcoded on
 * the way in (blam_upload_transcode_*), for GPUs that cannot sample BCn, and
 * Xbox maps always get their swizzled textures made linear. A PC level map's
 * images live in bitmaps.map, so the original bitmaps.map is held here while
 * every level map dropped with it is transcoded. A map takes minutes to
 * encode, so that runs on its own thread and the page polls it. */

/* JS refers to the heap as it was when the page last looked. Memory grown
 * from the worker leaves those views on the old, shorter buffer, and touching
 * HEAPU8 from library code is what renews them. */
EM_JS(void, refresh_heap_views, (), { HEAPU8.length; });

namespace {

using blam::file_header_t;
using libc_types::i32;
using libc_types::u32;
using libc_types::u64;

/* Keep in sync with KINDS in shell.html */
enum class kind_t : i32
{
    unknown = 0,
    pc      = 1,
    custom  = 2,
    xbox    = 3,
    mcc     = 4,
    trial   = 5,
    bitmaps = 6,
    sounds  = 7,
    loc     = 8,
    /* Halo 2 header found, layout not known until its tag index is read */
    halo2       = 9,
    halo2_xbox  = 10,
    halo2_vista = 11,
};

/* Read field by field from JS, layout is fixed */
struct upload_info_t
{
    i32  kind;
    i32  compressed;
    u32  decomp_len;
    char name[36];
    u32  index_offset; /*!< Halo 2: where blam_upload_identify_halo2() reads */
    i32  cache_type;   /*!< Halo 2: blam::dimeter::cache_type_t, or -1 */
};

static_assert(sizeof(upload_info_t) == 56);

upload_info_t info{};
/* Halo 2 header kept between the two identify calls */
std::array<char, sizeof(blam::dimeter::file_header_xbox_t)> halo2_header;
std::vector<char>                                           result;
std::string                                                 last_error;

struct transcode_session_t
{
    std::optional<mtx::kernel_fn> kernel;
    bool                          deswizzle{true};
    /* Original bitmaps.map, malloc()ed by the page and owned from here */
    libc_types::u8*    bitmaps{nullptr};
    u32                bitmaps_size{0};
    std::vector<char>  out_bitmaps;
    mtx::shared_cache  cache;

    /* The map being transcoded, malloc()ed by the page */
    char*              map{nullptr};
    u32                map_size{0};
    std::thread        worker;
    std::atomic<bool>  finished{false};
    bool               ok{false};
    std::string        error;
    mtx::map_stats     stats;
    /* Images done and in total, read from JS while the worker runs */
    std::atomic<u32>   progress[2]{};
};

/* Read field by field from JS */
static_assert(sizeof(mtx::map_stats) == 24);
static_assert(sizeof(std::atomic<u32>[2]) == 8);

transcode_session_t transcode;

kind_t kind_of(blam::version_t version)
{
    switch(version)
    {
    case blam::version_t::pc:
        return kind_t::pc;
    case blam::version_t::custom_edition:
        return kind_t::custom;
    case blam::version_t::xbox:
        return kind_t::xbox;
    case blam::version_t::mcc:
        return kind_t::mcc;
    default:
        return kind_t::unknown;
    }
}

/* A regular map starts with "head" and has "foot" at the end of its 2 KiB
 * header, with the game version right after "head" */
file_header_t const* map_header(semantic::BytesConst const& data)
{
    if(data.size < sizeof(file_header_t))
        return nullptr;
    if(blam::file_header_t::from_data(data, blam::pc_version).has_value() ||
       blam::file_header_t::from_data(data, blam::custom_version).has_value() ||
       blam::file_header_t::from_data(data, blam::xbox_version).has_value() ||
       blam::file_header_t::from_data(data, blam::mcc_version).has_value())
        return reinterpret_cast<file_header_t const*>(data.data);
    return nullptr;
}

/* bitmaps.map, sounds.map and loc.map have no "head"/"foot" header. They
 * are resource maps (blam::tag_atlas_t): a u32 type (1 bitmaps, 2 sounds,
 * 3 localization), the offset of the path strings, then the offset and count
 * of 12-byte locators. A map header's first u32 is "head", so it can never
 * read as 1-3. The offsets have to land inside the file. */
kind_t atlas_kind(semantic::BytesConst const& data, u32 file_size)
{
    if(data.size < sizeof(blam::tag_atlas_t))
        return kind_t::unknown;
    blam::tag_atlas_t atlas;
    std::memcpy(&atlas, data.data, sizeof(atlas));

    /* u64, so a bogus offset or count cannot wrap past the file size */
    u64 const header_end = sizeof(blam::tag_atlas_t);
    u64 const locators_end =
        u64{atlas.locators_offset} +
        u64{atlas.locators_count} * sizeof(blam::locator_block);
    if(atlas.name_block_off < header_end || atlas.name_block_off >= file_size ||
       atlas.locators_offset < header_end || locators_end > file_size)
        return kind_t::unknown;

    switch(atlas.type)
    {
    case blam::atlas_type_t::bitmaps:
        return kind_t::bitmaps;
    case blam::atlas_type_t::sounds:
        return kind_t::sounds;
    case blam::atlas_type_t::localization:
        return kind_t::loc;
    default:
        return kind_t::unknown;
    }
}

template<typename Header>
void copy_name(Header const& header)
{
    auto const& name = header.name.data;
    std::memcpy(
        info.name,
        name.data(),
        std::min(sizeof(info.name) - 1, ::strnlen(name.data(), name.size())));
}

bool is_global_cache_file(blam::dimeter::cache_type_t type)
{
    using blam::dimeter::cache_type_t;
    return type == cache_type_t::mainmenu || type == cache_type_t::shared ||
           type == cache_type_t::single_player_shared;
}

/* Halo 2 maps load raw data from mainmenu.map, shared.map and
 * single_player_shared.map next to them, by name. Xbox and Vista each have
 * their own, and shared.map in particular may carry little tag data to tell
 * the layouts apart by. When the layout is unknown, map_type is still worth
 * reporting, so the page can file them with the other Halo 2 maps it was
 * given. It sits at 0x140 on Xbox and 0x14C on Vista; only a value that
 * reads as one of these files at exactly one of the two is trusted. */
i32 unresolved_cache_type()
{
    using namespace blam::dimeter;
    auto const* xbox =
        reinterpret_cast<file_header_xbox_t const*>(halo2_header.data());
    auto const* vista =
        reinterpret_cast<file_header_vista_t const*>(halo2_header.data());
    auto const as_xbox  = blam::from_le(xbox->map_type);
    auto const as_vista = blam::from_le(vista->map_type);
    bool const xbox_ok  = is_global_cache_file(as_xbox);
    bool const vista_ok = is_global_cache_file(as_vista);
    if(xbox_ok == vista_ok)
        return -1;
    return static_cast<i32>(xbox_ok ? as_xbox : as_vista);
}

/* Halo 2 maps have the same "head"/"foot" header, with version 8. Xbox and
 * Vista lay the rest of it out differently, which only shows in the tag
 * index at meta_offset, so that is read separately. */
bool halo2_header_of(semantic::BytesConst const& data, u32 file_size)
{
    using blam::dimeter::file_header_xbox_t;
    using blam::dimeter::tag_index_t;
    if(data.size < sizeof(file_header_xbox_t))
        return false;
    auto const* header = reinterpret_cast<file_header_xbox_t const*>(data.data);
    if(!header->valid())
        return false;

    std::memcpy(halo2_header.data(), data.data, halo2_header.size());
    info.kind       = static_cast<i32>(kind_t::halo2);
    info.cache_type = unresolved_cache_type();
    /* meta_offset sits at the same place in both layouts */
    auto meta_offset = blam::from_le(header->meta_offset);
    if(u64{meta_offset} + sizeof(tag_index_t) <= file_size)
        info.index_offset = meta_offset;
    return true;
}

template<typename Ver>
bool inflate(semantic::BytesConst const& data)
{
    auto map = blam::map_container<Ver>::from_bytes(data, Ver());
    if(map.has_error())
    {
        last_error = magic_enum::enum_name(map.error());
        return false;
    }
    if(map.value().decompressed.empty())
    {
        last_error = "map is not compressed";
        return false;
    }
    /* The original header is kept, and decomp_len then matches the output
     * size, which is what loaders take as uncompressed */
    result = std::move(map.value().decompressed);
    return true;
}

} // namespace

extern "C" {

/*!
 * \brief Identify a file from its first bytes (at least 2048 for maps)
 * \param file_size Size of the whole file, which may be more than size.
 * Maps address everything with u32 offsets, so the page rejects files of
 * 4 GiB or more before calling this.
 * \return Pointer to an upload_info_t, valid until the next call
 */
EMSCRIPTEN_KEEPALIVE upload_info_t const* blam_upload_identify(
    char const* data, u32 size, u32 file_size)
{
    info            = {};
    info.cache_type = -1;
    auto head       = semantic::BytesConst::of(
        reinterpret_cast<libc_types::u8 const*>(data), size);

    if(auto header = map_header(head))
    {
        info.kind = static_cast<i32>(kind_of(blam::from_le(header->version)));
        info.decomp_len = blam::from_le(header->decomp_len);
        info.compressed = info.decomp_len != file_size;
        copy_name(*header);
        return &info;
    }
    if(halo2_header_of(head, file_size))
        return &info;

    if(size >= sizeof(blam::file_header_trial_t) &&
       reinterpret_cast<blam::file_header_trial_t const*>(data)->valid())
        info.kind = static_cast<i32>(kind_t::trial);
    else
        info.kind = static_cast<i32>(atlas_kind(head, file_size));
    return &info;
}

/*!
 * \brief Finish identifying a Halo 2 map from the 32-byte tag index read at
 * info.index_offset. Same rule as blam::dimeter::map_container: the group
 * table pointer is a virtual address on Xbox, and a small offset relative to
 * the index on Vista. The kind stays halo2 when no tag index is found there,
 * and cache_type then keeps what unresolved_cache_type() made of it.
 */
EMSCRIPTEN_KEEPALIVE upload_info_t const* blam_upload_identify_halo2(
    char const* data, u32 size)
{
    using namespace blam::dimeter;
    if(info.kind != static_cast<i32>(kind_t::halo2) ||
       size < sizeof(tag_index_t))
        return &info;

    tag_index_t index;
    std::memcpy(&index, data, sizeof(index));
    if(!index.valid())
        return &info;

    auto const* xbox =
        reinterpret_cast<file_header_xbox_t const*>(halo2_header.data());
    if(blam::from_le(index.group_table_pointer) <
       blam::from_le(xbox->meta_size))
    {
        auto const* vista =
            reinterpret_cast<file_header_vista_t const*>(halo2_header.data());
        info.kind       = static_cast<i32>(kind_t::halo2_vista);
        info.cache_type = static_cast<i32>(blam::from_le(vista->map_type));
        copy_name(*vista);
    } else
    {
        info.kind       = static_cast<i32>(kind_t::halo2_xbox);
        info.cache_type = static_cast<i32>(blam::from_le(xbox->map_type));
        copy_name(*xbox);
    }
    return &info;
}

/*!
 * \brief Inflate a whole compressed map. On success, the output is available
 * from blam_upload_result_data()/size() until blam_upload_result_free().
 */
EMSCRIPTEN_KEEPALIVE i32 blam_upload_decompress(char const* data, u32 size)
{
    result.clear();
    last_error.clear();

    auto bytes = semantic::BytesConst::of(
        reinterpret_cast<libc_types::u8 const*>(data), size);
    auto header = map_header(bytes);
    if(!header)
    {
        last_error = "not a map";
        return 0;
    }
    if(header->trailing_space > size - sizeof(file_header_t))
    {
        last_error = "corrupt header (trailing space past end of file)";
        return 0;
    }

    try
    {
        switch(blam::from_le(header->version))
        {
        case blam::version_t::xbox:
            return inflate<blam::xbox_version_t>(bytes);
        case blam::version_t::pc:
            return inflate<blam::pc_version_t>(bytes);
        case blam::version_t::custom_edition:
            return inflate<blam::custom_version_t>(bytes);
        case blam::version_t::mcc:
            return inflate<blam::mcc_version_t>(bytes);
        default:
            last_error = "unsupported map version";
            return 0;
        }
    } catch(std::exception const& e)
    {
        last_error = e.what();
        result     = {};
        return 0;
    }
}

EMSCRIPTEN_KEEPALIVE char const* blam_upload_result_data()
{
    return result.data();
}

EMSCRIPTEN_KEEPALIVE u32 blam_upload_result_size()
{
    return static_cast<u32>(result.size());
}

EMSCRIPTEN_KEEPALIVE void blam_upload_result_free()
{
    result = {};
}

/*!
 * \brief Pick the kernel for blam_upload_transcode_map(): ES3 (ETC2),
 * ES2, PowerVR or Gekko, see mtx::make_kernel(), or "" for none. With
 * deswizzle, Xbox textures stored swizzled are made linear.
 */
EMSCRIPTEN_KEEPALIVE i32 blam_upload_transcode_target(
    char const* target, i32 deswizzle)
{
    last_error.clear();
    transcode.deswizzle = deswizzle != 0;
    if(!*target)
    {
        transcode.kernel = mtx::kernel_fn{};
        return 1;
    }
    transcode.kernel = mtx::make_kernel(target);
    if(!transcode.kernel)
        last_error = "unknown transcode target";
    return transcode.kernel.has_value();
}

/*!
 * \brief Hold on to the original bitmaps.map for the level maps transcoded
 * after this. Takes ownership of data, which must come from malloc().
 */
EMSCRIPTEN_KEEPALIVE void blam_upload_transcode_begin(char* data, u32 size)
{
    std::free(transcode.bitmaps);
    transcode.bitmaps      = reinterpret_cast<libc_types::u8*>(data);
    transcode.bitmaps_size = size;
    transcode.out_bitmaps.assign(
        transcode.bitmaps, transcode.bitmaps + transcode.bitmaps_size);
    transcode.cache = {};
}

/*!
 * \brief Start transcoding one uncompressed Halo PC, Custom Edition or Xbox map,
 * which takes ownership of data (from malloc()). Poll it with
 * blam_upload_transcode_poll(). Without blam_upload_transcode_begin(), only
 * images stored in the map itself are transcoded.
 */
EMSCRIPTEN_KEEPALIVE i32 blam_upload_transcode_map(char* data, u32 size)
{
    result.clear();
    last_error.clear();
    if(!transcode.kernel || transcode.worker.joinable())
    {
        last_error = transcode.kernel ? "already transcoding a map"
                                      : "no transcode target";
        std::free(data);
        return 0;
    }

    /* Allocated here, so the worker only fills it in */
    result.assign(data, data + size);
    transcode.map      = data;
    transcode.map_size = size;
    transcode.finished = false;
    transcode.progress[0] = 0;
    transcode.progress[1] = 0;
    transcode.worker      = std::thread([] {
        auto& t = transcode;
        try
        {
            auto res = mtx::transcode_map(
                gsl::span(reinterpret_cast<libc_types::u8 const*>(t.map), t.map_size),
                gsl::span<libc_types::u8 const>(t.bitmaps, t.bitmaps_size),
                gsl::span(
                    reinterpret_cast<libc_types::u8*>(result.data()),
                    result.size()),
                gsl::span(
                    reinterpret_cast<libc_types::u8*>(t.out_bitmaps.data()),
                    t.out_bitmaps.size()),
                *t.kernel,
                {
                    .threads =
                        std::max(1u, std::thread::hardware_concurrency()),
                    .deswizzle = t.deswizzle,
                    .cache     = &t.cache,
                    .progress =
                        [](u32 done, u32 total) {
                            transcode.progress[0] = done;
                            transcode.progress[1] = total;
                        },
                });
            t.ok    = res.ok;
            t.error = res.error;
            t.stats = res.stats;
        } catch(std::exception const& e)
        {
            t.ok    = false;
            t.error = e.what();
        }
        t.finished = true;
    });
    return 1;
}

/*!
 * \brief 0 while the map is being transcoded, then 1 when it was, with the
 * patched map as the result, or -1 when it failed (blam_upload_error())
 */
EMSCRIPTEN_KEEPALIVE i32 blam_upload_transcode_poll()
{
    refresh_heap_views();
    if(!transcode.finished)
        return 0;
    transcode.worker.join();
    std::free(transcode.map);
    transcode.map = nullptr;
    refresh_heap_views();
    if(!transcode.ok)
    {
        last_error = transcode.error;
        result     = {};
        return -1;
    }
    return 1;
}

/* Images done and in total in the map being transcoded, two u32 */
EMSCRIPTEN_KEEPALIVE std::atomic<u32> const* blam_upload_transcode_progress()
{
    return transcode.progress;
}

/* mtx::map_stats of the last map transcoded, six u32 */
EMSCRIPTEN_KEEPALIVE mtx::map_stats const* blam_upload_transcode_stats()
{
    return &transcode.stats;
}

/*!
 * \brief Hand over the bitmaps.map patched by every map transcoded since
 * blam_upload_transcode_begin() as the result, and let go of the original
 */
EMSCRIPTEN_KEEPALIVE void blam_upload_transcode_end()
{
    if(transcode.worker.joinable())
    {
        transcode.worker.join();
        std::free(transcode.map);
        transcode.map = nullptr;
    }
    result = std::move(transcode.out_bitmaps);
    std::free(transcode.bitmaps);
    transcode.bitmaps      = nullptr;
    transcode.bitmaps_size = 0;
    transcode.out_bitmaps  = {};
    transcode.cache        = {};
}

EMSCRIPTEN_KEEPALIVE char const* blam_upload_error()
{
    return last_error.c_str();
}
}

int main()
{
    /* Everything is driven from shell.html, the runtime stays alive */
    return 0;
}
