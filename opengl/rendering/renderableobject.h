#ifndef RENDERABLEOBJECT
#define RENDERABLEOBJECT
#include <QObject>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <GL/glew.h>
#include "opengl/rendering/coffeevertex.h"
#include "opengl/helpers/shadercontainer.h"
class RenderableObject{
public:
    virtual glm::vec3 getPosition() = 0;
    virtual glm::vec3 getRotation() = 0;
    virtual glm::vec3 getScale() = 0;

    virtual QString getBumpTexture() = 0;
    virtual QString getDiffuseTexture() = 0;
    virtual QString getSpecularTexture() = 0;
    virtual QString getHighlightTexture() = 0;
    virtual QString getTransparencyTexture() = 0;

    virtual GLint getBumpTextureHandle() = 0;
    virtual GLint getDiffuseTextureHandle() = 0;
    virtual GLint getSpecularTextureHandle() = 0;
    virtual GLint getHighlightTextureHandle() = 0;
    virtual GLint getTransparencyTextureHandle() = 0;
    virtual void setBumpTextureHandle(GLint handle) = 0;
    virtual void setDiffuseTextureHandle(GLint handle) = 0;
    virtual void setSpecularTextureHandle(GLint handle) = 0;
    virtual void setHighlightTextureHandle(GLint handle) = 0;
    virtual void setTransparencyTextureHandle(GLint handle) = 0;

    virtual GLint getVaoHandle() = 0;
    virtual GLint getVboHandle() = 0;
    virtual void setVaoHandle(GLint handle) = 0;
    virtual void setVboHandle(GLint handle) = 0;

    virtual glm::vec3 getSpecularColor() = 0;
    virtual glm::vec3 getColorMultiplier() = 0;

    virtual float getShininess() = 0;
    virtual float getTransparency() = 0;

    virtual QPointer<ShaderContainer> getShader() = 0;
    virtual void setShader(QPointer<ShaderContainer> shader) = 0;

    virtual void render() = 0;
    virtual int getVertexDataSize() = 0;
    virtual GLfloat* getVertexData() = 0;

    virtual bool isStreamDraw() = 0;
    virtual bool isDepthTest() = 0;
    virtual bool isDrawn() = 0;
    virtual bool isBaked() = 0;
};

#endif // RENDERABLEOBJECT


