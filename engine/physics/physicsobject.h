#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "general/common.h"
#include "general/data/numbercontainer.h"
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

    QPointer<NumberContainer<glm::vec3>> getPositionObject();
    QPointer<NumberContainer<glm::quat>> getPhysicalRotation();

    QVariantList getPositionValue() const;
    QVariantList getRotationValue() const;
    QVariantList getScaleValue() const;

    void *getPhysicspointer();
    void setPhysicspointer(void *value);

    PhysicsDescriptor *getDescr();
    void setDescr(PhysicsDescriptor *value);

public slots:
    void setPosition(float x, float y, float z);
    void setRotation(float x, float y, float z);
    void applyForce(float x, float y, float z);
    void applyImpulse(float x, float y, float z);

    void updatePosition(glm::vec3 p);
    void updateVelocity(glm::vec3 p);
    void updateAcceleration(glm::vec3 p);

    void updateRotation(glm::quat r);
    void updateAngularVelocity(glm::quat r);

signals:
    void propertyModified(PhysicsObject* target, GenericPhysicsInterface::PhysicsProperty prop, VectorVariant *data);

    void deleteObject(void* pt);

private:
    PhysicsDescriptor* descr = nullptr;
    void* physicspointer = nullptr;

    QPointer<NumberContainer<glm::vec3>> v_position;
    QPointer<NumberContainer<glm::vec3>> v_scale;
    QPointer<NumberContainer<glm::quat>> v_physics_rotation;
};

#endif // PHYSICSOBJECT_H
