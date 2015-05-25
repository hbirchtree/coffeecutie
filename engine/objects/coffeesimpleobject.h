#ifndef COFFEESIMPLEOBJECT_H
#define COFFEESIMPLEOBJECT_H

#include "general/common.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/components/coffeematerial.h"
#include "general/data/numberbuffer.h"
#include "opengl/components/shadercontainer.h"
#include "engine/rendering/coffeerenderingmethod.h"
class CoffeeSimpleObject : public QObject,public RenderableObject
{
    Q_PROPERTY(QString position READ getStringPosition)
    Q_PROPERTY(QString rotation READ getStringRotation)
    Q_PROPERTY(QString scale READ getStringScale)

public:
    QString getStringPosition() const{
        return QStringFunctions::toString(pos);
    }
    QString getStringRotation() const{
        return QStringFunctions::toString(rotation);
    }
    QString getStringScale() const{
        return QStringFunctions::toString(scale);
    }
    virtual void render();
    virtual glm::vec3 getPosition() const;
    virtual glm::quat getRotation() const;
    virtual glm::vec3 getScale() const;
    virtual QPointer<CoffeeMaterial> getMaterial();
    virtual GLint getVaoHandle();
    virtual GLint getVboHandle();
    virtual void setVaoHandle(GLint handle);
    virtual void setVboHandle(GLint handle);
    virtual int getVertexDataSize();
    virtual int getVerticesCount();
    virtual NumberBuffer<GLfloat> *getVertexData();
    virtual bool isStreamDraw();
    virtual bool isDepthTest();
    virtual bool isDrawn();
    virtual bool isBaked();
    virtual void setBaked(bool val);
    virtual QPointer<ShaderContainer> getShader();
    virtual void setShader(QPointer<ShaderContainer> shader);
    virtual void unloadAssets();

    virtual void setMdl(QPointer<NumberBuffer<GLfloat> > value);

protected:
    CoffeeSimpleObject(QObject *parent);

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
    glm::quat rotation;
};

#endif // COFFEESIMPLEOBJECT_H
