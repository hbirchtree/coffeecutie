#include "coffeeframebufferobject.h"

#include "opengl/helpers/texturehelper.h"

CoffeeFramebufferObject::CoffeeFramebufferObject(QObject *parent) :
    CoffeeFramebufferBaseClass(parent)
{
}

void CoffeeFramebufferObject::createFramebuffer(QSize windowSize, uint sampling)
{
    this->sampling = sampling;
    this->renderSize = windowSize*sampling;
    CoffeeFramebufferBaseClass::createFramebuffer(windowSize);
}

void CoffeeFramebufferObject::setSize(QSize windowSize, uint sampling)
{
    this->sampling = sampling;
    this->renderSize = windowSize*sampling;
    m_ready = false;
}

void CoffeeFramebufferObject::cleanup()
{
    glDeleteFramebuffersEXT(1,&framebufferHandle);
    glDeleteTextures(textureHandles.size(),textureHandles.data());
}

uint CoffeeFramebufferObject::getSampling() const
{
    return sampling;
}

void CoffeeFramebufferObject::bindFramebufferRead()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER,framebufferHandle);
}

void CoffeeFramebufferObject::bindFramebufferWrite()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,framebufferHandle);
}

void CoffeeFramebufferObject::unbindFramebufferRead()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER,0);
}

void CoffeeFramebufferObject::unbindFramebufferWrite()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
}

bool CoffeeFramebufferObject::ready() const
{
    return m_ready;
}

QVector<GLuint>* CoffeeFramebufferObject::getTextureHandle()
{
    return &textureHandles;
}

void CoffeeFramebufferObject::bindFramebuffer()
{
    CoffeeFramebufferBaseClass::bindFramebuffer(framebufferHandle);
}

void CoffeeFramebufferObject::setNumTextures(uint textures)
{
    this->numTextures = textures;
}

void CoffeeFramebufferObject::resizeViewport(QSize windowSize)
{
    this->windowSize = windowSize;
    this->renderSize = windowSize*sampling;

    resizeFramebuffer();
}

void CoffeeFramebufferObject::updateSampling(uint sampling)
{
    this->sampling = sampling;
    m_ready = false;
}

void CoffeeFramebufferObject::setReady(bool ready)
{
    m_ready = ready;
}

void CoffeeFramebufferObject::resizeFramebuffer()
{
    if(framebufferActive){
        cleanup(); //We clear the previous framebuffer, renderbuffers and texture if the exist
        framebufferActive = true;
    }

    if(renderSize.isEmpty())
        return;

    GLuint framebuffer = 0;
    glGenFramebuffers(1,&framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

    QVector<GLenum> drawBufs;


    for(uint i=0;i<numTextures;i++){
        GLuint texture = allocTexture(renderSize.width(),renderSize.height(),
                                      GL_RGBA8,GL_BGRA,
                                      GL_LINEAR,GL_FLOAT);

        GLenum attachment = GL_COLOR_ATTACHMENT0+i;
        drawBufs.append(attachment);

        glFramebufferTexture2D(GL_FRAMEBUFFER,attachment,
                               GL_TEXTURE_2D,texture,0);
        textureHandles.append(texture);
    }
    {
        GLuint depthText = allocTexture(renderSize.width(),renderSize.height(),
                                        GL_DEPTH_COMPONENT24,GL_DEPTH_COMPONENT,
                                        GL_NEAREST,GL_FLOAT);

        glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,depthText,0);
        textureHandles.append(depthText);
    }

    glDrawBuffers(drawBufs.size(),drawBufs.data());

    GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(fbStatus){
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    default:
        qWarning("Failed to create framebuffer: %s",
                 glbinding::Meta::getString(fbStatus).c_str());
        break;
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    framebufferHandle = framebuffer;
}


