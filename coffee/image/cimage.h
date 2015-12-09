#ifndef CIMAGE_LIB
#define CIMAGE_LIB

#include <coffee/core/CBase>
#include <coffee/core/CGraphics>

namespace Coffee{
namespace CStbImageLib{

using namespace CResources;

/*!
 * \brief STB image format for standard imported images
 */
struct CStbImage
{
    ubyte_t* data = nullptr; /*!< Pointer to image data*/
    CSize size; /*!< Image resolution*/
    int bpp = 0; /*!< Describes image format according to STB*/
};

/*!
 * \brief STB image format for constant data which may not be modified
 */
struct CStbImageConst
{
    const ubyte_t* data = nullptr; /*!< const pointer to image data*/
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
        const CResource* src);
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
        const CStbImageConst* src);
/*!
 * \brief Save STB image to PNG file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool coffee_stb_image_save_png(
        CResource* target,
        const CStbImage* src);
/*!
 * \brief Save STB image to TGA file
 * \param target Target resource
 * \param src STB image to save
 * \return
 */
extern bool coffee_stb_image_save_tga(
        CResource* target,
        const CStbImage* src);

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

namespace CGraphicsWrappers{

/*!
 * \brief Debug function for dumping texture to file
 * \param tex
 * \param filename
 */
extern void     coffee_graphics_tex_dump(
        const CTexture* tex, cstring filename);

/*!
 * \brief Download GL texture to an image, sets image's properties
 * \param tex Texture to download
 * \param level Mipmap level to download
 * \param size Expected size of buffer (is allocated)
 * \param format Format of data
 * \param img Target image
 */
extern void     coffee_graphics_tex_download_texture(
        const CTexture* tex, CGint level,
        CGsize size, CTexFormat format, CStbImageLib::CStbImage* img);

/*!
 * \brief The simple way to load a texture
 * \param resource
 * \param location
 * \return Null if operation failed, valid pointer if success
 */
extern CTextureData* coffee_graphics_tex_create_texdata(
        const CResources::CResource &resource, c_ptr location);
/*!
 * \brief Free the texture data (correctly)
 * \param texd
 */
extern void coffee_graphics_tex_free_texdata(CTextureData* texd);

class CImportedTexture : public _cbasic_raii_container<CTextureData>
{
public:
    using _cbasic_raii_container::_cbasic_raii_container;
    virtual ~CImportedTexture()
    {
        coffee_graphics_tex_free_texdata(m_data);
    }
};

extern CImportedTexture coffee_graphics_tex_create_rtexdata(
        const CResources::CResource &resource);

}

} //Coffee

#endif
