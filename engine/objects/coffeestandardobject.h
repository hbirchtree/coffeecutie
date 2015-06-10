#ifndef COFFEESTANDARDOBJECT_H
#define COFFEESTANDARDOBJECT_H

#include "engine/scripting/qscriptvectorvalue.h"
#include "engine/objects/coffeeobject.h"
#include "opengl/components/coffeetexture.h" //TextureMapping needs to know.
#include "opengl/components/coffeematerial.h"
#include "opengl/components/shadercontainer.h"
#include "engine/models/coffeemesh.h"

class ShaderVariant;

class ShaderMapping {
public:
    QString uniform;
    QPointer<ShaderVariant> data;
    bool constant = false;
};
class TextureMapping {
public:
    bool operator==(const TextureMapping& val){
        return val.texture==this->texture.data()&&
                val.unit==this->unit&&
                val.samplerName==this->samplerName;
    }
    QPointer<CoffeeTexture> texture;
    GLenum unit;
    QString samplerName;
};

class CoffeeStandardObject : public QObject,public CoffeeObject
{
    Q_PROPERTY(QObject* position READ positionValue)
    Q_PROPERTY(QObject* rotation READ rotationValue)
    Q_PROPERTY(QObject* scale READ scaleValue)

    Q_PROPERTY(QObject* material READ materialRef)
    Q_PROPERTY(QObject* shader READ shaderRef)
    Q_PROPERTY(QObject* mesh READ meshRef)

    Q_OBJECT

public:
    CoffeeStandardObject(QObject* parent);
    ~CoffeeStandardObject();

    QString getStringPosition() const;
    QString getStringRotation() const;
    QString getStringScale() const;

    Q_INVOKABLE CoffeeMesh* mesh();
    void setMesh(CoffeeMesh* mesh);
    Q_INVOKABLE ShaderContainer* shader();
    void setShader(ShaderContainer* shader);
    Q_INVOKABLE CoffeeMaterial* material();
    void setMaterial(CoffeeMaterial *mtl);

    QObject* materialRef();
    QObject* meshRef();
    QObject* shaderRef();

    QObject* positionValue();
    QObject* rotationValue();
    QObject* scaleValue();

public slots:
    void render();
    void unload();
    void load();

    void setPosition(float x, float y, float z);
    void setRotation(float x, float y, float z);

    void setUniform(QString uniformName,ShaderVariant* data, bool constant);
    void setTexture(QString samplerName, CoffeeTexture *texture);

protected:
    VectorValue* posWrapper;
    QuaternionValue* rotWrapper;
    VectorValue* sclWrapper;

    bool baked = false;

    QVector<ShaderMapping> uniforms;
    QVector<TextureMapping> textures;

    QPointer<CoffeeMesh> pmesh;
    QPointer<ShaderContainer> pshader;
    QPointer<CoffeeMaterial> pmaterial;
};

#endif // COFFEESTANDARDOBJECT_H
