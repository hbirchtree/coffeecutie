#include "coffeeassetimporter.h"

#include "engine/objects/coffeestandardobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "engine/objects/coffeeobject.h"
#include "engine/models/coffeemesh.h"
#include "engine/data/coffeeresource.h"
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
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "engine/models/coffeeskeleton.h"
#include "assimp/anim.h"
#include "assimp/matrix4x4.h"
#include "assimp/vector3.h"

CoffeeAssetImporter::CoffeeAssetImporter(QObject *parent, QObject *outputParent) : QObject(parent)
{
    this->outputParent = outputParent;
}

CoffeeAssetStorage *CoffeeAssetImporter::importAssets(QVariantList assetList, const QString &filepath)
{
    QObject* importRoot = new QObject();
    QList<QFuture<CoffeeAssetStorage*>> importData;

    CoffeeAssetStorage* s = new CoffeeAssetStorage(0);
    s->filepath = filepath;
    s->moveToThread(outputParent->thread());

    for(QVariant assetContainer : assetList){
        QVariantMap assetData = assetContainer.toMap();
        if(!assetData.keys().contains("type"))
            continue;
        QString assetType = assetData.value("type").toString();
        if(assetType=="shader"){
            QFuture<CoffeeAssetStorage*> asset =
                    QtConcurrent::run(new CoffeeAssetImporter(importRoot,outputParent),
                                      &CoffeeAssetImporter::importShader,
                                      assetData,
                                      s->filepath);
            importData.append(asset);
        }else if(assetType=="model"){
            QFuture<CoffeeAssetStorage*> asset =
                    QtConcurrent::run(new CoffeeAssetImporter(importRoot,outputParent),
                                      &CoffeeAssetImporter::importModel,
                                      assetData,
                                      s->filepath);
            importData.append(asset);
        }else if(assetType=="texture"){
            QFuture<CoffeeAssetStorage*> asset =
                    QtConcurrent::run(new CoffeeAssetImporter(importRoot,outputParent),
                                      &CoffeeAssetImporter::importTexture,
                                      assetData,
                                      s->filepath);
            importData.append(asset);
        }
    }

    for(QFuture<CoffeeAssetStorage*> e : importData){
        CoffeeAssetStorage* p;
        p = e.result();
        s->merge(p);
        delete p;
    }

    importRoot->deleteLater();

    return s;
}

CoffeeAssetStorage *CoffeeAssetImporter::importModel(const QVariantMap &data,
                                                    const QString &filepath)
{
    CoffeeAssetStorage *s = new CoffeeAssetStorage(0);
    s->filepath = filepath;
    s->moveToThread(outputParent->thread());

    Assimp::Importer importer;
    QFileInfo fileinfo(data.value("file").toString());
    QString key = data.value("name").toString();

    if(fileinfo.isRelative())
        fileinfo.setFile(filepath+data.value("file").toString());

    if(!fileinfo.exists()){
        qDebug("Failed to locate model: %s",fileinfo.filePath().toStdString().c_str());
        return s;
    }
    if(key.isEmpty()){
        qDebug("Failed to import model: No key");
        return s;
    }

    QHash<QString,CoffeeModelStruct*> models;
    QHash<QString,QPointer<CoffeeMesh>> meshes;
    QHash<QString,QPointer<CoffeeMaterial>> materials;

    //replace with importer.ReadFileFromMemory() to make use of CoffeeResource*
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

        for(uint i=0;i<scene->mNumAnimations;i++){
            aiAnimation* ani = scene->mAnimations[i];
            CoffeeAnimation* cani = new CoffeeAnimation(0,ani,scene->mRootNode);
        }

        QVector<CoffeeMaterial*> mtllist;
        for(uint i=0;i<scene->mNumMaterials;i++){
            aiMaterial* mtl = scene->mMaterials[i];
            CoffeeMaterial* cmtl = new CoffeeMaterial(0,mtl,fileinfo.path()+QDir::separator());
            cmtl->moveToThread(outputParent->thread());
            materials.insert(cmtl->objectName(),cmtl);
            mtllist.append(cmtl);
        }
        for(uint i=0;i<scene->mNumMeshes;i++){
            aiMesh* mesh = scene->mMeshes[i];
            CoffeeMesh* cmesh = new CoffeeMesh(0,mesh,scene->mRootNode);
            while(meshes.contains(cmesh->objectName()))
                cmesh->setObjectName("Mesh."+QString::number(qrand()));
            meshes.insert(cmesh->objectName(),cmesh);
            CoffeeModelStruct *ms = new CoffeeModelStruct(0);
            ms->mesh = cmesh;
            if(mesh->mMaterialIndex<(uint)mtllist.size()){
                ms->material = mtllist.at(mesh->mMaterialIndex);
            }else{
                qDebug("Could not find material for mesh: %s",cmesh->objectName().toStdString().c_str());
                continue;
            }
            qDebug("New model: name=%s,material=%s",cmesh->objectName().toStdString().c_str(),
                   ms->material->objectName().toStdString().c_str());
            cmesh->moveToThread(outputParent->thread());
            ms->moveToThread(outputParent->thread());
            models.insert(cmesh->objectName(),ms);
        }
    }

    s->models.insert(key,models);
    s->meshes.insert(key,meshes);
    s->materials.insert(key,materials);

    return s;
}

CoffeeAssetStorage *CoffeeAssetImporter::importTexture(const QVariantMap &data, const QString &filepath)
{
    CoffeeAssetStorage *s = new CoffeeAssetStorage(0);
    s->filepath = filepath;
    s->moveToThread(outputParent->thread());

    QString subtype = data.value("variant").toString();
    QString name = data.value("name").toString();

    if(name.isEmpty()){
        qDebug("Failed to import texture: No key");
    }

    if(subtype=="cubemap"){
        QMap<GLenum,CoffeeResource*> textureMapping;

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
            else
                return false;
            return true;
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
                    return s;
                }
                CoffeeResource* r = new CoffeeResource(0,file.filePath());
                s->resources.append(r);

                textureMapping.insert(type,r);
            }
        }
        CoffeeTexture* t = new CoffeeTexture(0,textureMapping);

        t->setObjectName(name);
        t->moveToThread(outputParent->thread());
        s->textures.insert(name,t);
    }else if(subtype=="cubemap-dice"){
        QFileInfo file(data.value("source").toString());

        if(file.isRelative())
            file.setFile(filepath+file.filePath());

        CoffeeResource* r = new CoffeeResource(0,file.filePath());
        s->resources.append(r);

        CoffeeTexture* t = new CoffeeTexture(0,r,QRect());

        t->setObjectName(name);
        t->moveToThread(outputParent->thread());
        s->textures.insert(name,t);
    }else if(subtype=="standard"){
        QFileInfo texture(data.value("file").toString());
        if(texture.isRelative())
            texture.setFile(filepath+data.value("file").toString());

        if(!texture.exists()){
            qDebug("Failed to locate texture: %s",
                   texture.filePath().toStdString().c_str());
            return s;
        }
        CoffeeResource* r = new CoffeeResource(0,texture.filePath());
        s->resources.append(r);

        CoffeeTexture* t = new CoffeeTexture(0,r);

        t->setObjectName(name);
        t->moveToThread(outputParent->thread());
        s->textures.insert(name,t);
    }
    return s;
}

CoffeeAssetStorage *CoffeeAssetImporter::importShader(const QVariantMap &data, const QString &filepath)
{
    CoffeeAssetStorage *s = new CoffeeAssetStorage(0);
    s->filepath = filepath;
    s->moveToThread(outputParent->thread());

    QString name = data.value("name").toString();

    if(name.isEmpty()){
        qDebug("Failed to import shader program: No key");
        return s;
    }

    CoffeeResource *fragshader = nullptr,*vertshader = nullptr,*geomshader = nullptr;


    for(QString key : data.keys()){
        CoffeeResource *target; //we use this so that, in the case the path is relative, we may update it.
        if(key=="fragment"){
            fragshader= new CoffeeResource(0,data.value(key).toString());
            target = fragshader;
        }else if(key=="vertex"){
            vertshader= new CoffeeResource(0,data.value(key).toString());
            target = vertshader;
        }else if(key=="geometry"){
            geomshader= new CoffeeResource(0,data.value(key).toString());
            target = geomshader;
        }else
            continue;

        QFileInfo shaderFile(data.value(key).toString());

        if(shaderFile.isRelative())
            shaderFile.setFile(filepath+shaderFile.filePath());

        if(!shaderFile.exists()){
            qDebug("Failed to import shader program: Shader file not found: %s",
                   shaderFile.filePath().toStdString().c_str());
            return s;
        }
        target->setSource(shaderFile.filePath());
    }

    ShaderContainer* shader = new ShaderContainer(0);
    shader->setVertexShader(vertshader);
    shader->setFragmentShader(fragshader);
    shader->setGeometryShader(geomshader);

    s->resources << vertshader << fragshader << geomshader;

    shader->setObjectName(name);
    shader->moveToThread(outputParent->thread());
    s->shaders.insert(name,shader);

    return s;
}

CoffeeAssetStorage::CoffeeAssetStorage(QObject *parent) : QObject(parent)
{
}

void CoffeeAssetStorage::merge(CoffeeAssetStorage *data){
    models.unite(data->models);
    materials.unite(data->materials);
    meshes.unite(data->meshes);

    shaders.unite(data->shaders);
    worlds.append(data->worlds);
    textures.unite(data->textures);

    resources.append(data->resources);
}


CoffeeModelStruct *CoffeeAssetStorage::acquireModel(QString identification)
{
    QStringList id = identification.split(":");
    if(id.length()<2)
        return nullptr;
    QRegExp r(id.at(1));
    for(QString o : models.keys())
        if(o==id.at(0))
            for(QString m : models.value(id.at(0)).keys())
                if(m.contains(r)){
                    return models.value(id.at(0)).value(m);
                }
    return nullptr;
}

void CoffeeAssetStorage::setParents(QObject *parent)
{
    for(CoffeeWorldOpts* w : worlds){
        w->setParent(parent);
    }

    for(QHash<QString,CoffeeModelStruct*> m : models.values())
        for(CoffeeModelStruct* p : m.values())
            p->setParent(parent);

    for(QHash<QString,QPointer<CoffeeMesh>> m : meshes.values())
        for(QPointer<CoffeeMesh> p : m.values()){
            p->setParent(parent);
        }

    for(QHash<QString,QPointer<CoffeeMaterial>> m : materials.values())
        for(QPointer<CoffeeMaterial> p : m.values()){
            p->setParent(parent);
        }

    for(QPointer<CoffeeTexture> p : textures){
        p->setParent(parent);
    }

    for(QPointer<ShaderContainer> p : shaders)
        p->setParent(parent);
}

QObject *CoffeeAssetStorage::getMesh(QString source)
{
    QStringList id = source.split(":");
    if(id.length()<2)
        return nullptr;
    QRegExp r(id.at(1));
    for(QString o : meshes.keys())
        if(o==id.at(0))
            for(QString m : meshes.value(id.at(0)).keys())
                if(m.contains(r)){
                    return meshes.value(id.at(0)).value(m);
                }
    return nullptr;
}

QObject *CoffeeAssetStorage::getMaterial(QString source)
{
    QStringList id = source.split(":");
    if(id.length()<2)
        return nullptr;
    QRegExp r(id.at(1));
    for(QString o : materials.keys())
        if(o==id.at(0))
            for(QString m : materials.value(id.at(0)).keys())
                if(m.contains(r)){
                    return materials.value(id.at(0)).value(m);
                }
    return nullptr;
}

QObject *CoffeeAssetStorage::getModel(QString source)
{
    return acquireModel(source);
}

QObjectList CoffeeAssetStorage::getWorlds()
{
    QObjectList l;
    for(CoffeeWorldOpts* e : worlds){
        l.append(e);
    }
    return l;
}

QObjectList CoffeeAssetStorage::getShaders()
{
    QObjectList l;
    for(ShaderContainer* p : shaders.values())
        l.append(p);
    return l;
}

QObjectList CoffeeAssetStorage::getTextures()
{
    QObjectList l;
    for(QPointer<CoffeeTexture> p : textures.values())
        l.append(p);
    return l;
}

QObjectList CoffeeAssetStorage::getResources()
{
    QObjectList l;
    for(QPointer<CoffeeResource> p : resources)
        if(p)
            l.append(p);
    return l;
}


CoffeeModelStruct::CoffeeModelStruct(QObject *parent) : QObject(parent)
{
}

QObject *CoffeeModelStruct::getMeshObject()
{
    return mesh;
}

QObject *CoffeeModelStruct::getMaterialObject()
{
    return material;
}
