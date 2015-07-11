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

QObject *CoffeeObjectFactory::importAssets(QString src)
{
    CoffeeResource *f = new CoffeeResource(0,src);
    CoffeeAssetStorage* s = importAssets(f,this->parent());
    delete f;
    return s;
}

CoffeeAssetStorage* CoffeeObjectFactory::importAssets(CoffeeResource* src, QObject *parent)
{
    QElapsedTimer t;
    t.start();
    QVariantMap source;
    QJsonParseError err;
    source = QJsonDocument::fromJson(*src->data(),&err).object().toVariantMap();

    if(err.error!=QJsonParseError::NoError){
        qFatal("Failed to parse JSON document. This suggests that you should error-check the JSON data.\n"
               "error code %i: %s",
               err.error,
               err.errorString().toStdString().c_str());
    }

    CoffeeAssetStorage *s = new CoffeeAssetStorage(0);
    s->moveToThread(parent->thread());

    if(source.isEmpty()){
        qDebug("Object file was empty or non-existing!");
        return s;
    }

    QFileInfo f(src->source());
    s->filepath = f.path()+QDir::separator();

    //The asset-based system allows us to load a resource once and reuse it infinitely from the bottom up.
    //We will need to create a system later on that determines which resources should be unloaded and which should not.
    //We will most likely use the CoffeeGameAsset class for this (pre-alloc, alloc, and release as states)

    QObject* importerRoot = new QObject();
    QList<QFuture<CoffeeAssetStorage*>> importData;

    for(QString key : source.keys())
        if(key=="assets"){
            CoffeeAssetImporter* imp = new CoffeeAssetImporter(importerRoot,parent); //temporary object
            QFuture<CoffeeAssetStorage*> index =
                    QtConcurrent::run(imp,
                                      &CoffeeAssetImporter::importAssets,
                                      source.value(key).toList(),
                                      s->filepath);
            importData.append(index);
        }

    qDebug("Data import initiated");

    for(QFuture<CoffeeAssetStorage*> e : importData){
        CoffeeAssetStorage* p = e.result();
        s->merge(p);
        delete p;
    }

    qDebug("Data import finished");

    importerRoot->deleteLater();

    s->setParents(parent);

    qDebug("Assets: %i shader(s), %i model(s), %i mesh(es), %i material(s), %i texture(s)",
           s->shaders.size(),
           s->models.size(),
           s->meshes.size(),
           s->materials.size(),
           s->textures.size());

    //World data
    for(QString key : source.keys()){
        if(key.startsWith("world."))
            s->worlds.append(createWorld(key.mid(6),source.value(key).toMap(),s,parent));
    }
    qDebug("Spent %i milliseconds parsing content from disk",t.elapsed());
    return s;
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

CoffeeObject *CoffeeObjectFactory::createObject(const QVariantMap &data, CoffeeAssetStorage *assets, QObject* parent)
{
    CoffeeStandardObject* obj = new CoffeeStandardObject(parent);
    for(QString key : data.keys()){
        if(key=="id")
            obj->setObjectName(data.value(key).toString());
        else if(key=="shader.id"){
            if(assets->shaders.contains(data.value(key).toString())){
                obj->setShader(assets->shaders.value(data.value(key).toString()));
            }else{
                qFatal("Failed to set shader for CoffeeStandardObject: Shader not found");
            }
        }else if(key=="model.id"){
            CoffeeModelStruct *m = assets->acquireModel(data.value(key).toString());
            if(!m)
                qFatal("Failed to set model!");
            obj->setMesh(m->mesh);
            obj->setMaterial(m->material);
            if(!obj->mesh()||!obj->material()){
                qFatal("Failed to set CoffeeStandardObject model or mesh: Please verify configuration");
            }
        }
        else if(key=="model.position")
            obj->position()->setValue(listToVec3(data.value(key)));
        else if(key=="model.scale")
            obj->scale()->setValue(listToVec3(data.value(key)));
        else if(key=="physics"){
            PhysicsObject* pobj = new PhysicsObject(obj);
            QVariantMap pd = data.value(key).toMap();
            for(QString pkey : pd.keys()){
                if(pkey=="shape"){
                    QString t = pd.value(pkey).toString();
                    PhysicsDescriptor::PhysicalShape s = PhysicsDescriptor::Shape_None;
                    if(t=="box")
                        s = PhysicsDescriptor::Shape_Box;
                    else if(t=="sphere")
                        s = PhysicsDescriptor::Shape_Sphere;
                    else if(t=="staticplane")
                        s = PhysicsDescriptor::Shape_StaticPlane;
                    else if(t=="capsule")
                        s = PhysicsDescriptor::Shape_Capsule;
                    else if(t=="cylinder")
                        s = PhysicsDescriptor::Shape_Cylinder;
                    else if(t=="cone")
                        s = PhysicsDescriptor::Shape_Cone;
                    else if(t=="staticplane")
                        s = PhysicsDescriptor::Shape_StaticPlane;
                    pobj->getDescr()->setShape(s);
                }else if(pkey=="mass")
                    pobj->getDescr()->setMass(pd.value(pkey).toFloat());
                else if(pkey=="friction")
                    pobj->getDescr()->setFriction(pd.value(pkey).toFloat());
                else if(pkey=="restitution")
                    pobj->getDescr()->setRestitution(pd.value(pkey).toFloat());
                else if(pkey=="inertia")
                    pobj->getDescr()->setInertia(pd.value(pkey).toList());
                else if(pkey=="normal")
                    pobj->getDescr()->setNormal(pd.value(pkey).toList());
                else if(pkey=="linear-factor")
                    pobj->getDescr()->setLinearFactor(pd.value(pkey).toList());
                else if(pkey=="data"){
                    pobj->getDescr()->setFileSource(pd.value(pkey).toString());
                }else if(pkey=="position")
                    pobj->getDescr()->setPosition(pd.value(pkey).toList());
                else if(pkey=="scale")
                    pobj->getDescr()->setScale(pd.value(pkey).toList());
            }
            if(pobj->getDescr()->shape()!=PhysicsDescriptor::Shape_None){
                pobj->getPositionObject()->setValue(varListToVec3(pobj->getDescr()->position()));
                pobj->getPhysicalRotation()->setValue(varListToQuat(pobj->getDescr()->orientation()));
                obj->setPhysicsObject(pobj);
            }else
                delete pobj;
        }
    }
    if(obj->physics()){
        obj->physics()->setObjectName("physics");
        obj->position()->bindValue(obj->physics()->getPositionObject());
    }
    if(!obj->shader()){
        qFatal("CoffeeStandardObject does not have a shader!");
    }
    if(!obj->mesh()){
        qFatal("CoffeeStandardObject does not have a mesh!");
    }
    if(!obj->material()){
        qFatal("CoffeeStandardObject does not have a material!");
    }

    return obj;
}

CoffeeWorldOpts *CoffeeObjectFactory::createWorld(const QString &key, const QVariantMap &data, CoffeeAssetStorage *assets, QObject *parent)
{
    CoffeeWorldOpts* world = new CoffeeWorldOpts(parent);
    world->setObjectName(key);
    for(QString key : data.keys()){
        if(key=="camera")
            world->setCamera(createCamera(data.value(key).toMap(),world));
        else if(key=="world.fog.density")
            world->setFogDensity(data.value(key).toFloat());
        else if(key=="world.fog.color"){
            QColor c = stringToColor(data.value(key));
            world->setFogColor(glm::vec4(c.redF(),c.greenF(),c.blueF(),c.alphaF()));
        }else if(key=="objects")
            for(const QVariant &obj : data.value(key).toList())
                world->addObject(createObject(obj.toMap(),assets,world));
        else if(key=="lights")
            for(const QVariant &obj : data.value(key).toList())
                world->addLight(createLight(obj.toMap(),world));
        else if(key=="clearcolor"){
            QColor c = stringToColor(data.value(key));
            world->setClearColor(glm::vec4(c.redF(),c.greenF(),c.blueF(),c.alphaF()));
        }else if(key=="skybox"){
            world->setSkybox(createSkybox(data.value(key).toMap(),assets,world));
        }else if(key=="particle-systems")
            for(const QVariant &obj : data.value(key).toList())
                world->addParticleSystem(createParticleSystem(obj.toMap(),assets,world));
    }

    if(world->getSkybox())
        world->getSkybox()->setCamera(world->getCamera());

    world->prepareParticleSystems(); //sets camera for them

    return world;
}

CoffeeCamera *CoffeeObjectFactory::createCamera(const QVariantMap &data, QObject *parent)
{
    CoffeeCamera* camera = new CoffeeCamera(parent);
    for(QString key : data.keys()){
        if(key=="fov")
            camera->getFieldOfView()->setValue(data.value(key).toFloat());
        else if(key=="znear")
            camera->setZnear(data.value(key).toFloat());
        else if(key=="zfar")
            camera->setZfar(data.value(key).toFloat());
        else if(key=="position")
            camera->getPosition()->setValue(listToVec3(data.value(key)));
        else if(key=="rotation")
            camera->getRotation()->setValue(listToVec3(data.value(key)));
        else if(key=="orthographic")
            camera->setOrthographic(data.value(key).toBool());
    }
    return camera;
}

CoffeeOmniLight *CoffeeObjectFactory::createLight(const QVariantMap &data, QObject *parent)
{
    CoffeeOmniLight* light = new CoffeeOmniLight(parent);
    for(QString key : data.keys()){
        if(key=="ambient-coefficient")
            light->getAmbientCoefficient()->setValue(data.value(key).toFloat());
        else if(key=="attenuation")
            light->getAttenuation()->setValue(data.value(key).toFloat());
        else if(key=="position")
            light->getPosition()->setValue(listToVec3(data.value(key)));
        else if(key=="color"){
            QColor c = stringToColor(data.value(key));
            light->getColor()->setValue(glm::vec3(c.redF(),c.greenF(),c.blueF()));
        }
    }
    return light;
}

CoffeeSkybox *CoffeeObjectFactory::createSkybox(const QVariantMap &data, CoffeeAssetStorage *assets, QObject *parent)
{
    CoffeeSkybox* skybox = new CoffeeSkybox(parent);
    for(QString key : data.keys()){
        if(key=="cubemap"){
            skybox->setTexture(assets->textures.value(data.value(key).toString()));
            if(!skybox->getTexture())
                qFatal("Failed to set skybox texture!");
        }else if(key=="mesh"){
            CoffeeModelStruct *m = assets->acquireModel(data.value(key).toString());
            if(!m)
                qFatal("Failed to set skybox mesh!");
            skybox->setSkymesh(m->mesh);
            if(!skybox->getSkymesh())
                qFatal("Failed to set skybox mesh!");
        }else if(key=="shader"){
            skybox->setShader(assets->shaders.value(data.value(key).toString()));
            if(!skybox->getShader())
                qFatal("Failed to set skybox texture!");
        }
    }
    if(!skybox->getShader()||!skybox->getSkymesh()||!skybox->getTexture())
        qFatal("Skybox does not have the required components!");
    return skybox;
}

CoffeeParticleSystem *CoffeeObjectFactory::createParticleSystem(const QVariantMap &data, CoffeeAssetStorage *assets, QObject *parent)
{
    CoffeeParticleSystem* system = new CoffeeParticleSystem(parent,nullptr);

    for(QString key : data.keys()){
        if(key=="texture"){
            system->setTexture(assets->textures.value(data.value(key).toString()));
            if(!system->getTexture())
                qFatal("Failed to set particle system sprite!");
        }else if(key=="render-shader"){
            system->setShader(assets->shaders.value(data.value(key).toString()));
            if(!system->getShader())
                qFatal("Failed to set particle system render shader!");
        }else if(key=="transform-shader"){
            system->getTransform()->setShader(assets->shaders.value(data.value(key).toString()));
            if(!system->getTransform()->getShader())
                qFatal("Failed to set particle system transform shader!");
        }else if(key=="color"){
            system->setParticleColor(stringToColor(data.value(key)));
        }else if(key=="particle-mass"){
            system->setParticleMass(data.value(key).toFloat());
        }else if(key=="particle-size"){
            system->setParticleSize(data.value(key).toFloat());
        }else if(key=="gravity"){
            system->setGravity(listToVec3(data.value(key)));
        }else if(key=="position"){
            system->position()->setValue(listToVec3(data.value(key).toString()));
        }else if(key=="id"){
            system->setObjectName(data.value(key).toString());
        }
    }

    if(!system->getTexture())
        qFatal("Particle system has no sprite!");
    if(!system->getShader())
        qFatal("Particle system has no render shader!");
    if(!system->getTransform()->getShader())
        qFatal("Particle system has no transform shader!");

    return system;
}

glm::vec3 CoffeeObjectFactory::listToVec3(const QVariant &data)
{
    if(data.type()==QVariant::List){
        QVariantList vec = data.toList();
        if(vec.size()==3)
            return glm::vec3(vec.at(0).toFloat(),vec.at(1).toFloat(),
                             vec.at(2).toFloat());
    }
    return glm::vec3();
}

glm::vec4 CoffeeObjectFactory::listToVec4(const QVariant &data)
{
    if(data.type()==QVariant::List){
        QVariantList vec = data.toList();
        if(vec.size()==4)
            return glm::vec4(vec.at(0).toFloat(),vec.at(1).toFloat(),
                             vec.at(2).toFloat(),vec.at(3).toFloat());
    }
    return glm::vec4();
}

QColor CoffeeObjectFactory::stringToColor(const QVariant &data)
{
    QColor r;
    QString c = data.toString().mid(1); //separate out '#'
    r.setRed(c.mid(0,2).toUInt(0,16));
    r.setGreen(c.mid(2,2).toUInt(0,16));
    r.setBlue(c.mid(4,2).toUInt(0,16));
    if(c.size()==8)
        r.setAlpha(c.mid(6,2).toUInt(0,16));
    return r;
}

glm::vec3 CoffeeObjectFactory::varListToVec3(const QVariantList &d)
{
    if(d.size()!=3)
        return glm::vec3();
    return glm::vec3(d.at(0).toFloat(),d.at(1).toFloat(),d.at(2).toFloat());
}

glm::quat CoffeeObjectFactory::varListToQuat(const QVariantList &d)
{
    if(d.size()!=4)
        return glm::quat();
    return glm::quat(d.at(0).toFloat(),d.at(1).toFloat(),d.at(2).toFloat(),d.at(3).toFloat());
}
