#include <blam/volta/blam_atlas.h>
#include <blam/volta/blam_file_header.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_versions.h>

#include <emscripten/emscripten.h>
#include <magic_enum/magic_enum.hpp>

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

#if !defined(BLAM_HAS_COMPRESSION)
#error "BlamMapUpload needs CoreZ to inflate Xbox maps"
#endif

/* The JS half of this tool lives in shell.html. It reads the first
 * header_size bytes of each dropped file, asks blam_upload_identify() what it
 * is, and only hands the whole file over to blam_upload_decompress() when it
 * is a compressed map. Everything else goes into IndexedDB untouched. */

namespace {

using blam::file_header_t;
using libc_types::i32;
using libc_types::u32;

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
};

/* Read field by field from JS, layout is fixed */
struct upload_info_t
{
    i32  kind;
    i32  compressed;
    u32  decomp_len;
    char name[36];
};

static_assert(sizeof(upload_info_t) == 48);

upload_info_t     info{};
std::vector<char> result;
std::string       last_error;

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
kind_t atlas_kind(semantic::BytesConst const& data, double file_size)
{
    if(data.size < sizeof(blam::tag_atlas_t))
        return kind_t::unknown;
    blam::tag_atlas_t atlas;
    std::memcpy(&atlas, data.data, sizeof(atlas));

    auto const header_end = static_cast<double>(sizeof(blam::tag_atlas_t));
    auto const locators_end =
        static_cast<double>(atlas.locators_offset) +
        static_cast<double>(atlas.locators_count) * sizeof(blam::locator_block);
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
 * \param file_size Size of the whole file, which may be more than size
 * \return Pointer to an upload_info_t, valid until the next call
 */
EMSCRIPTEN_KEEPALIVE upload_info_t const* blam_upload_identify(
    char const* data, u32 size, double file_size)
{
    info      = {};
    auto head = semantic::BytesConst::of(
        reinterpret_cast<libc_types::u8 const*>(data), size);

    if(auto header = map_header(head))
    {
        info.kind = static_cast<i32>(kind_of(blam::from_le(header->version)));
        info.decomp_len = blam::from_le(header->decomp_len);
        info.compressed = static_cast<double>(info.decomp_len) != file_size;
        auto name       = header->name.data;
        std::memcpy(
            info.name,
            name.data(),
            std::min(
                sizeof(info.name) - 1, ::strnlen(name.data(), name.size())));
    } else if(
        size >= sizeof(blam::file_header_trial_t) &&
        reinterpret_cast<blam::file_header_trial_t const*>(data)->valid())
        info.kind = static_cast<i32>(kind_t::trial);
    else
        info.kind = static_cast<i32>(atlas_kind(head, file_size));
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
