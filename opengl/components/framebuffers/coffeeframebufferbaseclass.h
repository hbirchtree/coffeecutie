#ifndef COFFEEFRAMEBUFFERBASECLASS_H
#define COFFEEFRAMEBUFFERBASECLASS_H

#include "general/common.h"

#include <QSize>

class CoffeeFramebufferBaseClass : public QObject
{
    Q_PROPERTY(QSize renderSize READ getRenderSize)
    Q_PROPERTY(QSize windowSize READ getWindowSize)

    Q_OBJECT
public:
    virtual void bindFramebuffer() = 0;
    virtual void unbindFramebuffer();

    virtual void bindFramebufferRead() = 0;
    virtual void bindFramebufferWrite() = 0;

    virtual void unbindFramebufferRead() = 0;
    virtual void unbindFramebufferWrite() = 0;

    virtual void createFramebuffer(QSize windowSize);

    virtual void cleanup() = 0;
    virtual QVector<GLuint>* getTextureHandle() = 0;

    QSize getRenderSize() const;
    QSize getWindowSize() const;
protected:
    CoffeeFramebufferBaseClass(QObject* parent);

    virtual void resizeFramebuffer() = 0;
    virtual void bindFramebuffer(GLuint handle);

    static GLuint allocTexture(int w, int h,
                               GLenum internal, GLenum format,
                               GLenum filtering,GLenum datatype);
    static GLuint allocRenderBuffer(GLenum component,
                                    int w, int h);

    bool framebufferActive = true;
    QSize renderSize;
    QSize windowSize;

    bool m_viewport_set = true;
};

#endif // COFFEEFRAMEBUFFERBASECLASS_H
