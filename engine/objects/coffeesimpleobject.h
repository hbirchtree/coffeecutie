#ifndef COFFEESIMPLEOBJECT_H
#define COFFEESIMPLEOBJECT_H

#include "general/common.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/components/coffeematerial.h"
#include "general/data/numberbuffer.h"
#include "opengl/components/shadercontainer.h"
#include "engine/rendering/coffeerenderingmethod.h"
#include "opengl/components/coffeeframebufferobject.h"
class CoffeeSimpleObject : public QObject, public RenderableObject
{
    Q_OBJECT
public:
    CoffeeSimpleObject(QObject *parent, CoffeeFrameBufferObject *display);

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

private:
    bool baked = false;
    GLuint vao;
    GLuint vbo;
    GLuint texture;
    QPointer<CoffeeRenderingMethod> renderer;
    QPointer<CoffeeMaterial> mtl;
    QPointer<ShaderContainer> shader;
    QPointer<NumberBuffer<GLfloat>> mdl;
    glm::vec3 pos;
    glm::vec3 scale;
};

#endif // COFFEESIMPLEOBJECT_H
