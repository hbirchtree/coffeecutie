#ifndef CFRAMEBUFFER
#define CFRAMEBUFFER

#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CFramebuffer
{
    CFramebuffer();

    CGhnd      handle;
    CSize       size;
    GLenum      target;

    void attach(CTexture* texture, GLenum attachment, CGint level)
    {
//        glFramebufferTexture2D(target,attachment,textureTarget,texture->handle,level);
        glNamedFramebufferTexture(handle,attachment,texture->handle,level);
    }
    void attach3D(CTexture* texture, GLenum attachment, CGint level, CGint zoffset,
                GLenum textureTarget, GLenum target = GL_FRAMEBUFFER)
    {
//        glFramebufferTexture3D(target,attachment,textureTarget,
//                               texture->handle,level,zoffset);
    }
};

struct CFramebufferAttachment
{
    CFramebufferAttachment():
        target(GL_NONE),
        texture(nullptr),
        level(0)
    {
    }
    CFramebufferAttachment(GLenum target, const CTexture* texture, CGint level):
        target(target),
        texture(texture),
        level(level)
    {
    }

    GLenum target;
    const CTexture* texture;
    GLint level;
};

static const CFramebuffer coffee_default_framebuffer;

/*!
 * \brief Allocate framebuffer object
 * \param fb
 */
extern void coffee_graphics_alloc(CFramebuffer* fb);
/*!
 * \brief Free a framebuffer object and set handle to default framebuffer
 * \param fb
 */
extern void coffee_graphics_free(CFramebuffer* fb);

/*!
 * \brief Bind framebuffer object
 * \param fb
 */
extern void coffee_graphics_bind(const CFramebuffer* fb);
/*!
 * \brief Bind framebuffer object to specific target
 * \param fb
 * \param target
 */
extern void coffee_graphics_bind(const CFramebuffer* fb, const GLenum& target);
/*!
 * \brief Unbind framebuffer object
 * \param fb
 */
extern void coffee_graphics_unbind(const CFramebuffer* fb);
/*!
 * \brief Unbind framebuffer object from specific target
 * \param target
 */
extern void coffee_graphics_unbind(const CFramebuffer*, const GLenum& target);

/*!
 * \brief Check if the framebuffer is complete. Rendering with an incomplete framebuffer will result in undefined behavior.
 * \param fb
 * \return True if the framebuffer is valid
 */
extern bool coffee_graphics_framebuffer_check_valid(const CFramebuffer* fb);
/*!
 * \brief Check if the framebuffer is complete. Rendering with an incomplete framebuffer will result in undefined behavior.
 * \param fb
 * \return True if the framebuffer is valid
 */
extern bool coffee_graphics_framebuffer_check_valid_safe(const CFramebuffer* fb);
/*!
 * \brief Attach 2D texture to framebuffer for rendering
 * \param fb
 * \param attachment
 */
extern void coffee_graphics_framebuffer_attach_texture(
        const CFramebuffer *fb,
        const CFramebufferAttachment* attachment);
/*!
 * \brief Attach 2D texture to framebuffer for rendering
 * \param fb
 * \param attachment
 */
extern void coffee_graphics_framebuffer_attach_texture_safe(
        const CFramebuffer *fb,
        const CFramebufferAttachment* attachment);

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
        const CFramebuffer* srcFb,
        const CFramebuffer* trgFb,
        const CRectF& srcRect,
        const CRectF& trgRect,
        CClearFlag mask, GLenum filter);
/*!
 * \brief Blit framebuffer to another framebuffer, perhaps the default one
 * \param srcFb
 * \param trgFb
 * \param srcRect
 * \param trgRect
 * \param mask
 * \param filter
 */
extern void coffee_graphics_framebuffer_blit_safe(const CFramebuffer* srcFb,
        const CFramebuffer* trgFb,
        const CRectF& srcRect,
        const CRectF& trgRect,
        CClearFlag mask, GLenum filter);

}
}

#endif // CFRAMEBUFFER

