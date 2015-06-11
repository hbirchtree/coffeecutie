#include "physicsobject.h"

#include "physicsdescriptor.h"

PhysicsObject::PhysicsObject(QObject *parent) : QObject(parent){
    descr = new PhysicsDescriptor(this);
    v_position = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    v_position->setObjectName("position");
    v_physics_rotation = new NumberContainer<glm::quat>(this,glm::quat(1,0,0,0));
    v_physics_rotation->setObjectName("orientation");
    v_scale = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    v_scale->setObjectName("scale");
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

QVariantList PhysicsObject::getPositionValue() const
{
    glm::vec3 v = v_position->getValue();
    return QVariantList() << v.x << v.y << v.z;
}

QVariantList PhysicsObject::getRotationValue() const
{
    glm::quat v = v_physics_rotation->getValue();
    return QVariantList() << v.w << v.x << v.y << v.z;
}

QVariantList PhysicsObject::getScaleValue() const
{
    glm::vec3 v = v_position->getValue();
    return QVariantList() << v.x << v.y << v.z;
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
    if(value)
        descr = value;
}

void PhysicsObject::setPosition(float x, float y, float z)
{
    propertyModified(this,
                     GenericPhysicsInterface::PhysProp_Pos,
                     new VectorVariant(this,glm::vec3(x,y,z)));
}

void PhysicsObject::setRotation(float x, float y, float z)
{
    propertyModified(this,
                     GenericPhysicsInterface::PhysProp_Orientation,
                     new VectorVariant(this,glm::quat(glm::vec3(x,y,z))));
}

void PhysicsObject::applyForce(float x, float y, float z)
{
    propertyModified(this,
                     GenericPhysicsInterface::PhysProp_Force,
                     new VectorVariant(this,glm::vec3(x,y,z)));
}

void PhysicsObject::applyImpulse(float x, float y, float z)
{
    propertyModified(this,
                     GenericPhysicsInterface::PhysProp_Impulse,
                     new VectorVariant(this,glm::vec3(x,y,z)));
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
