#ifndef CBLAM_MAP
#define CBLAM_MAP

#include <coffee/core/Types>
#include "cblam_structures.h"

namespace Coffee{
namespace CBlam{

/*!
 * \brief Get the proper, full name of a map
 * \param map Map from which we identify a string
 * \return A null-terminated string with a map name or empty (empty if map name is not recognized)
 */
extern cstring blam_file_header_full_mapname(
        const blam_file_header* map);

/*!
 * \brief Validate and get the file header from a base pointer, mostly validation
 * \param baseptr Pointer to data which will be validated
 * \param expectedVersion Expected version of Halo to be parsed. If version does not match, fail.
 * \return Returns a casted pointer on success, nullptr on failure
 */
extern blam_file_header* blam_file_header_get(void* baseptr, blam_version expectedVersion);

/*!
 * \brief Sets index magic for a tag index and modifies it with the correct magic number.
 * \param tagindex Tag index to update
 * \param tagIndexOffset Tag index offset from map, used to calculate magic
 */
extern void blam_tag_index_magic(
        blam_tag_index* tagindex,
        int32 tagIndexOffset);

/*!
 * \brief Get the tag index pointer out of the file header
 * \param file File header from which we want a tag index
 * \return A pointer to the tag index
 */
extern const blam_tag_index* blam_tag_index_ptr(
        const blam_file_header* file);

/*!
 * \brief Get a pointer to the start of the tag index items, traversed as a simple array
 * \param file File header from which we get the pointer
 * \return A pointer to the first tag index item
 */
extern const blam_index_item* blam_tag_index_get_items(
        const blam_file_header* file);

extern const blam_index_item* blam_tag_index_get_item(
        const blam_file_header* file,
        const blam_tag_index* tags,
        int32 tag_id);

/*!
 * \brief Creates a copy of the tag index and sets up its magic index
 * \param file File header from which we extract a tag index
 * \return A tag index object copied from the file
 */
extern blam_tag_index blam_tag_index_get(
        const blam_file_header* file);

/*!
 * \brief Used to decode magic index pointers
 * \param base Pointer to the file header
 * \param magic Magic number from tag index
 * \param offset Data offset which will be transformed
 * \return A pointer to the described data
 */
extern const void* blam_mptr(
        const void* base, int32 magic, int32 offset);

/*!
 * \brief Acquire the name string for a tag
 * \param idx Index item for which a string will be given
 * \param map Map file pointer
 * \param tagindex Index item's tag index for the index magic
 * \return A null-terminated string with the index item's name
 */
extern cstring blam_index_item_get_string(
        const blam_index_item* idx,
        const blam_file_header* map,
        const blam_tag_index* tagindex);

/*!
 * \brief Get the name of a referenced tag
 * \param tag
 * \param file
 * \param tags
 * \return
 */
extern cstring blam_tagref_get_name(
        const blam_tagref* tag,
        const blam_file_header* file,
        const blam_tag_index* tags);

}
}

#endif //CBLAM_MAP
