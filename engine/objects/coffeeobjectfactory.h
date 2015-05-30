#ifndef COFFEEOBJECTFACTORY_H
#define COFFEEOBJECTFACTORY_H

#include "general/common.h"
#include "engine/objects/coffeestandardobject.h"
#ifdef COFFEE_USE_HORRIBLE_OBJ_IMPORTER
#include "general/models/wavefrontmodelreader.h"
#endif
#include "engine/rendering/coffeerenderingmethod.h"
#include "opengl/components/coffeeworldopts.h"
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QColor>
class CoffeeObjectFactory
{
public:
    CoffeeObjectFactory();
    QList<CoffeeWorldOpts *> importObjects(QString file, QObject *parent);
    CoffeeObject *createObject(const QVariantMap &data, QObject *parent);

    void importModels(const QVariantMap &data, QObject *parent);

    CoffeeWorldOpts* createWorld(const QString &key, const QVariantMap &data, QObject* parent);
    CoffeeCamera* createCamera(const QVariantMap &data, QObject* parent);
    CoffeeOmniLight* createLight(const QVariantMap &data, QObject* parent);

private:
    class CoffeeModelStruct {
    public:
        CoffeeMesh* mesh;
        CoffeeMaterial* material;
    };

    static glm::vec3 listToVec3(const QVariant &data);
    static glm::vec4 listToVec4(const QVariant &data);
    static QColor stringToColor(const QVariant &data);

    QString filepath;
#ifdef COFFEE_USE_HORRIBLE_OBJ_IMPORTER
    QHash<QString,QHash<QString,QPointer<WavefrontModelReader::ModelContainer>>> models;
#else
    //We store the collective mesh and material as well as separate meshes and materials
    QHash<QString,QHash<QString,CoffeeModelStruct> > models;
    QHash<QString,QHash<QString,QPointer<CoffeeMesh>> > meshes;
    QHash<QString,QHash<QString,QPointer<CoffeeMaterial>> > materials;
#endif
    QList<CoffeeWorldOpts*> worlds;
};

#endif // COFFEEOBJECTFACTORY_H
