#ifndef COFFEEFRAMEBUFFEROBJECT_H
#define COFFEEFRAMEBUFFEROBJECT_H

#include "general/common.h"
class CoffeeFrameBufferObject : public QObject
{
    Q_OBJECT
public:
    CoffeeFrameBufferObject(QObject *parent);

    void createFramebuffer();

    void bindFramebuffer();
    void unbindFramebuffer();

    GLuint getTextureHandle() const;

private:
    static GLuint allocTexture(int w, int h, GLenum internal, GLenum format);
    static GLuint allocRenderBuffer(GLenum component, int w, int h);

    GLuint framebufferHandle;
    GLuint textureHandle;
};

#endif // COFFEEFRAMEBUFFEROBJECT_H
