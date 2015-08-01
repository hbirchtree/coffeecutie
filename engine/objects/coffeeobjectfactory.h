#ifndef COFFEEOBJECTFACTORY_H
#define COFFEEOBJECTFACTORY_H

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

class CoffeeAssetStorage;

class CoffeeObjectFactory : public QObject
{
    Q_OBJECT
public:
    CoffeeObjectFactory(QObject* parent);

    Q_INVOKABLE QObject* importAssets(QObject *parent, const QVariantMap& source, const QString &srcFile);
    Q_INVOKABLE QObject* importAssetsFile(QString src); //Short, used for scripting

    CoffeeAssetStorage* importAssets__base(QObject *parent, const QVariantMap& source, const QString &srcFile);
    CoffeeAssetStorage *importAssets(CoffeeResource *src,
                                     QObject *parent);

    QList<CoffeeWorldOpts*> importObjects(QString file,
                                          QObject *parent);

};

#endif // COFFEEOBJECTFACTORY_H
