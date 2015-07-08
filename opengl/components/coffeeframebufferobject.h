#ifndef COFFEEFRAMEBUFFEROBJECT_H
#define COFFEEFRAMEBUFFEROBJECT_H

#include "opengl/components/framebuffers/coffeeframebufferbaseclass.h"

class CoffeeFrameBufferObject : public QObject, public CoffeeFramebufferBaseClass
{
    Q_PROPERTY(QSize renderSize READ getRenderSize)
    Q_PROPERTY(QSize windowSize READ getWindowSize)
    Q_PROPERTY(uint sampling READ getSampling WRITE updateSampling)

    Q_OBJECT
public:
    CoffeeFrameBufferObject(QObject *parent);

    void createFramebuffer(QSize windowSize, uint sampling);
    void bindFramebufferRead();
    void bindFramebufferWrite();
    void unbindFramebufferRead();
    void unbindFramebufferWrite();

    void cleanup();

    using CoffeeFramebufferBaseClass::getRenderSize;
    using CoffeeFramebufferBaseClass::getWindowSize;

    uint getSampling() const;
    QVector<GLuint>* getTextureHandle();
    void bindFramebuffer();

public slots:
    void setNumTextures(uint textures);
    void resizeViewport(QSize windowSize);
    void updateSampling(uint sampling);

protected:
    class DrawBuffer{
        GLuint texture;
        GLenum attachment;
    };

    uint numTextures = 1;
    void resizeFramebuffer();
    uint sampling = 1;
    QVector<DrawBuffer*> buffers;
    QVector<GLuint> textureHandles;
    GLuint framebufferHandle;
};

#endif // COFFEEFRAMEBUFFEROBJECT_H
