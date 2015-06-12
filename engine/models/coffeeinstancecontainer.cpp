#include "coffeeinstancecontainer.h"

#include "opengl/helpers/renderingmethods.h"
#include "engine/physics/physicsobject.h"

#include "engine/scripting/qscriptvectorvalue.h"

#include <QtConcurrent>

CoffeeInstanceContainer::CoffeeInstanceContainer(QObject *parent) : QObject(parent)
{
    //InstanceData is not a QObject, and should not be listed in the inspector.
    //Doing this would murder performance when there are 10 000 or more objects to add every 1 seconds.
    //The details of these objects are not interesting in most cases.
    instanceAnchor = new QObject();
}

CoffeeInstanceContainer::~CoffeeInstanceContainer()
{
    delete instanceAnchor;
    for(CoffeeInstanceData* d : instances)
        delete d;
}

uint CoffeeInstanceContainer::instanceCount() const
{
    return instances.size();
}

CoffeeInstanceData *CoffeeInstanceContainer::getInstance(int index)
{
    if(index>instances.size())
        qFatal("Invalid instance index");
    return instances.at(index);
}

QVector<glm::mat4> CoffeeInstanceContainer::getData() const
{
    QVector<glm::mat4> d;

    //TODO : Implement QtConcurrent for the case where we have tons of instances

    for(CoffeeInstanceData* i : instances)
        d.append(RenderingMethods::translateObjectMatrix(i->getPos()->getValue(),
                                                         i->getRot()->getValue(),
                                                         i->getScale()->getValue()));

    return d;
}

QObjectList CoffeeInstanceContainer::instanceObjects() const
{
    return instanceAnchor->findChildren<QObject*>();
}

void CoffeeInstanceContainer::createInstance()
{
    instances.append(createInstanceData());
}

void CoffeeInstanceContainer::addInstance(CoffeeInstanceData *i)
{
    instances.append(i);
}

void CoffeeInstanceContainer::clearInstances()
{
    for(CoffeeInstanceData* d : instances)
        delete d;
    instances.clear();
}

CoffeeInstanceData *CoffeeInstanceContainer::createInstanceData()
{
    return new CoffeeInstanceData(glm::vec3(0,0,0),glm::quat(1,0,0,0),glm::vec3(1,1,1),instanceAnchor);
}


CoffeeInstanceData::CoffeeInstanceData(glm::vec3 pos, glm::quat rot, glm::vec3 scale, QObject *parent) : QObject(parent){
    this->pos = new NumberContainer<glm::vec3>(this,pos);
    this->scale = new NumberContainer<glm::vec3>(this,scale);
    this->rot = new NumberContainer<glm::quat>(this,rot);

    this->posWrapper = new VectorValue(this->pos);
    this->rotWrapper = new QuaternionValue(this->rot);
    this->sclWrapper = new VectorValue(this->scale);
}

NumberContainer<glm::vec3> *CoffeeInstanceData::getPos(){ return pos;}

NumberContainer<glm::vec3> *CoffeeInstanceData::getScale(){ return scale;}

NumberContainer<glm::quat> *CoffeeInstanceData::getRot(){ return rot;}

PhysicsObject *CoffeeInstanceData::physics()
{
    return m_physics;
}

QObject *CoffeeInstanceData::rotationRef()
{
    return rotWrapper;
}

QObject *CoffeeInstanceData::positionRef()
{
    return posWrapper;
}

QObject *CoffeeInstanceData::scaleRef()
{
    return sclWrapper;
}

void CoffeeInstanceData::bindObject(PhysicsObject *target)
{
    this->pos->bindValue(target->getPositionObject());
    this->rot->bindValue(target->getPhysicalRotation());

    m_physics = target;
}
