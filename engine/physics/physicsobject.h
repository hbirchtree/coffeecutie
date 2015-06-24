#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include "engine/physics/genericphysicsinterface.h"

class PhysicsDescriptor;

class PhysicsObject : public QObject
{
    Q_PROPERTY(QVariantList position READ getPositionValue)
    Q_PROPERTY(QVariantList rotation READ getRotationValue)
    Q_PROPERTY(QVariantList scale READ getScaleValue)

    Q_OBJECT
public:
    PhysicsObject(QObject* parent);
    PhysicsObject(QPointer<PhysicsObject> object);
    ~PhysicsObject();

    QPointer<Vector3Value> getPositionObject();
    QPointer<QuatValue> getPhysicalRotation();

    QVariantList getPositionValue() const;
    QVariantList getRotationValue() const;
    QVariantList getScaleValue() const;

    void *getPhysicspointer();
    void setPhysicspointer(void *value);

    PhysicsDescriptor *getDescr();

public slots:
    void updatePosition(glm::vec3 p);
    void updateVelocity(glm::vec3 p);
    void updateAcceleration(glm::vec3 p);

    void updateRotation(glm::quat r);
    void updateAngularVelocity(glm::quat r);

    void setDescr(PhysicsDescriptor *value);

    void modifyPhysics(CoffeePhysicsEvent* ev);

signals:
    void propertyModified(CoffeePhysicsEvent* ev);

    void deleteObject(void* pt);

private:
    PhysicsDescriptor* descr = nullptr;
    void* physicspointer = nullptr;

    QPointer<Vector3Value> v_position;
    QPointer<Vector3Value> v_scale;
    QPointer<QuatValue> v_physics_rotation;
};

#endif // PHYSICSOBJECT_H
