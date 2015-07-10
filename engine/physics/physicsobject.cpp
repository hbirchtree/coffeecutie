#include "physicsobject.h"

#include "physicsdescriptor.h"

PhysicsObject::PhysicsObject(QObject *parent) : QObject(parent){
    descr = new PhysicsDescriptor(this);
    v_position = new Vector3Value(this,glm::vec3(0,0,0));
    v_position->setObjectName("position");
    v_physics_rotation = new QuatValue(this,glm::quat(1,0,0,0));
    v_physics_rotation->setObjectName("orientation");
    v_scale = new Vector3Value(this,glm::vec3(1,1,1));
    v_scale->setObjectName("scale");
}

PhysicsObject::PhysicsObject(QPointer<PhysicsObject> object){
    v_position->bindValue(object->getPositionObject());
    v_physics_rotation->bindValue(object->getPhysicalRotation());
}

PhysicsObject::~PhysicsObject(){
    if(physicspointer)
        deleteObject(physicspointer);
}

QPointer<Vector3Value> PhysicsObject::getPositionObject(){
    return v_position;
}

QPointer<QuatValue> PhysicsObject::getPhysicalRotation(){
    return v_physics_rotation;
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

QObject *PhysicsObject::getPositionQObject()
{
    return v_position;
}

QObject *PhysicsObject::getRotationQObject()
{
    return v_physics_rotation;
}

QObject *PhysicsObject::getScaleQObject()
{
    return v_scale;
}

void PhysicsObject::setDescr(PhysicsDescriptor *value)
{
    if(value)
        descr = value;
}

void PhysicsObject::modifyPhysics(CoffeePhysicsEvent *ev)
{
    propertyModified(ev);
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
//    v_physics_rotation->setValue(r);
    *v_physics_rotation = glm::normalize(r);
}

void PhysicsObject::updateAngularVelocity(glm::quat r)
{
    v_physics_rotation->setVelocity(r);
}
