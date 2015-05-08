#include "coffeedepthbufferobject.h"

CoffeeDepthBufferObject::CoffeeDepthBufferObject(QObject *parent)
    : CoffeeFrameBufferObject(parent)
{
}

void CoffeeDepthBufferObject::resizeFramebuffer()
{
    if(framebufferActive){
        cleanup(); //We clear the previous framebuffer, renderbuffers and texture if the exist
        framebufferActive=false;
    }

    qDebug("Creating framebuffer: (rendering)%ix%i -> (display)%ix%i",renderSize.width(),renderSize.height(),
           windowSize.width(),windowSize.height());

    GLuint framebuffer = 0;
    glGenFramebuffersEXT(1,&framebuffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);


    GLuint texture = allocTexture(renderSize.width(),renderSize.height(),GL_DEPTH_COMPONENT16,
                                    GL_DEPTH_COMPONENT,GL_NEAREST,GL_FLOAT);

    GLuint depth = allocRenderBuffer(GL_DEPTH_COMPONENT,renderSize.width(),renderSize.height());

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, depth);

    glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT,GL_DEPTH_ATTACHMENT_EXT,texture,0);

    framebufferHandle = framebuffer;
    textureHandle = texture;
    renderbuffers.append(depth);

    glDrawBuffer(GL_NONE);

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
