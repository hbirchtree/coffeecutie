#ifndef CFRAMEBUFFER
#define CFRAMEBUFFER

#include "ctexture.h"
#include "copengl_enums.h"
#include "coffee/core/types/composite_types.h"

namespace Coffee{
namespace CGraphicsWrappers{

enum class CFBType : uint8
{
    All = 1,
    Draw = 2,
    Read = 3,
};

enum class CFBAttachment : uint8
{
    Color = 1,
    Depth = 2,
    Stencil = 3,
    DepthStencil = 4,
};

enum class CFBFilter : uint8
{
    Nearest = 1,
    Linear = 2,
};

struct CFramebuffer
{
    CFramebuffer();

    CGhnd handle;
    CSize size;
    CFBType target;
};

struct CFramebufferAttachment
{
    CFramebufferAttachment();
    CFramebufferAttachment(
            CFBAttachment target, const CTexture* texture,
            CGint attlevel, CGint level);

    CFBAttachment target;
    CGint attachLevel;
    const CTexture* texture;
    CGint level;
};

/*!
 * \brief Get the object representing the default framebuffer
 * \return A pointer to the default framebuffer
 */
extern const CFramebuffer &coffee_graphics_framebuffer_get_default();

/*!
 * \brief Allocate framebuffer object
 * \param fb
 */
extern void coffee_graphics_alloc(size_t count, CFramebuffer* fb);
/*!
 * \brief Free a framebuffer object and set handle to default framebuffer
 * \param fb
 */
extern void coffee_graphics_free(size_t count, CFramebuffer* fb);

/*!
 * \brief Bind framebuffer object
 * \param fb
 */
extern void coffee_graphics_bind(const CFramebuffer &fb);
/*!
 * \brief Bind framebuffer object to specific target
 * \param fb
 * \param target
 */
extern void coffee_graphics_bind(const CFramebuffer& fb, const CFBType& target);
/*!
 * \brief Unbind framebuffer object
 * \param fb
 */
extern void coffee_graphics_unbind(const CFramebuffer& fb);
/*!
 * \brief Unbind framebuffer object from specific target
 * \param target
 */
extern void coffee_graphics_unbind(const CFramebuffer &, const CFBType& target);

/*!
 * \brief Check if the framebuffer is complete. Rendering with an incomplete framebuffer will result in undefined behavior.
 * \param fb
 * \return True if the framebuffer is valid
 */
extern bool coffee_graphics_framebuffer_check_valid(const CFramebuffer& fb);
/*!
 * \brief Check if the framebuffer is complete. Rendering with an incomplete framebuffer will result in undefined behavior.
 * \param fb
 * \return True if the framebuffer is valid
 */
extern bool coffee_graphics_framebuffer_check_valid_safe(const CFramebuffer& fb);
/*!
 * \brief Attach 2D texture to framebuffer for rendering
 * \param fb
 * \param attachment
 */
extern void coffee_graphics_framebuffer_attach_texture(
        CFramebuffer &fb,
        const CFramebufferAttachment& attachment);
/*!
 * \brief Attach 2D texture to framebuffer for rendering
 * \param fb
 * \param attachment
 */
extern void coffee_graphics_framebuffer_attach_texture_safe(
        CFramebuffer &fb,
        const CFramebufferAttachment& attachment);

/*!
 * \brief Blit framebuffer to another framebuffer, perhaps the default one
 * \param srcFb
 * \param trgFb
 * \param srcRect
 * \param trgRect
 * \param mask
 * \param filter
 */
extern void coffee_graphics_framebuffer_blit(
        const CFramebuffer& srcFb,
        const CFramebuffer& trgFb,
        const CRectF& srcRect,
        const CRectF& trgRect,
        CClearFlag const& mask, CFBFilter const& filter);
/*!
 * \brief Blit framebuffer to another framebuffer, perhaps the default one
 * \param srcFb
 * \param trgFb
 * \param srcRect
 * \param trgRect
 * \param mask
 * \param filter
 */
extern void coffee_graphics_framebuffer_blit_safe(
        const CFramebuffer& srcFb,
        const CFramebuffer& trgFb,
        const CRectF& srcRect,
        const CRectF& trgRect,
        CClearFlag const& mask, CFBFilter const& filter);

/*!
 * \brief Intended for use with PBOs! Don't abuse this with copying to host memory!
 * \param tex
 * \param dType
 * \param dPtr
 */
extern void coffee_graphics_framebuffer_readpixels(
        CTexture const& tex, CDataType const& dType,
        const CTexFormat &fmt, c_ptr dPtr);

}
}

#endif // CFRAMEBUFFER

