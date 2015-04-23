#include "vector3container.h"

Vector3Container::Vector3Container(QObject *parent) : QObject(parent)
{

}
Vector3Container::Vector3Container(QPointer<Vector3Container> bound){
    bindValue(bound);
}

Vector3Container::~Vector3Container()
{

}
void Vector3Container::bindValue(QPointer<Vector3Container> value){
    bound = value;
}

glm::vec3 Vector3Container::getValue(){
    glm::vec3 res = value;
    if(bound)
        res = bound->getValue();
    res += valueOffsetCallback();
    return res;
}
void Vector3Container::setValue(glm::vec3 value){
    unbindValue();
    this->value = value;
}
glm::vec3 Vector3Container::getVelocity()
{
    glm::vec3 res = velocity;
    if(bound!=NULL)
        res = bound->getValue();
    return res;
}

void Vector3Container::setVelocity(const glm::vec3 &value)
{
    unbindValue();
    velocity = value;
}
glm::vec3 Vector3Container::getAcceleration()
{
    glm::vec3 res = acceleration;
    if(bound!=NULL)
        res = bound->getAcceleration();
    return res;
}

void Vector3Container::setAcceleration(const glm::vec3 &value)
{
    unbindValue();
    acceleration = value;
}

std::function<glm::vec3()> Vector3Container::getValueOffset(){
    return valueOffsetCallback;
}
void Vector3Container::setValueOffset(std::function<glm::vec3()> func){
    this->valueOffsetCallback = func;
}
