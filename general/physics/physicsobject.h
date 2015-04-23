#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include <QObject>
#include "general/vectors/vector3container.h"
#include "general/floatcontainer.h"

class PhysicsObject : public QObject
{
    Q_OBJECT
public:
    enum PhysicsType {
        PHYS_TYPE_NONE,PHYS_TYPE_BOX,
        PHYS_TYPE_BALL,PHYS_TYPE_TRIMESH,
        PHYS_TYPE_CYLINDER,PHYS_TYPE_CAPSULE,
        PHYS_TYPE_HMAP
    };
    PhysicsObject(){

    }
    PhysicsObject(QPointer<PhysicsObject> object){
        v_position.bindValue(object->getPositionObject());
        v_physics_scale.bindValue(object->getPhysicalScale());
        v_physics_inertia.bindValue(object->getPhysicalInertia());
        v_physics_linear_factor.bindValue(object->getPhysicalLinearFactor());
        v_physics_rotation.bindValue(object->getPhysicalRotation());
        f_physics_friction.bindValue(object->getPhysicalFriction());
        f_physics_mass.bindValue(object->getPhysicalMass());
        f_physics_restitution.bindValue(object->getPhysicalRestitution());
        b_objectDeactivation = object->isObjectDeactivating();
        b_notifyForce = object->isNotifyingForce();
        b_updateRotation = object->isUpdatingRotation();
        s_collisionFile = object->getCollisionFile();
        e_physics_type = object->getPhysicsType();
    }
    ~PhysicsObject(){

    }

    QPointer<Vector3Container> getPositionObject(){
        return &v_position; //We return the address of the vector's container
    }
    QPointer<Vector3Container> getPhysicalScale(){
        return &v_physics_scale;
    }
    QPointer<Vector3Container> getPhysicalRotation(){
        return &v_physics_rotation;
    }
    QPointer<Vector3Container> getPhysicalLinearFactor(){
        return &v_physics_linear_factor;
    }
    QPointer<Vector3Container> getPhysicalInertia(){
        return &v_physics_inertia;
    }

    QPointer<FloatContainer> getPhysicalMass(){
        return &f_physics_mass;
    }
    QPointer<FloatContainer> getPhysicalRestitution(){
        return &f_physics_restitution;
    }
    QPointer<FloatContainer> getPhysicalFriction(){
        return &f_physics_friction;
    }
    bool isObjectDeactivating(){
        return b_objectDeactivation;
    }
    bool isNotifyingForce(){
        return b_notifyForce;
    }
    bool isUpdatingRotation(){
        return b_updateRotation;
    }
    QString getCollisionFile(){
        return s_collisionFile;
    }
    PhysicsType getPhysicsType(){
        return e_physics_type;
    }

protected:
    PhysicsType e_physics_type;
    QString s_collisionFile; //For heightmap or triangle mesh
    bool b_objectDeactivation;
    bool b_notifyForce;
    bool b_updateRotation;
    Vector3Container v_position; //Shared between 3D and physics
    Vector3Container v_physics_scale; //Different parameters pulled according to the physics type selected
    Vector3Container v_physics_rotation;
    Vector3Container v_physics_linear_factor;
    Vector3Container v_physics_inertia;
    FloatContainer f_physics_mass;
    FloatContainer f_physics_restitution;
    FloatContainer f_physics_friction;
};

#endif // PHYSICSOBJECT_H
