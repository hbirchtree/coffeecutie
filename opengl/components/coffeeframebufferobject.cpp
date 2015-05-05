#include "coffeeframebufferobject.h"

CoffeeFrameBufferObject::CoffeeFrameBufferObject(QObject *parent) : QObject(parent)
{

}

void CoffeeFrameBufferObject::createFramebuffer()
{
    GLuint framebuffer = 0;
    glGenFramebuffers(1,&framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

    GLuint texture = allocTexture(1280,720,GL_RGB,GL_RGB);
    GLuint depth = allocRenderBuffer(GL_DEPTH_COMPONENT,1280,720);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, depth);
    glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,texture,0);

    framebufferHandle = framebuffer;
    textureHandle = texture;

    GLenum drawBufs[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,drawBufs);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void CoffeeFrameBufferObject::bindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER,framebufferHandle);
}

void CoffeeFrameBufferObject::unbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

GLuint CoffeeFrameBufferObject::allocTexture(int w,int h, gl::GLenum internal, gl::GLenum format)
{
    GLuint handle = 0;
    glGenTextures(1,&handle);

    glBindTexture(GL_TEXTURE_2D,handle);

    glTexImage2D(GL_TEXTURE_2D,0,static_cast<int>(internal),w,h,0,format,GL_UNSIGNED_BYTE,0);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,static_cast<int>(GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,static_cast<int>(GL_NEAREST));

    glBindTexture(GL_TEXTURE_2D,0);
    return handle;
}

GLuint CoffeeFrameBufferObject::allocRenderBuffer(gl::GLenum component, int w, int h)
{
    GLuint handle = 0;
    glGenRenderbuffers(1,&handle);
    glBindRenderbuffer(GL_RENDERBUFFER,handle);

    glRenderbufferStorage(GL_RENDERBUFFER,component,w,h);

    glBindRenderbuffer(GL_RENDERBUFFER,0);
    return handle;
}
GLuint CoffeeFrameBufferObject::getTextureHandle() const
{
    return textureHandle;
}


