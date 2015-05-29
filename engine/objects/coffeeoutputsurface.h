#ifndef COFFEEOUTPUTSURFACE_H
#define COFFEEOUTPUTSURFACE_H

#include "engine/objects/coffeeobject.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "general/data/numberbuffer.h"
#include "opengl/components/coffeeframebufferobject.h"
class CoffeeOutputSurface : public CoffeeObject
{
    Q_OBJECT
    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)
public:
    CoffeeOutputSurface();
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
    NumberBuffer<GLfloat> *mdl;
    GLuint vao;
    GLuint vbo;
    CoffeeFrameBufferObject* framebuffer;
};

#endif // COFFEEOUTPUTSURFACE_H
