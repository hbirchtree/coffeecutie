#include "coffeeobject.h"

CoffeeObject::CoffeeObject(QObject *parent) : QObject(parent)
{
    vposition = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    qrotation = new NumberContainer<glm::quat>(this,glm::quat(1,0,0,0));
    vscale = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
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


