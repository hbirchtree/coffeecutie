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

    Q_INVOKABLE QObject* importAssets(QString src); //Short, used for scripting
    CoffeeAssetStorage *importAssets(CoffeeResource *src,
                                     QObject *parent);
    QList<CoffeeWorldOpts*> importObjects(QString file,
                                          QObject *parent);

    static glm::vec3 varListToVec3(const QVariantList &d);
    static glm::quat varListToQuat(const QVariantList &d);

private:
    static CoffeeWorldOpts* createWorld(const QString &key,
                                        const QVariantMap &data,
                                        CoffeeAssetStorage *assets,
                                        QObject* parent);
    static CoffeeCamera* createCamera(const QVariantMap &data,
                                      QObject* parent);
    static CoffeeOmniLight* createLight(const QVariantMap &data,
                                        QObject* parent);
    static CoffeeSkybox* createSkybox(const QVariantMap &data,
                                      CoffeeAssetStorage *assets,
                                      QObject* parent);
    static CoffeeParticleSystem* createParticleSystem(const QVariantMap &data,
                                                      CoffeeAssetStorage *assets,
                                                      QObject* parent);
    static CoffeeObject *createObject(const QVariantMap &data,
                                      CoffeeAssetStorage *assets,
                                      QObject *parent);

    static glm::vec3 listToVec3(const QVariant &data);
    static glm::vec4 listToVec4(const QVariant &data);
    static QColor stringToColor(const QVariant &data);
};

#endif // COFFEEOBJECTFACTORY_H
