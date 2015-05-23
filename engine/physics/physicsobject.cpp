#include "physicsobject.h"

PhysicsObject::PhysicsObject(QObject *parent) : QObject(parent){
    descr = new PhysicsDescriptor();
    v_position = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    v_physics_rotation = new NumberContainer<glm::quat>(this,glm::quat(1,0,0,0));
}

PhysicsObject::PhysicsObject(QPointer<PhysicsObject> object){
    v_position->bindValue(object->getPositionObject());
    v_physics_rotation->bindValue(object->getPhysicalRotation());
}

PhysicsObject::~PhysicsObject(){
    delete descr;
    if(physicspointer)
        deleteObject(physicspointer);
}

QPointer<NumberContainer<glm::vec3> > PhysicsObject::getPositionObject(){
    return v_position;
}

QPointer<NumberContainer<glm::quat> > PhysicsObject::getPhysicalRotation(){
    return v_physics_rotation;
}

QString PhysicsObject::getStringPosition() const
{
    return QStringFunctions::toString(v_position->getValue());
}

QString PhysicsObject::getStringRotation() const
{
    return QStringFunctions::toString(v_physics_rotation->getValue());
}

QString PhysicsObject::getStringScale() const
{
    return QStringFunctions::toString(descr->scale());
}

float PhysicsObject::getMass() const
{
    return descr->mass();
}

float PhysicsObject::getFriction() const
{
    return descr->friction();
}

float PhysicsObject::getRestitution() const
{
    return descr->restitution();
}

bool PhysicsObject::getActivation() const
{
    return descr->activation();
}

PhysicsDescriptor::PhysicalShape PhysicsObject::getShape() const
{
    return descr->getShape();
}

void *PhysicsObject::getPhysicspointer()
{
    return physicspointer;
}

void PhysicsObject::setPhysicspointer(void *value)
{
    physicspointer = value;
}
PhysicsDescriptor *PhysicsObject::getDescr()
{
    return descr;
}

void PhysicsObject::setDescr(PhysicsDescriptor *value)
{
    descr = value;
}

void PhysicsObject::updatePosition(glm::vec3 p)
{
    v_position->setValue(p);
}

void PhysicsObject::updateVelocity(glm::vec3 p)
{
    v_position->setVelocity(p);
}

void PhysicsObject::updateAcceleration(glm::vec3 p)
{
    v_position->setAcceleration(p);
}

void PhysicsObject::updateRotation(glm::quat r)
{
    v_physics_rotation->setValue(r);
}

void PhysicsObject::updateAngularVelocity(glm::quat r)
{
    v_physics_rotation->setVelocity(r);
}


