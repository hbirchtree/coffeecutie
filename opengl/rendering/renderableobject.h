#ifndef RENDERABLEOBJECT
#define RENDERABLEOBJECT
#include "general/common.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/rendering/coffeevertex.h"
#include "opengl/helpers/shadercontainer.h"
class RenderableObject{
public:
    ~RenderableObject(){

    }

    //General
    virtual glm::vec3 getPosition() = 0;
    virtual glm::vec3 getRotation() = 0;
    virtual glm::vec3 getScale() = 0;

    //Material
    virtual void setTexture(int id,QPointer<CoffeeTexture> texture) = 0;
    virtual QPointer<CoffeeTexture> getTexture(int id) = 0;
    virtual GLint getTextureHandle(int id) = 0;

    virtual glm::vec3 getSpecularColor() = 0;
    virtual glm::vec3 getColorMultiplier() = 0;

    virtual float getShininess() = 0;
    virtual float getTransparency() = 0;

    //Model
    virtual GLint getVaoHandle() = 0;
    virtual GLint getVboHandle() = 0;
    virtual void setVaoHandle(GLint handle) = 0;
    virtual void setVboHandle(GLint handle) = 0;

    virtual int getVertexDataSize() = 0;
    virtual int getVerticesCount() = 0;
    virtual GLfloat* getVertexData() = 0;

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


