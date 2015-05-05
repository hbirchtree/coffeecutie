#include "physicsobject.h"

PhysicsObject::PhysicsObject(QObject *parent) : QObject(parent){
    v_position = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    v_physics_scale = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    v_physics_rotation = new NumberContainer<glm::quat>(this,glm::quat(0,0,0,0));
    v_physics_linear_factor = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    v_physics_inertia = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
    f_physics_mass = new NumberContainer<float>(this,0.f);
    f_physics_restitution = new NumberContainer<float>(this,0.f);
    f_physics_friction = new NumberContainer<float>(this,0.f);
}

PhysicsObject::PhysicsObject(QPointer<PhysicsObject> object){
    v_position->bindValue(object->getPositionObject());
    v_physics_scale->bindValue(object->getPhysicalScale());
    v_physics_inertia->bindValue(object->getPhysicalInertia());
    v_physics_linear_factor->bindValue(object->getPhysicalLinearFactor());
    v_physics_rotation->bindValue(object->getPhysicalRotation());
    f_physics_friction->bindValue(object->getPhysicalFriction());
    f_physics_mass->bindValue(object->getPhysicalMass());
    f_physics_restitution->bindValue(object->getPhysicalRestitution());
    b_objectDeactivation = object->isObjectDeactivating();
    b_notifyForce = object->isNotifyingForce();
    b_updateRotation = object->isUpdatingRotation();
    s_collisionFile = object->getCollisionFile();
    e_physics_type = object->getPhysicsType();
}

PhysicsObject::~PhysicsObject(){
    for(QMetaObject::Connection cnct : connections)
        disconnect(cnct);
}

QPointer<NumberContainer<glm::vec3> > PhysicsObject::getPositionObject(){
    return v_position; //We return the address of the vector's container
}

QPointer<NumberContainer<glm::vec3> > PhysicsObject::getPhysicalScale(){
    return v_physics_scale;
}

QPointer<NumberContainer<glm::quat> > PhysicsObject::getPhysicalRotation(){
    return v_physics_rotation;
}

QPointer<NumberContainer<glm::vec3> > PhysicsObject::getPhysicalLinearFactor(){
    return v_physics_linear_factor;
}

QPointer<NumberContainer<glm::vec3> > PhysicsObject::getPhysicalInertia(){
    return v_physics_inertia;
}

QPointer<NumberContainer<float> > PhysicsObject::getPhysicalMass(){
    return f_physics_mass;
}

QPointer<NumberContainer<float> > PhysicsObject::getPhysicalRestitution(){
    return f_physics_restitution;
}

QPointer<NumberContainer<float>> PhysicsObject::getPhysicalFriction(){
    return f_physics_friction;
}

bool PhysicsObject::isObjectDeactivating(){
    return b_objectDeactivation;
}

bool PhysicsObject::isNotifyingForce(){
    return b_notifyForce;
}

bool PhysicsObject::isUpdatingRotation(){
    return b_updateRotation;
}

QString PhysicsObject::getCollisionFile(){
    return s_collisionFile;
}

PhysicsObject::PhysicsType PhysicsObject::getPhysicsType(){
    return e_physics_type;
}
