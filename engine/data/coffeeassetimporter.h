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

class CoffeeModelStruct :public QObject{
    Q_PROPERTY(QObject* mesh READ getMeshObject)
    Q_PROPERTY(QObject* material READ getMaterialObject)

    Q_OBJECT
public:
    CoffeeModelStruct(QObject* parent);
    CoffeeMesh* mesh;
    CoffeeMaterial* material;

    QObject* getMeshObject();
    QObject* getMaterialObject();
};

class CoffeeAssetStorage : public QObject
{
    Q_PROPERTY(QObjectList worlds READ getWorlds)
    Q_PROPERTY(QObjectList shaders READ getShaders)

    Q_OBJECT

public:
    CoffeeAssetStorage(QObject* parent);

    QString filepath;
    QHash<QString,QHash<QString,CoffeeModelStruct*> > models;
    QHash<QString,QHash<QString,QPointer<CoffeeMesh>> > meshes;
    QHash<QString,QHash<QString,QPointer<CoffeeMaterial>> > materials;

    QList<CoffeeWorldOpts*> worlds;
    QHash<QString,QPointer<ShaderContainer>> shaders;
    QHash<QString,QPointer<CoffeeTexture>> textures;

    void merge(CoffeeAssetStorage* data);
    CoffeeModelStruct *acquireModel(QString identification);

    void setParents(QObject* parent);

    Q_INVOKABLE QObject* getMesh(QString source);
    Q_INVOKABLE QObject* getMaterial(QString source);
    Q_INVOKABLE QObject* getModel(QString source);

    QObjectList getWorlds();

    QObjectList getShaders();
public slots:
};

class CoffeeAssetImporter : public QObject
{
    Q_OBJECT
public:
    CoffeeAssetImporter(QObject *parent,QObject* outputParent);

    CoffeeAssetStorage* importTexture(const QVariantMap &data, const QString &filepath);
    CoffeeAssetStorage* importShader(const QVariantMap &data, const QString &filepath);
    CoffeeAssetStorage* importModel(const QVariantMap &data, const QString &filepath);

    CoffeeAssetStorage* importAssets(QVariantList assetList, const QString &filepath);

private:
    QObject* outputParent;
};

#endif // COFFEEASSETIMPORTER_H
