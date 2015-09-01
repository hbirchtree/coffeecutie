#ifndef COFFEEFRAMEBUFFEROBJECT_H
#define COFFEEFRAMEBUFFEROBJECT_H

#include "opengl/components/framebuffers/coffeeframebufferbaseclass.h"

class CoffeeFramebufferObject : public CoffeeFramebufferBaseClass
{
    Q_OBJECT
public:
    CoffeeFramebufferObject(QObject *parent);

    void createFramebuffer(QSize renderSize);
    void resize(QSize renderSize);

    void cleanup();

    QVector<GLuint>* getTextureHandle();

    void bindFramebuffer();

    void bindFramebufferRead();
    void bindFramebufferWrite();

    bool ready() const;

public slots:
    void setNumTextures(uint textures);

protected:
    class DrawBuffer{
        GLuint texture;
        GLenum attachment;
    };

    uint numTextures = 1;
    void resizeFramebuffer();
    QVector<DrawBuffer*> buffers;
    QVector<GLuint> textureHandles;
    GLuint framebufferHandle = 0;
};

#endif // COFFEEFRAMEBUFFEROBJECT_H
