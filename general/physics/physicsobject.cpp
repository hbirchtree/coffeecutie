#include "physicsobject.h"



PhysicsObject::PhysicsObject(QObject *parent) : QObject(parent){
    v_position = new Vector3Container(this);
    v_position = new Vector3Container(this);
    v_physics_scale = new Vector3Container(this);
    v_physics_rotation = new Vector3Container(this);
    v_physics_linear_factor = new Vector3Container(this);
    v_physics_inertia = new Vector3Container(this);
    f_physics_mass = new FloatContainer(this);
    f_physics_restitution = new FloatContainer(this);
    f_physics_friction = new FloatContainer(this);

    connections.append(connect(v_position.data(),&Vector3Container::valueChanged,[=](glm::vec3 v){
        phys_positionChanged(v);
    }));
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

QPointer<Vector3Container> PhysicsObject::getPositionObject(){
    return v_position; //We return the address of the vector's container
}

QPointer<Vector3Container> PhysicsObject::getPhysicalScale(){
    return v_physics_scale;
}

QPointer<Vector3Container> PhysicsObject::getPhysicalRotation(){
    return v_physics_rotation;
}

QPointer<Vector3Container> PhysicsObject::getPhysicalLinearFactor(){
    return v_physics_linear_factor;
}

QPointer<Vector3Container> PhysicsObject::getPhysicalInertia(){
    return v_physics_inertia;
}

QPointer<FloatContainer> PhysicsObject::getPhysicalMass(){
    return f_physics_mass;
}

QPointer<FloatContainer> PhysicsObject::getPhysicalRestitution(){
    return f_physics_restitution;
}

QPointer<FloatContainer> PhysicsObject::getPhysicalFriction(){
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
