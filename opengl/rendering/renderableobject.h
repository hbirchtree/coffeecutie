#ifndef RENDERABLEOBJECT
#define RENDERABLEOBJECT
#include "general/common.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/rendering/coffeevertex.h"
#include "opengl/helpers/shadercontainer.h"
#include "general/vectors/vector3container.h"
class RenderableObject{
public:
    ~RenderableObject(){}

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

    QString toString(){
        return QString("GMY: pos:%1 "
                       "rot:%2 "
                       "scl:%3 "
                       "MAT: aph:%4 "
                       "shn:%5 "
                       "tex:%10 "
                       "col:%12 "
                       "OGL: str:%6 "
                       "bkd:%7 "
                       "drw:%8 "
                       "dpt:%9 "
                       "DAT: vao:%11 ")
                .arg(Vector3Container::vec3toString(getPosition()))
                .arg(Vector3Container::vec3toString(getRotation()))
                .arg(Vector3Container::vec3toString(getScale()))
                .arg(getTransparency())
                .arg(getShininess())
                .arg(isStreamDraw())
                .arg(isBaked())
                .arg(isDrawn())
                .arg(isDepthTest())
                .arg(getTextureHandle(CoffeeTexture::Texture_Diffusion))
                .arg(getVaoHandle())
                .arg(Vector3Container::vec3toString(getSpecularColor()));
    }
};

#endif // RENDERABLEOBJECT


