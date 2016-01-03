#include <glbinding/wrappers/cframebuffer.h>

#include <glfunctions.h>
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

void coffee_graphics_alloc(size_t count, CFramebuffer* fb)
{
    CGuint *handles = new CGuint[count];
    glGenFramebuffers(count,handles);
    for(size_t i=0;i<count;i++)
    {
        fb[i].handle = handles[i];
        glBindFramebuffer(GL_READ_FRAMEBUFFER,fb[i].handle);
    }
    glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
    delete[] handles;
}

void coffee_graphics_free(size_t count, CFramebuffer* fb)
{
    CGuint *handles = new CGuint[count];
    for(size_t i=0;i<count;i++)
    {
        handles[i] = fb[i].handle;
        fb[i].handle = 0;
    }
    glDeleteFramebuffers(count,handles);
    delete[] handles;
}

void coffee_graphics_bind(const CFramebuffer& fb)
{
    glBindFramebuffer(gl_get(fb.target),
                      fb.handle);
}

void coffee_graphics_unbind(const CFramebuffer& fb)
{
    glBindFramebuffer(gl_get(fb.target),
                      0);
}

void coffee_graphics_bind(const CFramebuffer &fb, const CFBType &target)
{
    glBindFramebuffer(gl_get(target),
                      fb.handle);
}

void coffee_graphics_unbind(const CFramebuffer&, const CFBType &target)
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

bool coffee_graphics_framebuffer_check_valid(const CFramebuffer &fb)
{
    GLenum value = glCheckNamedFramebufferStatus(
                fb.handle,
                gl_get(fb.target));
    return _coffee_graphics_framebuffer_check_status(value);
}

bool coffee_graphics_framebuffer_check_valid_safe(const CFramebuffer &fb)
{
    coffee_graphics_bind(fb);
    GLenum value = glCheckFramebufferStatus(
                gl_get(fb.target));
    coffee_graphics_unbind(fb);
    return _coffee_graphics_framebuffer_check_status(value);
}

void coffee_graphics_framebuffer_attach_texture(
        CFramebuffer &fb,
        const CFramebufferAttachment &attachment)
{
    uint32 offset = (attachment.target == CFBAttachment::Color)
            ? attachment.attachLevel: 0;

    glNamedFramebufferTexture(
                fb.handle,
                gl_geti(attachment.target,offset),
                attachment.texture->handle,
                attachment.level);
}

void coffee_graphics_framebuffer_attach_texture_safe(CFramebuffer &fb,
        const CFramebufferAttachment &attachment)
{
    uint32 offset = (attachment.target == CFBAttachment::Color)
            ? attachment.attachLevel: 0;

    coffee_graphics_bind(fb);
    glFramebufferTexture(
                gl_get(fb.target),
                gl_geti(attachment.target,offset),
                attachment.texture->handle,
                attachment.level);
    coffee_graphics_unbind(fb);
}

void coffee_graphics_framebuffer_blit(
        const CFramebuffer &srcFb, const CFramebuffer &trgFb,
        const CRectF &srcRect, const CRectF &trgRect,
        const CClearFlag &mask, const CFBFilter &filter)
{
    glBlitNamedFramebuffer(
                srcFb.handle, trgFb.handle,
                srcRect.left(), srcRect.bottom(), srcRect.right(), srcRect.top(),
                trgRect.left(), trgRect.bottom(), trgRect.right(), trgRect.top(),
                gl_getf(mask),
                gl_get(filter));
}

void coffee_graphics_framebuffer_blit_safe(const CFramebuffer &srcFb, const CFramebuffer &trgFb,
        const CRectF &srcRect, const CRectF &trgRect,
        const CClearFlag &mask, const CFBFilter &filter)
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

void coffee_graphics_framebuffer_readpixels(
        const CFramebuffer *tex, const CDataType &dType,
        CTexFormat const& fmt, c_ptr dPtr,
        CFBAttachment const& attachment, CGint const& colorIndex)
{
    glReadBuffer(gl_geti(attachment,colorIndex));
    glReadPixels(0,0,tex->size.w,tex->size.h,gl_get(fmt),gl_get(dType),dPtr);
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
