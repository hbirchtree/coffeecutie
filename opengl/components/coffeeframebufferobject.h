#ifndef COFFEEFRAMEBUFFEROBJECT_H
#define COFFEEFRAMEBUFFEROBJECT_H

#include "opengl/components/framebuffers/coffeeframebufferbaseclass.h"

class CoffeeFramebufferObject : public CoffeeFramebufferBaseClass
{
    Q_PROPERTY(uint sampling READ getSampling WRITE updateSampling)
    Q_PROPERTY(bool ready READ ready WRITE setReady)

    Q_OBJECT
public:
    CoffeeFramebufferObject(QObject *parent);

    void createFramebuffer(QSize windowSize, uint sampling);
    void setSize(QSize windowSize, uint sampling);

    void cleanup();

    uint getSampling() const;
    QVector<GLuint>* getTextureHandle();

    void bindFramebuffer();

    void bindFramebufferRead();
    void bindFramebufferWrite();

    void unbindFramebufferRead();
    void unbindFramebufferWrite();

    bool ready() const;

public slots:
    void setNumTextures(uint textures);
    void resizeViewport(QSize windowSize);
    void updateSampling(uint sampling);
    void setReady(bool ready);

protected:
    class DrawBuffer{
        GLuint texture;
        GLenum attachment;
    };

    bool m_resized = false;

    uint numTextures = 1;
    void resizeFramebuffer();
    uint sampling = 1;
    QVector<DrawBuffer*> buffers;
    QVector<GLuint> textureHandles;
    GLuint framebufferHandle = 0;
    bool m_ready;
};

#endif // COFFEEFRAMEBUFFEROBJECT_H
