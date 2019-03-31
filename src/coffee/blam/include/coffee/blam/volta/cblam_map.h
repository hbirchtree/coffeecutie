#pragma once

#include "cblam_structures.h"

namespace Coffee {
namespace Blam {

/*!
 * \brief Get the proper, full name of a map
 * \param map Map from which we identify a string
 * \return A null-terminated string with a map name or empty (empty if map name
 * is not recognized)
 */
extern cstring file_header_full_mapname(const file_header_t* map);

/*!
 * \brief Validate and get the file header from a base pointer, mostly
 * validation \param baseptr Pointer to data which will be validated \param
 * expectedVersion Expected version of Halo to be parsed. If version does not
 * match, fail. \return Returns a casted pointer on success, nullptr on failure
 */
extern file_header_t const* file_header_get(
    c_cptr baseptr, version_t expectedVersion);

/*!
 * \brief Sets index magic for a tag index and modifies it with the correct
 * magic number. \param tagindex Tag index to update \param tagIndexOffset Tag
 * index offset from map, used to calculate magic
 */
extern void tag_index_magic(tag_index_t* tagindex, i32 tagIndexOffset);

/*!
 * \brief Get the tag index pointer out of the file header
 * \param file File header from which we want a tag index
 * \return A pointer to the tag index
 */
extern const tag_index_t* tag_index_ptr(const file_header_t* file);

/*!
 * \brief Get a pointer to the start of the tag index items, traversed as a
 * simple array \param file File header from which we get the pointer \return A
 * pointer to the first tag index item
 */
extern const index_item_t* tag_index_get_items(const file_header_t* file);

extern const index_item_t* tag_index_get_item(
    const file_header_t* file, const tag_index_t* tags, i32 tag_id);

/*!
 * \brief Creates a copy of the tag index and sets up its magic index
 * \param file File header from which we extract a tag index
 * \return A tag index object copied from the file
 */
extern tag_index_t tag_index_get(const file_header_t* file);

/*!
 * \brief Used to decode magic index pointers
 * \param base Pointer to the file header
 * \param magic Magic number from tag index
 * \param offset Data offset which will be transformed
 * \return A pointer to the described data
 */
extern const void* blam_mptr(c_cptr base, i32 magic, i32 offset);

/*!
 * \brief Acquire the name string for a tag
 * \param idx Index item for which a string will be given
 * \param map Map file pointer
 * \param tagindex Index item's tag index for the index magic
 * \return A null-terminated string with the index item's name
 */
extern cstring index_item_get_string(
    const index_item_t*  idx,
    const file_header_t* map,
    const tag_index_t*   tagindex);

/*!
 * \brief Get the name of a referenced tag
 * \param tag
 * \param file
 * \param tags
 * \return
 */
extern cstring tagref_get_name(
    const tagref_t* tag, const file_header_t* file, const tag_index_t* tags);

} // namespace Blam
} // namespace Coffee
