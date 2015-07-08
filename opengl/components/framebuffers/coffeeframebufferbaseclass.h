#ifndef COFFEEFRAMEBUFFERBASECLASS_H
#define COFFEEFRAMEBUFFERBASECLASS_H

#include "general/common.h"
class CoffeeFramebufferBaseClass
{

public:
    virtual void bindFramebuffer() = 0;
    virtual void unbindFramebuffer();

    virtual void createFramebuffer(QSize windowSize);

    virtual void cleanup() = 0;
    virtual QVector<GLuint>* getTextureHandle() = 0;

    QSize getRenderSize() const;
    QSize getWindowSize() const;
protected:
    CoffeeFramebufferBaseClass();
    virtual void resizeFramebuffer() = 0;
    virtual void bindFramebuffer(GLuint handle);

    static GLuint allocTexture(int w, int h, GLenum internal, GLenum format,GLenum filtering,GLenum datatype);
    static GLuint allocRenderBuffer(GLenum component, int w, int h);

    bool framebufferActive = true;
    QSize renderSize;
    QSize windowSize;
};

#endif // COFFEEFRAMEBUFFERBASECLASS_H
