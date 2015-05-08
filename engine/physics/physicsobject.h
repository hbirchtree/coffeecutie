#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "general/common.h"
#include "general/data/numbercontainer.h"

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

    QPointer<NumberContainer<glm::vec3>> getPositionObject();
    QPointer<NumberContainer<glm::vec3>> getPhysicalScale();
    QPointer<NumberContainer<glm::quat>> getPhysicalRotation();
    QPointer<NumberContainer<glm::vec3>> getPhysicalLinearFactor();
    QPointer<NumberContainer<glm::vec3>> getPhysicalInertia();

    QPointer<NumberContainer<float>> getPhysicalMass();
    QPointer<NumberContainer<float>> getPhysicalRestitution();
    QPointer<NumberContainer<float>> getPhysicalFriction();
    bool isObjectDeactivating();
    bool isNotifyingForce();
    bool isUpdatingRotation();
    QString getCollisionFile();
    PhysicsType getPhysicsType();

    void *getPhysicspointer();
    void setPhysicspointer(void *value);

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
    void* physicspointer = nullptr;
    QList<QMetaObject::Connection> connections;

    PhysicsType e_physics_type;
    QString s_collisionFile; //For heightmap or triangle mesh
    bool b_objectDeactivation;
    bool b_notifyForce;
    bool b_updateRotation;
    QPointer<NumberContainer<glm::vec3>> v_position; //Shared between 3D and physics
    QPointer<NumberContainer<glm::vec3>> v_physics_scale; //Different parameters pulled according to the physics type selected
    QPointer<NumberContainer<glm::quat>> v_physics_rotation;
    QPointer<NumberContainer<glm::vec3>> v_physics_linear_factor;
    QPointer<NumberContainer<glm::vec3>> v_physics_inertia;
    QPointer<NumberContainer<float>> f_physics_mass;
    QPointer<NumberContainer<float>> f_physics_restitution;
    QPointer<NumberContainer<float>> f_physics_friction;
};

#endif // PHYSICSOBJECT_H
