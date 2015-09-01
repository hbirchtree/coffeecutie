#ifndef COFFEEASSETIMPORTER_H
#define COFFEEASSETIMPORTER_H

#include "general/common.h"

class CoffeeWorldOpts;
class CoffeeCamera;
class CoffeeOmniLight;
class CoffeeSkybox;
class CoffeeMesh;
class CoffeeMaterial;
class CoffeeShader;
class CoffeeTexture;
class CoffeeObject;
class CoffeeParticleSystem;
class CoffeeResource;

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
    Q_PROPERTY(QVariantMap shaders READ getShaders)
    Q_PROPERTY(QObjectList textures READ getTextures)

    Q_PROPERTY(QObjectList resources READ getResources)

    Q_OBJECT

public:
    CoffeeAssetStorage(QObject* parent);

    QString filepath;
    QHash<QString,QHash<QString,CoffeeModelStruct*> > models;
    QHash<QString,QHash<QString,QPointer<CoffeeMesh>> > meshes;
    QHash<QString,QHash<QString,QPointer<CoffeeMaterial>> > materials;
    QList<QPointer<CoffeeResource>> resources;

    QList<CoffeeWorldOpts*> worlds;
    QHash<QString,QPointer<CoffeeShader>> shaders;
    QHash<QString,QPointer<CoffeeTexture>> textures;

    void merge(CoffeeAssetStorage* data);
    CoffeeModelStruct *acquireModel(QString identification);

    Q_INVOKABLE void setParents(QObject* parent);

    Q_INVOKABLE QObject* getMesh(QString source);
    Q_INVOKABLE QObject* getMaterial(QString source);
    Q_INVOKABLE QObject* getModel(QString source);

    QObjectList getWorlds();
    QVariantMap getShaders();
    QObjectList getTextures();
    QObjectList getResources();
};

class CoffeeAssetImporter : public QObject
{
    Q_PROPERTY(bool verbose READ verbose WRITE setVerbose)

    Q_OBJECT
public:
    CoffeeAssetImporter(QObject *parent,QObject* outputParent);

    CoffeeAssetStorage* importTexture(const QVariantMap &data, const QString &filepath);
    CoffeeAssetStorage* importShader(const QVariantMap &data, const QString &filepath);
    CoffeeAssetStorage* importModel(const QVariantMap &data, const QString &filepath);

    CoffeeAssetStorage* importAssets(QVariantList assetList, const QString &filepath);

    bool verbose() const;

public slots:
    void setVerbose(bool verbose);

private:
    QObject* outputParent;
    bool m_verbose = false;
};



#endif // COFFEEASSETIMPORTER_H
