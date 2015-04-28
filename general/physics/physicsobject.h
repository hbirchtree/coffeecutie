#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "general/common.h"
#include "general/floatcontainer.h"
#include "general/vectors/vector3container.h"

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
    PhysicsObject(QObject* parent);
    PhysicsObject(QPointer<PhysicsObject> object);
    ~PhysicsObject();

    QPointer<Vector3Container> getPositionObject();
    QPointer<Vector3Container> getPhysicalScale();
    QPointer<Vector3Container> getPhysicalRotation();
    QPointer<Vector3Container> getPhysicalLinearFactor();
    QPointer<Vector3Container> getPhysicalInertia();

    QPointer<FloatContainer> getPhysicalMass();
    QPointer<FloatContainer> getPhysicalRestitution();
    QPointer<FloatContainer> getPhysicalFriction();
    bool isObjectDeactivating();
    bool isNotifyingForce();
    bool isUpdatingRotation();
    QString getCollisionFile();
    PhysicsType getPhysicsType();

signals:
    void phys_activationChanged(bool);
    void phys_positionChanged(glm::vec3);
    void phys_rotationChanged(glm::vec3);
    void phys_scaleChanged(glm::vec3);
    void phys_linearFactorChanged(glm::vec3);
    void phys_inertiaChanged(glm::vec3);
    void phys_massChanged(float);
    void phys_restitutionChanged(float);
    void phys_frictionChanged(float);

protected:
    QList<QMetaObject::Connection> connections;

    PhysicsType e_physics_type;
    QString s_collisionFile; //For heightmap or triangle mesh
    bool b_objectDeactivation;
    bool b_notifyForce;
    bool b_updateRotation;
    QPointer<Vector3Container> v_position; //Shared between 3D and physics
    QPointer<Vector3Container> v_physics_scale; //Different parameters pulled according to the physics type selected
    QPointer<Vector3Container> v_physics_rotation;
    QPointer<Vector3Container> v_physics_linear_factor;
    QPointer<Vector3Container> v_physics_inertia;
    QPointer<FloatContainer> f_physics_mass;
    QPointer<FloatContainer> f_physics_restitution;
    QPointer<FloatContainer> f_physics_friction;
};

#endif // PHYSICSOBJECT_H
