#include "coffeeobjectfactory.h"

#include "general/filehandler.h"
#include "engine/objects/coffeestandardobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "engine/objects/coffeeobject.h"
#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeeomnilight.h"
#include "engine/objects/coffeestandardobject.h"
#include "engine/objects/coffeeskybox.h"
#include "engine/objects/coffeeparticlesystem.h"
#include "opengl/components/coffeecamera.h"
#include "engine/data/coffeeresource.h"

#include "engine/physics/physicsdescriptor.h"
#include "engine/data/coffeeassetimporter.h"
#include "engine/compute/coffeetransformcomputer.h"

#include <QVariantMap>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QColor>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QEventLoop>

CoffeeObjectFactory::CoffeeObjectFactory(QObject *parent) : QObject(parent){}

QObject *CoffeeObjectFactory::importAssets(QObject* parent, const QVariantMap &source, const QString &srcFile)
{
    return importAssets__base(parent,source,srcFile);
}

QObject *CoffeeObjectFactory::importAssetsFile(QString src)
{
    CoffeeResource *f = new CoffeeResource(0,src);
    CoffeeAssetStorage* s = importAssets(f,this->parent());
    delete f;
    return s;
}

CoffeeAssetStorage *CoffeeObjectFactory::importAssets__base(QObject *parent, const QVariantMap &source, const QString &srcFile)
{
    QElapsedTimer t;
    t.start();
    CoffeeAssetStorage *s = new CoffeeAssetStorage(0);
    s->moveToThread(parent->thread());

    if(source.isEmpty()){
        qDebug("Object file was empty or non-existing!");
        return s;
    }

    QFileInfo f(srcFile);
    s->filepath = f.path()+QDir::separator();

    //The asset-based system allows us to load a resource once and reuse it infinitely from the bottom up.
    //We will need to create a system later on that determines which resources should be unloaded and which should not.
    //We will most likely use the CoffeeGameAsset class for this (pre-alloc, alloc, and release as states)

    QObject* importerRoot = new QObject();
    QList<QFuture<CoffeeAssetStorage*>> importData;

    for(QString key : source.keys())
        if(key=="assets"){
            CoffeeAssetImporter* imp = new CoffeeAssetImporter(importerRoot,parent); //temporary object
            imp->setVerbose(this->verbose());
            QFuture<CoffeeAssetStorage*> index =
                    QtConcurrent::run(imp,
                                      &CoffeeAssetImporter::importAssets,
                                      source.value(key).toList(),
                                      s->filepath);
            importData.append(index);
        }

    for(QFuture<CoffeeAssetStorage*> e : importData){
        CoffeeAssetStorage* p = e.result();
        s->merge(p);
        delete p;
    }

    importerRoot->deleteLater();

    s->setParents(parent);

    qDebug("Assets: %i shader(s), %i model(s), %i mesh(es), %i material(s), %i texture(s)",
           s->shaders.size(),
           s->models.size(),
           s->meshes.size(),
           s->materials.size(),
           s->textures.size());

    //World data
//    for(QString key : source.keys()){
//        if(key.startsWith("world."))
//            s->worlds.append(createWorld(key.mid(6),source.value(key).toMap(),s,parent));
//    }
    qDebug("Spent %i milliseconds parsing content from disk",t.elapsed());
    return s;
}

CoffeeAssetStorage* CoffeeObjectFactory::importAssets(CoffeeResource* src, QObject *parent)
{
    QVariantMap source;
    QJsonParseError err;
    source = QJsonDocument::fromJson(*src->data(),&err).object().toVariantMap();

    if(err.error!=QJsonParseError::NoError){
        qFatal("Failed to parse JSON document. This suggests that you should error-check the JSON data.\n"
               "error code %i: %s",
               err.error,
               err.errorString().toStdString().c_str());
    }

    return importAssets__base(parent,source,src->source());
}

QList<CoffeeWorldOpts *> CoffeeObjectFactory::importObjects(QString file, QObject *parent)
{
    CoffeeResource *f = new CoffeeResource(0,file);
    CoffeeAssetStorage* s = importAssets(f,parent);
    QList<CoffeeWorldOpts *> w = s->worlds;
    delete s;
    delete f;
    return w;
}

bool CoffeeObjectFactory::verbose() const
{
    return m_verbose;
}

void CoffeeObjectFactory::setVerbose(bool verbose)
{
    m_verbose = verbose;
}
