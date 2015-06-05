#include "coffeeinstancecontainer.h"

#include "opengl/helpers/renderingmethods.h"

CoffeeInstanceContainer::CoffeeInstanceContainer(QObject *parent) : QObject(parent)
{
    instances.append(new InstanceData(glm::vec3(5,0,5),glm::quat(1,0,0,0),glm::vec3(1,1,1)));
    instances.append(new InstanceData(glm::vec3(-5,0,-5),glm::quat(1,0,0,0),glm::vec3(1,3,1)));
}

uint CoffeeInstanceContainer::instanceCount() const
{
    return instances.size();
}

CoffeeInstanceContainer::InstanceData *CoffeeInstanceContainer::getInstance(uint index)
{
    if(index>instances.size())
        qFatal("Invalid instance index");
    return instances.at(index);
}

QVector<glm::mat4> CoffeeInstanceContainer::getData() const
{
    QVector<glm::mat4> d;

    //TODO : Implement QtConcurrent for the case where we have tons of instances

    for(InstanceData* i : instances)
        d.append(glm::transpose(RenderingMethods::translateObjectMatrix(i->pos,
                                                         i->rot,
                                                         i->scale)));

    return d;
}
