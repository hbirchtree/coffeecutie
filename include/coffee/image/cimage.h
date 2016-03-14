#ifndef CIMAGE_LIB
#define CIMAGE_LIB

#include <coffee/core/CBase>

namespace Coffee{
namespace CStbImageLib{

using namespace CResources;

/*!
 * \brief STB image format for standard imported images
 */
struct CStbImage
{
    byte_t* data; /*!< Pointer to image data*/
    CSize size; /*!< Image resolution*/
    int bpp = 0; /*!< Describes image format according to STB*/
};

/*!
 * \brief STB image format for constant data which may not be modified
 */
struct CStbImageConst
{
    const byte_t* data = nullptr; /*!< const pointer to image data*/
    CSize size; /*!< Image resolution*/
    int bpp = 0; /*!< Describes image format according to STB*/
};

/*!
 * \brief Print any potential STB errors
 */
extern void Error();
/*!
 * \brief Load STB image from file
 * \param target Target to load into
 * \param src Source to load from
 * \return True if success
 */
extern bool LoadData(CStbImage* target,
                     const Resource* src,
                     PixelComponents comp = PixelComponents::RGBA);
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
extern bool Resize(
        CStbImage* img,
        const CSize& target,
        int channels);
/*!
 * \brief Save STB image to PNG file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool SavePNG(
        Resource* target,
        const CStbImageConst* src);
/*!
 * \brief Save STB image to PNG file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool SavePNG(
        Resource* target,
        const CStbImage* src);
/*!
 * \brief Save STB image to TGA file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool SaveTGA(
        Resource* target,
        const CStbImage* src);

/*!
 * \brief Flip image vertically, allocates and frees memory
 * \param src
 */
extern void FlipVertical(
        CStbImage* src);
/*!
 * \brief Flip image horizontally, allocates and frees memory
 * \param src
 */
extern void FlipHorizontal(
        CStbImage* src);
/*!
 * \brief Free image data
 * \param img
 */
extern void ImageFree(CStbImage* img);

} //CStbImageLib

namespace CImage{

/*!
 * \brief Only saves RGB data, R, RG and RGBA won't work
 * \param resolution
 * \param imgData
 * \param outdata
 */
extern void SaveTGA(const CSize &resolution,
                     const CByteData &imgData,
                     CByteData &outdata);

}

} //Coffee

#endif
