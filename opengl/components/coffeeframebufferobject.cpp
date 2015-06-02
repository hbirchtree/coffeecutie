#include "coffeeframebufferobject.h"

CoffeeFrameBufferObject::CoffeeFrameBufferObject(QObject *parent) : QObject(parent)
{

}

void CoffeeFrameBufferObject::cleanup()
{
    glDeleteFramebuffersEXT(1,&framebufferHandle);
    framebufferHandle = 0;
    for(GLuint renderbuffer : renderbuffers)
        glDeleteRenderbuffers(1,&renderbuffer);
    glDeleteTextures(1,&textureHandle);
    textureHandle = 0;
}

void CoffeeFrameBufferObject::createFramebuffer(QSize windowSize, uint sampling)
{
    this->windowSize = windowSize;
    this->renderSize = windowSize*sampling;
    this->sampling = sampling;
    resizeFramebuffer();
}

QSize CoffeeFrameBufferObject::getRenderSize() const
{
    return renderSize;
}

QSize CoffeeFrameBufferObject::getWindowSize() const
{
    return windowSize;
}

uint CoffeeFrameBufferObject::getSampling() const
{
    return sampling;
}

void CoffeeFrameBufferObject::bindFramebuffer()
{
    glViewport(0,0,renderSize.width(),renderSize.height());
    glBindFramebuffer(GL_FRAMEBUFFER,framebufferHandle);
}

void CoffeeFrameBufferObject::unbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,windowSize.width(),windowSize.height());
}

GLuint CoffeeFrameBufferObject::allocTexture(int w, int h, gl::GLenum internal, gl::GLenum format, GLenum filtering,GLenum datatype)
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

void CoffeeFrameBufferObject::resizeViewport(QSize windowSize)
{
    this->windowSize = windowSize;
    this->renderSize = windowSize*sampling;
    resizeFramebuffer();
}

void CoffeeFrameBufferObject::updateSampling(uint sampling)
{
    this->sampling = sampling;
    resizeViewport(windowSize);
}

void CoffeeFrameBufferObject::resizeFramebuffer()
{
    if(framebufferActive){
        cleanup(); //We clear the previous framebuffer, renderbuffers and texture if the exist
        framebufferActive=false;
    }

    qDebug("Creating framebuffer: %ix%i -> %ix%i",renderSize.width(),renderSize.height(),
           windowSize.width(),windowSize.height());

    GLuint framebuffer = 0;
    glGenFramebuffers(1,&framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

    GLuint texture = allocTexture(renderSize.width(),renderSize.height(),
                                  GL_RGBA,GL_RGBA,GL_LINEAR,GL_HALF_FLOAT);
//    GLuint depthtext = allocTexture(512,512,GL_DEPTH_COMPONENT16,
//                                    GL_DEPTH_COMPONENT,GL_NEAREST,GL_FLOAT);

    GLuint depth = allocRenderBuffer(GL_DEPTH_COMPONENT,renderSize.width(),renderSize.height());
    GLuint color = allocRenderBuffer(GL_RGBA,renderSize.width(),renderSize.height());

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
                                 GL_RENDERBUFFER,color);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,
                                 GL_RENDERBUFFER, depth);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
                              GL_TEXTURE_2D,texture,0);
//    glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,depthtext,0);

    framebufferHandle = framebuffer;
    textureHandle = texture;
    renderbuffers.append(depth);
    renderbuffers.append(color);

    GLenum drawBufs[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,drawBufs);

    GLenum status;
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status){
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    default:
        qWarning("Failed to create framebuffer!");
        break;
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    framebufferActive = true;
}


