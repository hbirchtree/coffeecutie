#include "coffeeinstancecontainer.h"

#include "opengl/helpers/renderingmethods.h"
#include "engine/physics/physicsobject.h"

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


PhysicsObject *CoffeeInstanceData::physics()
{
    return m_physics;
}

void CoffeeInstanceData::bindObject(PhysicsObject *target)
{
    this->pos->bindValue(target->getPositionObject());
    this->rot->bindValue(target->getPhysicalRotation());

    m_physics = target;
}
