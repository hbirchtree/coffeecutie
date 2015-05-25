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
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebufferHandle);
}

void CoffeeFrameBufferObject::unbindFramebuffer()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
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
    glGenRenderbuffersEXT(1,&handle);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,handle);

    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,component,w,h);

    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT,0);
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
    glGenFramebuffersEXT(1,&framebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);

    GLuint texture = allocTexture(renderSize.width(),renderSize.height(),
                                  GL_RGBA,GL_RGBA,GL_LINEAR,GL_HALF_FLOAT);
//    GLuint depthtext = allocTexture(512,512,GL_DEPTH_COMPONENT16,
//                                    GL_DEPTH_COMPONENT,GL_NEAREST,GL_FLOAT);

    GLuint depth = allocRenderBuffer(GL_DEPTH_COMPONENT,renderSize.width(),renderSize.height());
    GLuint color = allocRenderBuffer(GL_RGBA,renderSize.width(),renderSize.height());

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,
                                 GL_RENDERBUFFER_EXT,color);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, depth);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D,texture,0);
//    glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,depthtext,0);

    framebufferHandle = framebuffer;
    textureHandle = texture;
    renderbuffers.append(depth);
    renderbuffers.append(color);

    GLenum drawBufs[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,drawBufs);

    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status){
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    default:
        qWarning("Failed to create framebuffer!");
        break;
    }
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);

    framebufferActive = true;
}


