#ifndef COFFEESTANDARDOBJECT_H
#define COFFEESTANDARDOBJECT_H

#include "engine/objects/coffeeobject.h"
#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/components/shadercontainer.h"
#include "general/shadervariant.h"
class CoffeeStandardObject : public CoffeeObject
{
    Q_PROPERTY(QString position READ getStringPosition)
    Q_PROPERTY(QString rotation READ getStringRotation)
    Q_PROPERTY(QString scale READ getStringScale)

    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)

public:
    CoffeeStandardObject(QObject* parent);

    void render();
    void unload();
    void load();

    QString getStringPosition() const;
    QString getStringRotation() const;
    QString getStringScale() const;

    CoffeeMesh* mesh();
    void setMesh(CoffeeMesh* mesh);
    ShaderContainer* shader();
    void setShader(ShaderContainer* shader);
    CoffeeMaterial* material();
    void setMaterial(CoffeeMaterial *mtl);

    void setUniform(QString uniformName,ShaderVariant* data, bool constant);
    void setTexture(QString samplerName, CoffeeTexture *texture);

protected:
    bool baked = false;

    class ShaderMapping {
    public:
        QString uniform;
        QPointer<ShaderVariant> data;
        bool constant = false;
    };
    class TextureMapping {
    public:
        bool operator==(const TextureMapping& val){
            return val.texture==this->texture&&
                    val.unit==this->unit&&
                    val.samplerName==this->samplerName;
        }
        QPointer<CoffeeTexture> texture;
        GLenum unit;
        QString samplerName;
    };

    QVector<ShaderMapping> uniforms;
    QVector<TextureMapping> textures;

    QPointer<CoffeeMesh> pmesh;
    QPointer<ShaderContainer> pshader;
    QPointer<CoffeeMaterial> pmaterial;
};

#endif // COFFEESTANDARDOBJECT_H
