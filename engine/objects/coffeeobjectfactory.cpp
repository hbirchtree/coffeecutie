#include "coffeeobjectfactory.h"

CoffeeObjectFactory::CoffeeObjectFactory(){}

QList<CoffeeWorldOpts*> CoffeeObjectFactory::importObjects(QString file, QObject *parent)
{
    QVariantMap source;
    QJsonParseError err;
    source = QJsonDocument::fromJson(FileHandler::getStringFromFile(file).toLocal8Bit(),
                                     &err).object().toVariantMap();

    if(source.isEmpty())
        return QList<CoffeeWorldOpts*>();

    QFileInfo f(file);
    filepath = f.path()+QDir::separator();

    for(QString key : source.keys()){
        if(key.startsWith("world.")){
            worlds.append(createWorld(source.value(key).toMap(),parent));
        }else if(key=="models")
            importModels(source.value(key).toMap(),parent);
    }
    return worlds;
}

CoffeeObject *CoffeeObjectFactory::createObject(const QVariantMap &data, QObject* parent)
{
    CoffeeObject* obj = new CoffeeObject(parent);
    for(QString key : data.keys()){
        if(key=="id")
            obj->setObjectName("coffeeobject::"+data.value(key).toString());
        else if(key=="shader.vertex")
            obj->setVertShader(filepath+data.value(key).toString());
        else if(key=="shader.fragment")
            obj->setFragShader(filepath+data.value(key).toString());
        else if(key=="model.idsrc"){
            QStringList id = data.value(key).toString().split(":");
            if(id.length()<2)
                continue;
            QRegExp r(id.at(1));
            for(QString m : models.value(id.at(0)).keys())
                if(m.contains(r)){
                    obj->setModel(models.value(id.at(0)).value(m)->model);
                    obj->setMaterial(models.value(id.at(0)).value(m)->material);
                }
        }
        else if(key=="model.position")
            obj->getPositionObject()->setValue(listToVec3(data.value(key)));
        else if(key=="model.scale")
            obj->getScaleObject()->setValue(listToVec3(data.value(key)));
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
     * physics.*
     *
    */

    return obj;
}

void CoffeeObjectFactory::importModels(const QVariantMap &data,QObject* parent)
{
    WavefrontModelReader rdr(parent);
    for(QString key : data.keys())
        models.insert(key,rdr.parseModel(filepath+data.value(key).toString()));
}

CoffeeWorldOpts *CoffeeObjectFactory::createWorld(const QVariantMap &data, QObject *parent)
{
    CoffeeWorldOpts* world = new CoffeeWorldOpts(parent);
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
