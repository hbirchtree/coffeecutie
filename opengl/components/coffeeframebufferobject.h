#ifndef COFFEEFRAMEBUFFEROBJECT_H
#define COFFEEFRAMEBUFFEROBJECT_H

#include "general/common.h"
class CoffeeFrameBufferObject : public QObject
{
    Q_PROPERTY(QSize renderSize READ getRenderSize)
    Q_PROPERTY(QSize windowSize READ getWindowSize)
    Q_PROPERTY(uint sampling READ getSampling)

    Q_OBJECT
public:
    CoffeeFrameBufferObject(QObject *parent);

    void cleanup();
    void createFramebuffer(QSize windowSize, uint sampling);

    QSize getRenderSize() const;
    QSize getWindowSize() const;
    uint getSampling() const;

    void bindFramebuffer();
    void bindFramebufferRead();
    void bindFramebufferWrite();
    void unbindFramebuffer();
    void unbindFramebufferRead();
    void unbindFramebufferWrite();

    GLuint getTextureHandle() const;

public slots:
    void resizeViewport(QSize windowSize);
    void updateSampling(uint sampling);

protected:

    virtual void resizeFramebuffer();
    bool framebufferActive = false;
    QSize renderSize;
    QSize windowSize;
    uint sampling = 1;
    static GLuint allocTexture(int w, int h, GLenum internal, GLenum format,GLenum filtering,GLenum datatype);
    static GLuint allocRenderBuffer(GLenum component, int w, int h);

    QList<GLuint> renderbuffers;
    GLuint framebufferHandle;
    GLuint textureHandle;
};

#endif // COFFEEFRAMEBUFFEROBJECT_H
