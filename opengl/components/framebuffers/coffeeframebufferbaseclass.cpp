#include "coffeeframebufferbaseclass.h"

CoffeeFramebufferBaseClass::CoffeeFramebufferBaseClass(QObject *parent) : QObject(parent)
{
}

GLuint CoffeeFramebufferBaseClass::allocTexture(int w, int h, gl::GLenum internal, gl::GLenum format, GLenum filtering,GLenum datatype)
{
    GLuint handle = 0;
    glGenTextures(1,&handle);

    glBindTexture(GL_TEXTURE_2D,handle);

    glTexImage2D(
                GL_TEXTURE_2D,
                0,
                static_cast<int>(internal),
                w,h,
                0,
                format,
                datatype,
                0);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,
                    static_cast<int>(GL_REPEAT));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,
                    static_cast<int>(GL_REPEAT));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
                    static_cast<int>(filtering));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                    static_cast<int>(filtering));

    glBindTexture(GL_TEXTURE_2D,0);
    return handle;
}

GLuint CoffeeFramebufferBaseClass::allocRenderBuffer(gl::GLenum component, int w, int h)
{
    GLuint handle = 0;
    glGenRenderbuffers(1,&handle);
    glBindRenderbuffer(GL_RENDERBUFFER,handle);

    glRenderbufferStorage(GL_RENDERBUFFER,component,w,h);

    glBindRenderbuffer(GL_RENDERBUFFER,0);
    return handle;
}

void CoffeeFramebufferBaseClass::unbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    if(m_viewport_set)
        glViewport(0,0,windowSize.width(),windowSize.height());
}

void CoffeeFramebufferBaseClass::bindFramebuffer(GLuint handle)
{
    if(m_viewport_set)
        glViewport(0,0,renderSize.width(),renderSize.height());
    glBindFramebuffer(GL_FRAMEBUFFER,handle);
}

QSize CoffeeFramebufferBaseClass::getRenderSize() const
{
    return renderSize;
}

QSize CoffeeFramebufferBaseClass::getWindowSize() const
{
    return windowSize;
}

void CoffeeFramebufferBaseClass::createFramebuffer(QSize windowSize)
{
    this->windowSize = windowSize;
    resizeFramebuffer();
}
