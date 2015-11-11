#include "cframebuffer.h"

#include "glbinding.h"
#include <coffee/core/base/cdebug.h>

namespace Coffee{
namespace CGraphicsWrappers{

CFramebuffer::CFramebuffer():
    handle(0),
    size(0,0),
    target(GL_NONE)
{
}

void coffee_graphics_alloc(CFramebuffer* fb)
{
    glCreateFramebuffers(1,&fb->handle);
}

void coffee_graphics_free(CFramebuffer* fb)
{
    glDeleteFramebuffers(1,&fb->handle);
    fb->handle = 0;
}

void coffee_graphics_bind(const CFramebuffer* fb)
{
    glBindFramebuffer(fb->target,fb->handle);
}

void coffee_graphics_unbind(const CFramebuffer* fb)
{
    glBindFramebuffer(fb->target,0);
}

void coffee_graphics_bind(const CFramebuffer* fb, const GLenum &target)
{
    glBindFramebuffer(target,fb->handle);
}

void coffee_graphics_unbind(const CFramebuffer*, const GLenum &target)
{
    glBindFramebuffer(target,0);
}

inline bool _coffee_graphics_framebuffer_check_status(const GLenum& value)
{
    if(value==GL_FRAMEBUFFER_COMPLETE)
        return true;
    cWarning("Framebuffer is incomplete: %s",
             _glbinding_get_string(value).c_str());
    return false;
}

bool coffee_graphics_framebuffer_check_valid(const CFramebuffer *fb)
{
    GLenum value = glCheckNamedFramebufferStatus(fb->handle,fb->target);
    return _coffee_graphics_framebuffer_check_status(value);
}

bool coffee_graphics_framebuffer_check_valid_safe(const CFramebuffer *fb)
{
    coffee_graphics_bind(fb);
    GLenum value = glCheckFramebufferStatus(fb->target);
    coffee_graphics_unbind(fb);
    return _coffee_graphics_framebuffer_check_status(value);
}

void coffee_graphics_framebuffer_attach_texture(
        const CFramebuffer *fb,
        const CFramebufferAttachment* attachment)
{
    glNamedFramebufferTexture(
                fb->handle,
                attachment->target,
                attachment->texture->handle,
                attachment->level);
}

void coffee_graphics_framebuffer_attach_texture_safe(
        const CFramebuffer *fb,
        const CFramebufferAttachment *attachment)
{
    coffee_graphics_bind(fb);
    glFramebufferTexture(
                fb->target,
                attachment->target,
                attachment->texture->handle,
                attachment->level);
    coffee_graphics_unbind(fb);
}

void coffee_graphics_framebuffer_blit(const CFramebuffer *srcFb, const CFramebuffer *trgFb,
        const CRectF &srcRect, const CRectF &trgRect,
        CClearFlag mask, GLenum filter)
{
    glBlitNamedFramebuffer(
                srcFb->handle, trgFb->handle,
                srcRect.left(), srcRect.bottom(), srcRect.right(), srcRect.top(),
                trgRect.left(), trgRect.bottom(), trgRect.right(), trgRect.top(),
                mask,filter);
}

void coffee_graphics_framebuffer_blit_safe(
        const CFramebuffer *srcFb, const CFramebuffer *trgFb,
        const CRectF &srcRect, const CRectF &trgRect,
        CClearFlag mask, GLenum filter)
{
    coffee_graphics_bind(srcFb,GL_READ_FRAMEBUFFER);
    coffee_graphics_bind(trgFb,GL_DRAW_FRAMEBUFFER);
    glBlitFramebuffer(
                srcRect.left(), srcRect.bottom(), srcRect.right(), srcRect.top(),
                trgRect.left(), trgRect.bottom(), trgRect.right(), trgRect.top(),
                mask,filter);
    coffee_graphics_unbind(srcFb,GL_READ_FRAMEBUFFER);
    coffee_graphics_unbind(trgFb,GL_DRAW_FRAMEBUFFER);
}

CFramebufferAttachment::CFramebufferAttachment():
    target(CFBAttachment::Color),
    texture(nullptr),
    attachLevel(0),
    level(0)
{
}

CFramebufferAttachment::CFramebufferAttachment(
        CFBAttachment target,
        const CTexture *texture,
        CGint attlevel,
        CGint level):
    target(target),
    texture(texture),
    attachLevel(attlevel),
    level(level)
{
}

}
}
