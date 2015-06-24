#include "coffeeobject.h"

CoffeeObject::CoffeeObject(QObject *parent)
{
    vposition = new Vector3Value(parent,glm::vec3(0,0,0));
    qrotation = new QuatValue(parent,glm::quat(1,0,0,0));
    vscale = new Vector3Value(parent,glm::vec3(1,1,1));
}

CoffeeObject::~CoffeeObject()
{
}

Vector3Value *CoffeeObject::position(){
    return vposition;
}

QuatValue *CoffeeObject::rotation(){
    return qrotation;
}

Vector3Value *CoffeeObject::scale(){
    return vscale;
}

PhysicsObject *CoffeeObject::physics(){
    return physical;
}

void CoffeeObject::setPhysicsObject(PhysicsObject *obj)
{
    this->physical = obj;
}


