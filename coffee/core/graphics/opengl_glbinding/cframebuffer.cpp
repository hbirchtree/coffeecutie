#include "cframebuffer.h"

#include "glbinding.h"
#include <coffee/core/base/cdebug.h>

namespace Coffee{
namespace CGraphicsWrappers{

static CFramebuffer fb_default;

const CFramebuffer& coffee_graphics_framebuffer_get_default()
{
    fb_default.handle = opengl_default_framebuffer;
    fb_default.target = CFBType::Draw;

    return fb_default;
}

CFramebuffer::CFramebuffer():
    handle(0),
    size(0,0),
    target(CFBType::All)
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
    glBindFramebuffer(gl_get(fb->target),
                      fb->handle);
}

void coffee_graphics_unbind(const CFramebuffer* fb)
{
    glBindFramebuffer(gl_get(fb->target),
                      0);
}

void coffee_graphics_bind(const CFramebuffer* fb, const CFBType &target)
{
    glBindFramebuffer(gl_get(target),
                      fb->handle);
}

void coffee_graphics_unbind(const CFramebuffer*, const CFBType &target)
{
    glBindFramebuffer(gl_get(target),
                      0);
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
    GLenum value = glCheckNamedFramebufferStatus(
                fb->handle,
                gl_get(fb->target));
    return _coffee_graphics_framebuffer_check_status(value);
}

bool coffee_graphics_framebuffer_check_valid_safe(const CFramebuffer *fb)
{
    coffee_graphics_bind(fb);
    GLenum value = glCheckFramebufferStatus(
                gl_get(fb->target));
    coffee_graphics_unbind(fb);
    return _coffee_graphics_framebuffer_check_status(value);
}

void coffee_graphics_framebuffer_attach_texture(
        CFramebuffer *fb,
        const CFramebufferAttachment* attachment)
{
    uint32 offset = (attachment->target == CFBAttachment::Color)
            ? attachment->attachLevel: 0;

    glNamedFramebufferTexture(
                fb->handle,
                gl_geti(attachment->target,offset),
                attachment->texture->handle,
                attachment->level);
}

void coffee_graphics_framebuffer_attach_texture_safe(
        CFramebuffer *fb,
        const CFramebufferAttachment *attachment)
{
    uint32 offset = (attachment->target == CFBAttachment::Color)
            ? attachment->attachLevel: 0;

    coffee_graphics_bind(fb);
    glFramebufferTexture(
                gl_get(fb->target),
                gl_geti(attachment->target,offset),
                attachment->texture->handle,
                attachment->level);
    coffee_graphics_unbind(fb);
}

void coffee_graphics_framebuffer_blit(const CFramebuffer *srcFb, const CFramebuffer *trgFb,
        const CRectF &srcRect, const CRectF &trgRect,
        CClearFlag mask, CFBFilter filter)
{
    glBlitNamedFramebuffer(
                srcFb->handle, trgFb->handle,
                srcRect.left(), srcRect.bottom(), srcRect.right(), srcRect.top(),
                trgRect.left(), trgRect.bottom(), trgRect.right(), trgRect.top(),
                gl_getf(mask),
                gl_get(filter));
}

void coffee_graphics_framebuffer_blit_safe(const CFramebuffer *srcFb, const CFramebuffer *trgFb,
        const CRectF &srcRect, const CRectF &trgRect,
        CClearFlag mask, CFBFilter filter)
{
    coffee_graphics_bind(srcFb,CFBType::Read);
    coffee_graphics_bind(trgFb,CFBType::Draw);
    glBlitFramebuffer(
                srcRect.left(), srcRect.bottom(), srcRect.right(), srcRect.top(),
                trgRect.left(), trgRect.bottom(), trgRect.right(), trgRect.top(),
                gl_getf(mask),
                gl_get(filter));
    coffee_graphics_unbind(srcFb,CFBType::Read);
    coffee_graphics_unbind(trgFb,CFBType::Draw);
}

CFramebufferAttachment::CFramebufferAttachment():
    target(CFBAttachment::Color),
    attachLevel(0),
    texture(nullptr),
    level(0)
{
}

CFramebufferAttachment::CFramebufferAttachment(
        CFBAttachment target,
        const CTexture *texture,
        CGint attlevel,
        CGint level):
    target(target),
    attachLevel(attlevel),
    texture(texture),
    level(level)
{
}

}
}
