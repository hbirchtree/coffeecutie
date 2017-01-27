#ifndef CBLAM_BITM
#define CBLAM_BITM

#include <coffee/core/coffee_macros.h>
#include "cblam_structures.h"

namespace Coffee{
namespace Blam{

/*!
 * \brief To acquire a bitmap from a tag item
 * \param item Item which texture should be fetched
 * \param map File header from which we search
 * \param magic Magic tag number
 * \param numImages Number of images are written to this
 * \return
 */
extern const bitm_image_t* bitm_get(
        const index_item_t* item,
        const file_header_t* map,
        int32 magic,
        int32 *numImages);

/*!
 * \brief Transforms the image structure to a GL-ready structure which may be uploaded directly, doing any possible decoding on the GPU. The data is read directly from the source file without copying. (Unless the GL does this)
 * \param img
 * \param bitmfile
 * \return A struct containing data readily digested for GL
 */
extern bitm_texture_t bitm_get_texture(
        const bitm_image_t* img,
        const void* bitmfile);

}
}

#endif
