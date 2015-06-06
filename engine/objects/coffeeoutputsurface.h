#ifndef COFFEEOUTPUTSURFACE_H
#define COFFEEOUTPUTSURFACE_H

#include "engine/objects/coffeeobject.h"

class CoffeeFrameBufferObject;
class ShaderContainer;
class CoffeeTexture;

class CoffeeOutputSurface : public CoffeeObject
{
    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)
public:
    CoffeeOutputSurface(QObject *parent, CoffeeFrameBufferObject *display);

    void setFramebuffer(CoffeeFrameBufferObject *display);

    void load();
    void render();
    void unload();

    bool isBaked() const;

protected:
    void setBaked(bool val);
    bool baked = false;
    QPointer<ShaderContainer> shader;
    GLuint vao;
    GLuint vbo;
    CoffeeFrameBufferObject* framebuffer;
};

#endif // COFFEEOUTPUTSURFACE_H
