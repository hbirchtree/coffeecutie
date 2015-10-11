#ifndef CIMAGE_LIB
#define CIMAGE_LIB

#include "coffee_types.h"
#include "coffee/cfiles.h"
#include "coffee/cmath.h"

namespace Coffee{
namespace CStbImageLib{

using namespace CResources;

/*!
 * \brief STB image format for standard imported images
 */
struct CStbImage
{
    ubyte* data = nullptr; /*!< Pointer to image data*/
    CSize size; /*!< Image resolution*/
    int bpp = 0; /*!< Describes image format according to STB*/
};

/*!
 * \brief STB image format for constant data which may not be modified
 */
struct CStbImageConst
{
    const ubyte* data = nullptr; /*!< const pointer to image data*/
    CSize size; /*!< Image resolution*/
    int bpp = 0; /*!< Describes image format according to STB*/
};

/*!
 * \brief Print any potential STB errors
 */
extern void coffee_stb_error();
/*!
 * \brief Load STB image from file
 * \param target Target to load into
 * \param src Source to load from
 * \return True if success
 */
extern bool coffee_stb_image_load(
        CStbImage* target,
        CResource* src);
/*!
 * \brief Function used by STB to write data into resource. Allocates and copies data into resource.
 * \param ctxt Context pointer, CResource in our case
 * \param data Pointer to data
 * \param size Size of data
 */
extern void _stbi_write_data(
        void* ctxt,
        void* data,
        int size);
/*!
 * \brief Resize an image using STB
 * \param img Target image
 * \param target New resolution
 * \param channels Amount of channels
 * \return True if success
 */
extern bool coffee_stb_image_resize(
        CStbImage* img,
        const CSize& target,
        int channels);
/*!
 * \brief Save STB image to PNG file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool coffee_stb_image_save_png(
        CResource* target,
        CStbImageConst* src);
/*!
 * \brief Save STB image to PNG file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool coffee_stb_image_save_png(
        CResource* target,
        CStbImage* src);
/*!
 * \brief Save STB image to TGA file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool coffee_stb_image_save_tga(
        CResource* target,
        CStbImage* src);

/*!
 * \brief Flip image vertically, allocates and frees memory
 * \param src
 */
extern void coffee_stb_image_flip_vertical(
        CStbImage* src);
/*!
 * \brief Flip image horizontally, allocates and frees memory
 * \param src
 */
extern void coffee_stb_image_flip_horizontal(
        CStbImage* src);
/*!
 * \brief Free image data
 * \param img
 */
extern void coffee_stb_image_free(CStbImage* img);

} //CStbImageLib
} //Coffee

#endif
