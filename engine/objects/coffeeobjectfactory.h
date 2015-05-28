#ifndef COFFEEOBJECTFACTORY_H
#define COFFEEOBJECTFACTORY_H

#include "general/common.h"
#include "engine/objects/coffeeobject.h"
#include "general/models/wavefrontmodelreader.h"
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
    static glm::vec3 listToVec3(const QVariant &data);
    static glm::vec4 listToVec4(const QVariant &data);
    static QColor stringToColor(const QVariant &data);

    QString filepath;
    QHash<QString,QHash<QString,QPointer<WavefrontModelReader::ModelContainer>>> models;
    QList<CoffeeWorldOpts*> worlds;
};

#endif // COFFEEOBJECTFACTORY_H
