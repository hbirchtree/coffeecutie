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

} // namespace blam
