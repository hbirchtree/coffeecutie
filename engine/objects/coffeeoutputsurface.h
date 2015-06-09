#ifndef COFFEEOUTPUTSURFACE_H
#define COFFEEOUTPUTSURFACE_H

#include "coffeeobject.h"

class CoffeeFrameBufferObject;
class ShaderContainer;
class CoffeeTexture;

class CoffeeOutputSurface : public QObject,public CoffeeObject
{
    Q_OBJECT
public:
    CoffeeOutputSurface(QObject *parent, CoffeeFrameBufferObject *display);

    void setFramebuffer(CoffeeFrameBufferObject *display);

    bool isBaked() const;

public slots:
    void load();
    void render();
    void unload();

protected:
    void setBaked(bool val);
    bool baked = false;
    QPointer<ShaderContainer> shader;
    GLuint vao;
    GLuint vbo;
    CoffeeFrameBufferObject* framebuffer;
};

#endif // COFFEEOUTPUTSURFACE_H
