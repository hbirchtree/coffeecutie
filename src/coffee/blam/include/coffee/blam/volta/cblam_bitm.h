#pragma once

#include "cblam_structures.h"

namespace blam {

/*!
 * \brief To acquire a bitmap from a tag item
 * \param item Item which texture should be fetched
 * \param map File header from which we search
 * \param magic Magic tag number
 * \param numImages Number of images are written to this
 * \return
 */
extern const bitm_image_t* bitm_get(
    index_item_t const*  item,
    file_header_t const* map,
    i32                  magic,
    i32*                 numImages);

/*!
 * \brief Transforms the image structure to a GL-ready structure which may be
 * uploaded directly, doing any possible decoding on the GPU. The data is read
 * directly from the source file without copying. (Unless the GL does this)
 * \param img
 * \param bitmfile
 * \return A struct containing data readily digested for GL
 */
extern bitm_texture_t bitm_get_texture(
    bitm_image_t const* img, c_cptr bitmfile);

} // namespace blam
