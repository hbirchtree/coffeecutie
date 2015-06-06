#ifndef COFFEESTANDARDOBJECT_H
#define COFFEESTANDARDOBJECT_H

#include "engine/objects/coffeeobject.h"

class ShaderVariant;
class ShaderContainer;
class CoffeeMaterial;
class CoffeeMesh;
class CoffeeTexture;

class CoffeeStandardObject : public CoffeeObject
{
    Q_PROPERTY(QString position READ getStringPosition)
    Q_PROPERTY(QString rotation READ getStringRotation)
    Q_PROPERTY(QString scale READ getStringScale)

    Q_INTERFACES(CoffeeObject)
    Q_PLUGIN_METADATA(IID CoffeeObjectIID)

public:
    CoffeeStandardObject(QObject* parent);
    ~CoffeeStandardObject();

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
