#ifndef RENDERABLEOBJECT
#define RENDERABLEOBJECT
#include "general/common.h"

class CoffeeMaterial;
class ShaderContainer;
template <class T> class NumberBuffer;

class RenderableObject{
public:
    ~RenderableObject(){}

    //General
    virtual glm::vec3 getPosition() const = 0;
    virtual glm::quat getRotation() const = 0;
    virtual glm::vec3 getScale() const = 0;

    //Material
    virtual QPointer<CoffeeMaterial> getMaterial() = 0;

    //Model
    virtual GLint getVaoHandle() = 0;
    virtual GLint getVboHandle() = 0;
    virtual void setVaoHandle(GLint handle) = 0;
    virtual void setVboHandle(GLint handle) = 0;

    virtual int getVertexDataSize() = 0;
    virtual int getVerticesCount() = 0;
    virtual NumberBuffer<GLfloat>* getVertexData() = 0;

    virtual bool isStreamDraw() = 0;
    virtual bool isDepthTest() = 0;
    virtual bool isDrawn() = 0;
    virtual bool isBaked() = 0;
    virtual void setBaked(bool val) = 0;

    //OpenGL
    virtual QPointer<ShaderContainer> getShader() = 0;
    virtual void setShader(QPointer<ShaderContainer> shader) = 0;

    virtual void unloadAssets() = 0;
};

#endif // RENDERABLEOBJECT


