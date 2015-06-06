#include "coffeeinstancecontainer.h"

#include "opengl/helpers/renderingmethods.h"

CoffeeInstanceContainer::CoffeeInstanceContainer(QObject *parent) : QObject(parent)
{
    //We do this to av
    instanceAnchor = new QObject();

    int i,j;

    for(i=0;i<100;i++)
        for(j=0;j<100;j++)
            instances.append(new InstanceData(
                                 glm::vec3(i,0,j),
                                 glm::quat(1,0,0,0),
                                 glm::vec3(1,1,1),
                                 instanceAnchor));
}

CoffeeInstanceContainer::~CoffeeInstanceContainer()
{
    //InstanceData is not a QObject, and should not be listed in the inspector.
    //Doing this would murder performance when there are 10 000 or more objects to add every 1 seconds.
    //The details of these objects are not interesting in most cases.
    delete instanceAnchor;
    for(InstanceData* d : instances)
        delete d;
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
        d.append(RenderingMethods::translateObjectMatrix(i->getPos()->getValue(),
                                                         i->getRot()->getValue(),
                                                         i->getScale()->getValue()));

    return d;
}
