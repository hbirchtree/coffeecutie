#ifndef COFFEEOUTPUTSURFACE_H
#define COFFEEOUTPUTSURFACE_H

#include "engine/objects/coffeesimpleobject.h"
#include "opengl/components/coffeeframebufferobject.h"
class CoffeeOutputSurface : public CoffeeSimpleObject
{
    Q_OBJECT
public:
    CoffeeOutputSurface(QObject *parent, CoffeeFrameBufferObject *display);

    void render();
    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    glm::vec3 getScale() const;
    QPointer<CoffeeMaterial> getMaterial();
    GLint getVaoHandle();
    GLint getVboHandle();
    void setVaoHandle(GLint handle);
    void setVboHandle(GLint handle);
    int getVertexDataSize();
    int getVerticesCount();
    NumberBuffer<GLfloat> *getVertexData();
    bool isStreamDraw();
    bool isDepthTest();
    bool isDrawn();
    bool isBaked();
    void setBaked(bool val);
    QPointer<ShaderContainer> getShader();
    void setShader(QPointer<ShaderContainer> shader);
    void unloadAssets();

    void setMdl(QPointer<NumberBuffer<GLfloat> > value);
};

#endif // COFFEEOUTPUTSURFACE_H
