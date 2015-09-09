#ifndef CFRAMEBUFFER
#define CFRAMEBUFFER

#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CFramebuffer
{
    GLuint      handle          = 0;
    CSize       size;
    GLenum      target          = GL_NONE;

    void create()
    {
        glGenFramebuffers(1,&handle);
    }
    void free()
    {
        glDeleteFramebuffers(1,&handle);
    }

    void bind(GLenum target = GL_FRAMEBUFFER)
    {
        glBindFramebuffer(target,handle);
    }
    void unbind(GLenum target = GL_FRAMEBUFFER)
    {
        glBindFramebuffer(target,0);
    }

    bool valid(GLenum target = GL_FRAMEBUFFER)
    {
        GLenum value = glCheckFramebufferStatus(target);
        if(value!=GL_FRAMEBUFFER_COMPLETE)
            cDebug("Framebuffer error: %s",glbinding::Meta::getString(value).c_str());
        return value==GL_FRAMEBUFFER_COMPLETE;
    }

    void attach2D(CTexture* texture, GLenum attachment, GLint level,
                GLenum textureTarget, GLenum target = GL_FRAMEBUFFER)
    {
        glFramebufferTexture2D(target,attachment,textureTarget,texture->handle,level);
    }
    void attach3D(CTexture* texture, GLenum attachment, GLint level, GLint zoffset,
                GLenum textureTarget, GLenum target = GL_FRAMEBUFFER)
    {
        glFramebufferTexture3D(target,attachment,textureTarget,
                               texture->handle,level,zoffset);
    }
};


}
}

#endif // CFRAMEBUFFER

