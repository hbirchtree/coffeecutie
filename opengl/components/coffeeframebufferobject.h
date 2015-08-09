#ifndef COFFEEFRAMEBUFFEROBJECT_H
#define COFFEEFRAMEBUFFEROBJECT_H

#include "opengl/components/framebuffers/coffeeframebufferbaseclass.h"

class CoffeeFramebufferObject : public CoffeeFramebufferBaseClass
{
    Q_PROPERTY(bool ready READ ready WRITE setReady)

    Q_OBJECT
public:
    CoffeeFramebufferObject(QObject *parent);

    void createFramebuffer(QSize windowSize, uint sampling);
    void setSize(QSize windowSize, uint sampling);

    void cleanup();

    QVector<GLuint>* getTextureHandle();

    void bindFramebuffer();

    void bindFramebufferRead();
    void bindFramebufferWrite();

    bool ready() const;

public slots:
    void setNumTextures(uint textures);
    void setReady(bool ready);

protected:
    class DrawBuffer{
        GLuint texture;
        GLenum attachment;
    };

    bool m_resized = false;

    uint numTextures = 1;
    void resizeFramebuffer();
    QVector<DrawBuffer*> buffers;
    QVector<GLuint> textureHandles;
    GLuint framebufferHandle = 0;
    bool m_ready = false;
};

#endif // COFFEEFRAMEBUFFEROBJECT_H
