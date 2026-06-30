#pragma once

#include "blam_base_types.h"
#include "blam_structures.h"
#include "blam_versions.h"

namespace blam {

/*!
 * \brief A file header located from the start of a map file
 */
struct alignas(4) file_header_t
{
    bl_header id;      /*!< Header value, should correspond with specific data*/
    version_t version; /*!< Version of Halo, determines the process*/
    u32       decomp_len; /*!< Decompressed length */
    u32 trailing_space;   /*!< Specifies size of empty space at end of file */
    u32 tag_index_offset; /*!< Offset to tag index*/
    i32 tag_index_len;    /*!< Length of tag index item metadata*/
    u32 reserved_1[2];
    bl_string name;      /*!< Name identifier for map*/
    bl_string buildDate; /*!< Build date for the map file*/
    maptype_t map_type;  /*!< Type of map, determines whether it is playable*/
    i32       unknown_4;
    i32       reserved_2[485];
    bl_footer footer; /*!< Footer value, should correspond with specific data*/

    /*!
     * \brief Get the proper, full name of a map
     * \param map Map from which we identify a string
     * \return A null-terminated string with a map name or empty (empty if map
     * name is not recognized)
     */
    inline std::string_view full_mapname() const
    {
        for(auto name : map_names)
        {
            if(this->name.str() == name.first)
                return name.second;
        }
        return name;
    }

    /*!
     * \brief Validate and get the file header from a base pointer, mostly
     * validation
     * \param data Pointer to data which will be validated
     * \param expected Expected version of Halo to be parsed. If version does
     *    not match, fail.
     * \return Returns a casted pointer on success, nullptr on failure
     */
    static result<file_header_t const*, map_load_error> from_data(
        semantic::BytesConst const& data, pc_version_t);

    static result<file_header_t const*, map_load_error> from_data(
        semantic::BytesConst const& data, custom_version_t);

    static result<file_header_t const*, map_load_error> from_data(
        semantic::BytesConst const& data, xbox_version_t);

    static stl_types::result<file_header_t const*, map_load_error> from_data(
        semantic::BytesConst const& data, mcc_version_t);

    static stl_types::result<file_header_t const*, map_load_error> from_data(
        semantic::BytesConst const& data, trial_version_t);

    inline bool is_xbox() const
    {
        return version == version_t::xbox;
    }

    inline void copy_to(file_header_t& other) const
    {
        other = *this;
    }

  private:
    file_header_t& operator=(file_header_t const& other) = default;
};

/*!
 * \brief Halo PC Trial/Demo cache header.
 *
 * According to SnowyMouse:
 * Bungie deliberately scrambled the field layout for the Trial release so it
 * doesn't resemble the retail header (offsets cross-referenced against Invader's
 * CacheFileDemoHeader, which calls this version "Demo"). Same 2048-byte size,
 * but every field sits at a different, mostly unaligned offset — hence the
 * packing. In-file the head/foot literals are "Ehed"/"Gfot" (vs retail
 * "head"/"foot"), byte-reversed in memory to "dehE"/"tofG".
 *
 * The rest of the loader only understands the retail layout, so this is
 * translated up-front via to_retail().
 */
#pragma pack(push, 1)
struct file_header_trial_t
{
    u8        pad_0[0x002];
    u16       map_type;
    u8        pad_1[0x2BC];
    bl_header id;
    u32       tag_index_len;
    bl_string buildDate;
    u8        pad_2[0x2A0];
    version_t version;
    bl_string name;
    u8        pad_3[0x004];
    u32       crc32;
    u8        pad_4[0x034];
    u32       decomp_len;
    u32       tag_index_offset;
    bl_footer footer;
    u8        pad_5[0x800 - 0x5F4];

    inline bool valid() const
    {
        return version == version_t::trial
               && id == bl_header{'d', 'e', 'h', 'E'}
               && footer == bl_footer{'t', 'o', 'f', 'G'};
    }

    /*!
     * \brief Translate to the retail header layout the rest of the loader uses.
     * Trial maps are uncompressed and have no trailing-space field, so
     * trailing_space is normalised to 0.
     */
    inline file_header_t to_retail() const
    {
        file_header_t out{};
        out.id               = bl_header{'d', 'a', 'e', 'h'}; // "daeh"
        out.version          = version;
        out.decomp_len       = decomp_len;
        out.trailing_space   = 0;
        out.tag_index_offset = tag_index_offset;
        out.tag_index_len    = static_cast<i32>(tag_index_len);
        out.name             = name;
        out.buildDate        = buildDate;
        out.map_type         = static_cast<maptype_t>(map_type);
        out.footer           = bl_footer{'t', 'o', 'o', 'f'}; // "toof"
        return out;
    }
};
#pragma pack(pop)

static_assert(offsetof(file_header_trial_t, map_type) == 0x002);
static_assert(offsetof(file_header_trial_t, id) == 0x2C0);
static_assert(offsetof(file_header_trial_t, tag_index_len) == 0x2C4);
static_assert(offsetof(file_header_trial_t, buildDate) == 0x2C8);
static_assert(offsetof(file_header_trial_t, version) == 0x588);
static_assert(offsetof(file_header_trial_t, name) == 0x58C);
static_assert(offsetof(file_header_trial_t, crc32) == 0x5B0);
static_assert(offsetof(file_header_trial_t, decomp_len) == 0x5E8);
static_assert(offsetof(file_header_trial_t, tag_index_offset) == 0x5EC);
static_assert(offsetof(file_header_trial_t, footer) == 0x5F0);
static_assert(sizeof(file_header_trial_t) == 0x800);

} // namespace blam
