#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "general/common.h"
#include "general/data/numbercontainer.h"
#include "physicsdescriptor.h"

class PhysicsObject : public QObject
{
    Q_PROPERTY(QString position READ getStringPosition)
    Q_PROPERTY(QString rotation READ getStringRotation)
    Q_PROPERTY(QString scale READ getStringScale)
    Q_PROPERTY(float mass READ getMass)
    Q_PROPERTY(float friction READ getFriction)
    Q_PROPERTY(float restitution READ getRestitution)
    Q_PROPERTY(int shape READ getShape)

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
    QPointer<NumberContainer<glm::quat>> getPhysicalRotation();

    QString getStringPosition() const;
    QString getStringRotation() const;
    QString getStringScale() const;
    float getMass() const;
    float getFriction() const;
    float getRestitution() const;
    bool getActivation() const;
    PhysicsDescriptor::PhysicalShape getShape() const;

    void *getPhysicspointer();
    void setPhysicspointer(void *value);

    PhysicsDescriptor *getDescr();
    void setDescr(PhysicsDescriptor *value);

public slots:
    void updatePosition(glm::vec3 p);
    void updateVelocity(glm::vec3 p);
    void updateAcceleration(glm::vec3 p);

    void updateRotation(glm::quat r);
    void updateAngularVelocity(glm::quat r);

signals:
    void deleteObject(void* pt);

private:
    PhysicsDescriptor* descr = nullptr;
    void* physicspointer = nullptr;

    QPointer<NumberContainer<glm::vec3>> v_position;
    QPointer<NumberContainer<glm::quat>> v_physics_rotation;
};

#endif // PHYSICSOBJECT_H
