#include "coffeeobjectorganizer.h"

CoffeeObjectOrganizer::CoffeeObjectOrganizer(QObject *parent) : QObject(parent)
{

}

int CoffeeObjectOrganizer::removeObject(QString name)
{
    return objects.remove(name);
}

bool CoffeeObjectOrganizer::createObject(QString name)
{
    if(objects.contains(name))
        return false;
    CoffeeObject* object = new CoffeeObject(this);
    object->setObjectName(name);
    objects.insert(name,object);
    return true;
}

CoffeeObject *CoffeeObjectOrganizer::getObject(QString name)
{
    return objects.value(name);
}
