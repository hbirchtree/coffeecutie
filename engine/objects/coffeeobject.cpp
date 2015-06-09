#include "coffeeobject.h"

CoffeeObject::CoffeeObject(QObject *parent)
{
    vposition = new NumberContainer<glm::vec3>(parent,glm::vec3(0,0,0));
    qrotation = new NumberContainer<glm::quat>(parent,glm::quat(1,0,0,0));
    vscale = new NumberContainer<glm::vec3>(parent,glm::vec3(1,1,1));
}

CoffeeObject::~CoffeeObject()
{
}

NumberContainer<glm::vec3> *CoffeeObject::position(){
    return vposition;
}

NumberContainer<glm::quat> *CoffeeObject::rotation(){
    return qrotation;
}

NumberContainer<glm::vec3> *CoffeeObject::scale(){
    return vscale;
}

PhysicsObject *CoffeeObject::physics(){
    return physical;
}

void CoffeeObject::setPhysicsObject(PhysicsObject *obj)
{
    this->physical = obj;
}


