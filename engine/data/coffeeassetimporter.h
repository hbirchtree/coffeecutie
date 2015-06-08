#ifndef COFFEEASSETIMPORTER_H
#define COFFEEASSETIMPORTER_H

#include "general/common.h"

class CoffeeWorldOpts;
class CoffeeCamera;
class CoffeeOmniLight;
class CoffeeSkybox;
class CoffeeMesh;
class CoffeeMaterial;
class ShaderContainer;
class CoffeeTexture;
class CoffeeObject;
class CoffeeParticleSystem;

class CoffeeModelStruct {
public:
    CoffeeMesh* mesh;
    CoffeeMaterial* material;
};

class CoffeeAssetStorage {
public:
    QString filepath;
    QHash<QString,QHash<QString,CoffeeModelStruct*> > models;
    QHash<QString,QHash<QString,QPointer<CoffeeMesh>> > meshes;
    QHash<QString,QHash<QString,QPointer<CoffeeMaterial>> > materials;

    QList<CoffeeWorldOpts*> worlds;
    QHash<QString,QPointer<ShaderContainer>> shaders;
    QHash<QString,QPointer<CoffeeTexture>> textures;

    void merge(const CoffeeAssetStorage &data);
    CoffeeModelStruct *acquireModel(QString identification);

    void setParents(QObject* parent);
};

class CoffeeAssetImporter : public QObject
{
    Q_OBJECT
public:
    CoffeeAssetImporter(QObject *parent);

    CoffeeAssetStorage importTexture(const QVariantMap &data, const QString &filepath);
    CoffeeAssetStorage importShader(const QVariantMap &data, const QString &filepath);
    CoffeeAssetStorage importModel(const QVariantMap &data, const QString &filepath);

    CoffeeAssetStorage importAssets(QVariantList assetList, const QString &filepath);
};

#endif // COFFEEASSETIMPORTER_H
