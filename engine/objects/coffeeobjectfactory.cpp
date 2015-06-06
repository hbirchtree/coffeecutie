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
#include <QVariantMap>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QColor>

CoffeeObjectFactory::CoffeeObjectFactory(){}

QList<CoffeeWorldOpts*> CoffeeObjectFactory::importObjects(QString file, QObject *parent)
{
    QElapsedTimer t;
    QVariantMap source;
    QJsonParseError err;
    source = QJsonDocument::fromJson(FileHandler::getStringFromFile(file).toLocal8Bit(),
                                     &err).object().toVariantMap();

    if(source.isEmpty()){
        qDebug("Object file was empty or non-existing!");
        return QList<CoffeeWorldOpts*>();
    }

    QFileInfo f(file);
    filepath = f.path()+QDir::separator();

    //Resources
//    for(QString key : source.keys())
//        if(key=="models")
//            importModels(source.value(key).toMap(),parent);

    //The asset-based system allows us to load a resource once and reuse it infinitely from the bottom up.
    //We will need to create a system later on that determines which resources should be unloaded and which should not.
    //We will most likely use the CoffeeGameAsset class for this (pre-alloc, alloc, and release as states)

    Assimp::Importer importer;

    for(QString key : source.keys())
        if(key=="assets"){
            for(QVariant assetContainer : source.value(key).toList()){
                QVariantMap assetData = assetContainer.toMap();
                if(!assetData.keys().contains("type"))
                    continue;
                QString assetType = assetData.value("type").toString();
                if(assetType=="shader"){
                    importShader(assetData,parent);
                }else if(assetType=="model"){
                    importModel(importer,assetData,parent);
                }else if(assetType=="texture"){
                    importTexture(assetData,parent);
                }
            }
        }

    qDebug("Assets: %i shader(s), %i model(s), %i mesh(es), %i material(s), %i texture(s)",
           shaders.size(),
           models.size(),
           meshes.size(),
           materials.size(),
           textures.size());

    //World data
    for(QString key : source.keys()){
        if(key.startsWith("world."))
            worlds.append(createWorld(key,source.value(key).toMap(),parent));
    }
    qDebug("Spent %u milliseconds parsing content from disk",t.elapsed());
    return worlds;
}

CoffeeObject *CoffeeObjectFactory::createObject(const QVariantMap &data, QObject* parent)
{
    CoffeeStandardObject* obj = new CoffeeStandardObject(parent);
    for(QString key : data.keys()){
        if(key=="id")
            obj->setObjectName("coffeeobject::"+data.value(key).toString());
        else if(key=="shader.id"){
            if(shaders.contains(data.value(key).toString())){
                obj->setShader(shaders.value(data.value(key).toString()));
            }else{
                qFatal("Failed to set shader for CoffeeStandardObject: Shader not found");
            }
        }else if(key=="model.id"){
            CoffeeModelStruct *m = acquireModel(data.value(key).toString());
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
                    pobj->getDescr()->setInertia(listToVec3(pd.value(pkey)));
                else if(pkey=="normal")
                    pobj->getDescr()->setnormal(listToVec3(pd.value(pkey)));
                else if(pkey=="linear-factor")
                    pobj->getDescr()->setLinearFactor(listToVec3(pd.value(pkey)));
                else if(pkey=="data"){
                    pobj->getDescr()->setFileSource(
                                FileHandler::getBytesFromFile(pd.value(pkey).toString()));
                }else if(pkey=="position")
                    pobj->getDescr()->setPosition(listToVec3(pd.value(pkey)));
                else if(pkey=="scale")
                    pobj->getDescr()->setScale(listToVec3(pd.value(pkey)));
            }
            if(pobj->getDescr()->getShape()!=PhysicsDescriptor::Shape_None){
                pobj->getPositionObject()->setValue(pobj->getDescr()->position());
                pobj->getPhysicalRotation()->setValue(pobj->getDescr()->orientation());
                obj->setPhysicsObject(pobj);
            }else
                delete pobj;
        }
    }
    if(obj->physics()){
        obj->physics()->setObjectName(obj->objectName()+".physics-object");
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

void CoffeeObjectFactory::importModel(Assimp::Importer &importer,const QVariantMap &data,QObject* parent)
{
    QFileInfo fileinfo(data.value("file").toString());
    QString key = data.value("name").toString();

    if(fileinfo.isRelative())
        fileinfo.setFile(filepath+data.value("file").toString());

    if(!fileinfo.exists()){
        qDebug("Failed to locate model: %s",fileinfo.filePath().toStdString().c_str());
        return;
    }
    if(key.isEmpty()){
        qDebug("Failed to import model: No key");
        return;
    }

    QHash<QString,CoffeeModelStruct*> models;
    QHash<QString,QPointer<CoffeeMesh>> meshes;
    QHash<QString,QPointer<CoffeeMaterial>> materials;

    const aiScene* scene = importer.ReadFile(
                fileinfo.filePath().toStdString().c_str(),
                aiProcess_CalcTangentSpace |
                aiProcess_Triangulate |
                aiProcess_OptimizeMeshes |
                aiProcess_SortByPType);
    if(!scene){
        qDebug("Failed to read model data %s: %s",
               fileinfo.filePath().toStdString().c_str(),importer.GetErrorString());
    }else{
        qDebug("Successfully read model data: %s:"
               " %i mesh(es), %i material(s), %i texture(s), "
               "%i light(s), %i camera(s), %i animation(s)",
               fileinfo.filePath().toStdString().c_str(),
               scene->mNumMeshes,scene->mNumMaterials,
               scene->mNumTextures,scene->mNumLights,
               scene->mNumCameras,scene->mNumAnimations);

        QVector<CoffeeMaterial*> mtllist;
        for(uint i=0;i<scene->mNumMaterials;i++){
            aiMaterial* mtl = scene->mMaterials[i];
            CoffeeMaterial* cmtl = new CoffeeMaterial(parent,mtl,fileinfo.path()+QDir::separator());
            materials.insert(cmtl->objectName(),cmtl);
            mtllist.append(cmtl);
        }
        for(uint i=0;i<scene->mNumMeshes;i++){
            aiMesh* mesh = scene->mMeshes[i];
            CoffeeMesh* cmesh = new CoffeeMesh(parent,mesh);
            while(meshes.contains(cmesh->objectName()))
                cmesh->setObjectName("Mesh."+QString::number(qrand()));
            meshes.insert(cmesh->objectName(),cmesh);
            CoffeeModelStruct *s = new CoffeeModelStruct;
            s->mesh = cmesh;
            if(mesh->mMaterialIndex<(uint)mtllist.size()){
                s->material = mtllist.at(mesh->mMaterialIndex);
            }else{
                qDebug("Could not find material for mesh: %s",cmesh->objectName().toStdString().c_str());
                continue;
            }
            qDebug("New model: name=%s,material=%s",cmesh->objectName().toStdString().c_str(),
                   s->material->objectName().toStdString().c_str());
            models.insert(cmesh->objectName(),s);
        }
    }

    this->models.insert(key,models);
    this->meshes.insert(key,meshes);
    this->materials.insert(key,materials);
}

void CoffeeObjectFactory::importTexture(const QVariantMap &data, QObject *parent)
{
    QString subtype = data.value("variant").toString();
    QString name = data.value("name").toString();

    if(name.isEmpty()){
        qDebug("Failed to import texture: No key");
    }

    if(subtype=="cubemap"){
        QMap<GLenum,QString> textureMapping;

        auto loopfunc = [](QString dir, GLenum &mapping){
            mapping = GL_NONE;

            if(dir == "north")
                mapping = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
            else if(dir == "south")
                mapping = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
            else if(dir == "west")
                mapping = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
            else if(dir == "east")
                mapping = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
            else if(dir == "up")
                mapping = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
            else if(dir == "down")
                mapping = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;

            if(mapping==GL_TEXTURE_CUBE_MAP_POSITIVE_X||
                    mapping==GL_TEXTURE_CUBE_MAP_POSITIVE_Y||
                    mapping==GL_TEXTURE_CUBE_MAP_POSITIVE_Z||
                    mapping==GL_TEXTURE_CUBE_MAP_NEGATIVE_X||
                    mapping==GL_TEXTURE_CUBE_MAP_NEGATIVE_Y||
                    mapping==GL_TEXTURE_CUBE_MAP_NEGATIVE_Z){
                return true;
            }
            return false;
        };

        for(QString key : data.keys()){
            QFileInfo file;
            GLenum type;

            if(loopfunc(key,type)){
                file.setFile(data.value(key).toString());

                if(file.isRelative())
                    file.setFile(filepath+file.filePath());

                if(!file.exists()){
                    qDebug("Failed to locate texture for cubemap: direction=%s,file=%s",
                           key.toStdString().c_str(),
                           file.filePath().toStdString().c_str());
                    return;
                }

                textureMapping.insert(type,file.filePath());
            }
        }
        CoffeeTexture* t = new CoffeeTexture(parent,textureMapping);

        t->setObjectName(name);

        this->textures.insert(name,t);
    }else if(subtype=="standard"){
        QFileInfo texture(data.value("file").toString());
        if(texture.isRelative())
            texture.setFile(filepath+data.value("file").toString());

        if(!texture.exists()){
            qDebug("Failed to locate texture: %s",
                   texture.filePath().toStdString().c_str());
            return;
        }

        CoffeeTexture* t = new CoffeeTexture(parent,texture.filePath());

        t->setObjectName(name);

        this->textures.insert(name,t);
    }
}

void CoffeeObjectFactory::importShader(const QVariantMap &data, QObject *parent)
{
    QString name = data.value("name").toString();

    if(name.isEmpty()){
        qDebug("Failed to import shader program: No key");
        return;
    }

    QString fragshader,vertshader,geomshader;


    for(QString key : data.keys()){
        QString *target; //we use this so that, in the case the path is relative, we may update it.
        if(key=="fragment"){
            fragshader=data.value(key).toString();
            target = &fragshader;
        }else if(key=="vertex"){
            vertshader=data.value(key).toString();
            target = &vertshader;
        }else if(key=="geometry"){
            geomshader=data.value(key).toString();
            target = &geomshader;
        }else
            continue;

        QFileInfo shaderFile(data.value(key).toString());

        if(shaderFile.isRelative())
            shaderFile.setFile(filepath+shaderFile.filePath());

        if(!shaderFile.exists()){
            qDebug("Failed to import shader program: Shader file not found: %s",
                   shaderFile.filePath().toStdString().c_str());
            return;
        }
        *target = shaderFile.filePath();
    }

    ShaderContainer* shader = new ShaderContainer(parent);
    shader->setVertexShader(vertshader);
    shader->setFragmentShader(fragshader);
    shader->setGeometryShader(geomshader);

    shader->setObjectName(name);

    shaders.insert(name,shader);
}

CoffeeWorldOpts *CoffeeObjectFactory::createWorld(const QString &key, const QVariantMap &data, QObject *parent)
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
                world->addObject(createObject(obj.toMap(),world));
        else if(key=="lights")
            for(const QVariant &obj : data.value(key).toList())
                world->addLight(createLight(obj.toMap(),world));
        else if(key=="clearcolor"){
            QColor c = stringToColor(data.value(key));
            world->setClearColor(glm::vec4(c.redF(),c.greenF(),c.blueF(),c.alphaF()));
        }else if(key=="skybox"){
            world->setSkybox(createSkybox(data.value(key).toMap(),world));
        }else if(key=="particle-systems")
            for(const QVariant &obj : data.value(key).toList())
                world->addParticleSystem(createParticleSystem(obj.toMap(),world));
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

CoffeeSkybox *CoffeeObjectFactory::createSkybox(const QVariantMap &data, QObject *parent)
{
    CoffeeSkybox* skybox = new CoffeeSkybox(parent);
    for(QString key : data.keys()){
        if(key=="cubemap"){
            skybox->setTexture(textures.value(data.value(key).toString()));
            if(!skybox->getTexture())
                qFatal("Failed to set skybox texture!");
        }else if(key=="mesh"){
            CoffeeModelStruct *m = acquireModel(data.value(key).toString());
            if(!m)
                qFatal("Failed to set skybox mesh!");
            skybox->setSkymesh(m->mesh);
            if(!skybox->getSkymesh())
                qFatal("Failed to set skybox mesh!");
        }else if(key=="shader"){
            skybox->setShader(shaders.value(data.value(key).toString()));
            if(!skybox->getShader())
                qFatal("Failed to set skybox texture!");
        }
    }
    if(!skybox->getShader()||!skybox->getSkymesh()||!skybox->getTexture())
        qFatal("Skybox does not have the required components!");
    return skybox;
}

CoffeeParticleSystem *CoffeeObjectFactory::createParticleSystem(const QVariantMap &data, QObject *parent)
{
    CoffeeParticleSystem* system = new CoffeeParticleSystem(parent,nullptr);

    for(QString key : data.keys()){
        if(key=="texture"){
            system->setTexture(textures.value(data.value(key).toString()));
            if(!system->getTexture())
                qFatal("Failed to set particle system sprite!");
        }else if(key=="render-shader"){
            system->setShader(shaders.value(data.value(key).toString()));
            if(!system->getShader())
                qFatal("Failed to set particle system render shader!");
        }else if(key=="transform-shader"){
            system->setTransformShader(shaders.value(data.value(key).toString()));
            if(!system->getTransformShader())
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
    if(!system->getTransformShader())
        qFatal("Particle system has no transform shader!");

    return system;
}

CoffeeObjectFactory::CoffeeModelStruct *CoffeeObjectFactory::acquireModel(QString identification)
{
    QStringList id = identification.split(":");
    if(id.length()<2)
        return nullptr;
    QRegExp r(id.at(1));
    for(QString m : models.value(id.at(0)).keys())
        if(m.contains(r)){
            return models.value(id.at(0)).value(m);
        }
    return nullptr;
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
