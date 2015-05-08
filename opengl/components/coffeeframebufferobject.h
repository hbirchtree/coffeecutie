#ifndef COFFEEFRAMEBUFFEROBJECT_H
#define COFFEEFRAMEBUFFEROBJECT_H

#include "general/common.h"
#include <QList>
class CoffeeFrameBufferObject : public QObject
{
    Q_OBJECT
public:
    CoffeeFrameBufferObject(QObject *parent);

    void cleanup();
    void createFramebuffer(QSize windowSize, uint sampling);

    void bindFramebuffer();
    void unbindFramebuffer();

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
