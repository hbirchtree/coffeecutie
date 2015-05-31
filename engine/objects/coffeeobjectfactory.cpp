#include "coffeeobjectfactory.h"

CoffeeObjectFactory::CoffeeObjectFactory(){}

QList<CoffeeWorldOpts*> CoffeeObjectFactory::importObjects(QString file, QObject *parent)
{
    qint64 t = QDateTime::currentMSecsSinceEpoch();
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
    for(QString key : source.keys())
        if(key=="models")
            importModels(source.value(key).toMap(),parent);

    for(QString key : source.keys()){
        if(key.startsWith("world."))
            worlds.append(createWorld(key,source.value(key).toMap(),parent));
    }
    qDebug("Spent %i milliseconds parsing content from disk",QDateTime::currentMSecsSinceEpoch()-t);
    return worlds;
}

CoffeeObject *CoffeeObjectFactory::createObject(const QVariantMap &data, QObject* parent)
{
    CoffeeStandardObject* obj = new CoffeeStandardObject(parent);
    for(QString key : data.keys()){
        if(key=="id")
            obj->setObjectName("coffeeobject::"+data.value(key).toString());
        else if(key=="shader.vertex"&&data.keys().contains("shader.fragment")){
            obj->setShader(new ShaderContainer(obj));
            obj->shader()->setVertexShader(filepath+data.value("shader.vertex").toString());
            obj->shader()->setFragmentShader(filepath+data.value("shader.fragment").toString());
        }else if(key=="model.idsrc"){
            QStringList id = data.value(key).toString().split(":");
            if(id.length()<2)
                continue;
            QRegExp r(id.at(1));
#ifdef COFFEE_USE_HORRIBLE_OBJ_IMPORTER
            for(QString m : models.value(id.at(0)).keys())
                if(m.contains(r)){
                    obj->setMesh(models.value(id.at(0)).value(m)->model);
                    obj->setMaterial(models.value(id.at(0)).value(m)->material);
                }
#else
            for(QString m : models.value(id.at(0)).keys())
                if(m.contains(r)){
                    CoffeeModelStruct s = models.value(id.at(0)).value(m);
                    obj->setMaterial(s.material);
                    obj->setMesh(s.mesh);
                }
#endif
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
    /*
     * id : object name
     *
     * shader.id : vsh and fsh
     * shader.vertex : vsh
     * shader.fragment : fsh
     *
     * model.position
     * model.position-offset : model's offset from physical one
     * model.rotation
     * model.scale
     * model.filesrc : direct filename
     * model.idsrc : id of model (includes material)
     *
     * material.transparency
     * material.shininess
     * material.color-multiplier
     *
     * physics.shape (QString)
     * physics.mass
     * physics.friction
     * physics.restitution
     * physics.inertia
     * physics.data
    */

    return obj;
}

void CoffeeObjectFactory::importModels(const QVariantMap &data,QObject* parent)
{
#ifdef COFFEE_USE_HORRIBLE_OBJ_IMPORTER
    WavefrontModelReader rdr(parent);
    int mcnt = models.size();
    for(QString key : data.keys()){
        models.insert(key,rdr.parseModel(filepath+data.value(key).toString()));
        rdr.clearData();
    }
    qDebug("Imported %i model sources from asset index",models.size()-mcnt);
#else
    Assimp::Importer importer;


    for(QString key : data.keys()){
        QString filename = filepath+data.value(key).toString();
        QFileInfo fileinfo(filename);

        QHash<QString,CoffeeModelStruct> models;
        QHash<QString,QPointer<CoffeeMesh>> meshes;
        QHash<QString,QPointer<CoffeeMaterial>> materials;

        const aiScene* scene = importer.ReadFile(
                    filename.toStdString().c_str(),
                    aiProcess_CalcTangentSpace |
                    aiProcess_Triangulate |
                    aiProcess_OptimizeMeshes |
                    aiProcess_SortByPType);
        if(!scene){
            qDebug("Failed to read model data %s: %s",filename.toStdString().c_str(),importer.GetErrorString());
        }else{
            qDebug("Successfully read model data: %s:\n"
                   " %i mesh(es), %i material(s), %i texture(s),\n"
                   "%i light(s), %i camera(s), %i animation(s)",
                   filename.toStdString().c_str(),
                   scene->mNumMeshes,scene->mNumMaterials,
                   scene->mNumTextures,scene->mNumLights,
                   scene->mNumCameras,scene->mNumAnimations);

            QVector<CoffeeMaterial*> mtllist;
            for(int i=0;i<scene->mNumMaterials;i++){
                aiMaterial* mtl = scene->mMaterials[i];
                CoffeeMaterial* cmtl = new CoffeeMaterial(parent,mtl,fileinfo.path()+QDir::separator());
                materials.insert(cmtl->objectName(),cmtl);
                mtllist.append(cmtl);
            }
            for(int i=0;i<scene->mNumMeshes;i++){
                aiMesh* mesh = scene->mMeshes[i];
                CoffeeMesh* cmesh = new CoffeeMesh(parent,mesh);
                while(meshes.contains(cmesh->objectName()))
                    cmesh->setObjectName("Mesh."+QString::number(qrand()));
                meshes.insert(cmesh->objectName(),cmesh);
                CoffeeModelStruct s;
                s.mesh = cmesh;
                if(mesh->mMaterialIndex<mtllist.size()){
                    s.material = mtllist.at(mesh->mMaterialIndex);
                }else{
                    qDebug("Could not find material for mesh: %s",cmesh->objectName().toStdString().c_str());
                    continue;
                }
                qDebug("New model: name=%s,material=%s",cmesh->objectName().toStdString().c_str(),
                       s.material->objectName().toStdString().c_str());
                models.insert(cmesh->objectName(),s);
            }
        }

        this->models.insert(key,models);
        this->meshes.insert(key,meshes);
        this->materials.insert(key,materials);
    }

#endif
}

CoffeeWorldOpts *CoffeeObjectFactory::createWorld(const QString &key, const QVariantMap &data, QObject *parent)
{
    CoffeeWorldOpts* world = new CoffeeWorldOpts(parent);
    world->setObjectName(key);
    for(QString key : data.keys()){
        if(key=="camera")
            world->setCamera(createCamera(data.value(key).toMap(),parent));
        else if(key=="world.fog.density")
            world->setFogDensity(data.value(key).toFloat());
        else if(key=="world.fog.color"){
            QColor c = stringToColor(data.value(key));
            world->setFogColor(glm::vec4(c.redF(),c.greenF(),c.blueF(),c.alphaF()));
        }else if(key=="objects")
            for(const QVariant &obj : data.value(key).toList())
                world->addObject(createObject(obj.toMap(),parent));
        else if(key=="lights")
            for(const QVariant &obj : data.value(key).toList())
                world->addLight(createLight(obj.toMap(),parent));
        else if(key=="clearcolor"){
            QColor c = stringToColor(data.value(key));
            world->setClearColor(glm::vec4(c.redF(),c.greenF(),c.blueF(),c.alphaF()));
        }
    }
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
