#ifndef COFFEEOBJECTFACTORY_H
#define COFFEEOBJECTFACTORY_H

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

class CoffeeObjectFactory
{
public:
    CoffeeObjectFactory();
    QList<CoffeeWorldOpts *> importObjects(QString file, QObject *parent);
    CoffeeObject *createObject(const QVariantMap &data, QObject *parent);

    void importModel(Assimp::Importer &importer, const QVariantMap &data, QObject *parent);
    void importTexture(const QVariantMap &data, QObject* parent);
    void importShader(const QVariantMap &data, QObject* parent);

    CoffeeWorldOpts* createWorld(const QString &key, const QVariantMap &data, QObject* parent);
    CoffeeCamera* createCamera(const QVariantMap &data, QObject* parent);
    CoffeeOmniLight* createLight(const QVariantMap &data, QObject* parent);
    CoffeeSkybox* createSkybox(const QVariantMap &data, QObject* parent);
    CoffeeParticleSystem* createParticleSystem(const QVariantMap &data, QObject* parent);

private:
    class CoffeeModelStruct {
    public:
        CoffeeMesh* mesh;
        CoffeeMaterial* material;
    };

    CoffeeModelStruct *acquireModel(QString identification);

    static glm::vec3 listToVec3(const QVariant &data);
    static glm::vec4 listToVec4(const QVariant &data);
    static QColor stringToColor(const QVariant &data);

    QString filepath;
    //We store the collective mesh and material as well as separate meshes and materials
    QHash<QString,QHash<QString,CoffeeModelStruct*> > models;
    QHash<QString,QHash<QString,QPointer<CoffeeMesh>> > meshes;
    QHash<QString,QHash<QString,QPointer<CoffeeMaterial>> > materials;
    QList<CoffeeWorldOpts*> worlds;

    QHash<QString,QPointer<ShaderContainer>> shaders;
    QHash<QString,QPointer<CoffeeTexture>> textures;
};

#endif // COFFEEOBJECTFACTORY_H
