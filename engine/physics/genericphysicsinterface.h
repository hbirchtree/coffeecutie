#ifndef GENERICPHYSICSINTERFACE_H
#define GENERICPHYSICSINTERFACE_H

#include "general/common.h"
#include "general/data/mutabledatacontainer.h"

class PhysicsObject;

class GenericPhysicsInterface
{
public:
    virtual QString systemName();

private:
    GenericPhysicsInterface(QObject *parent);
};

class VectorVariant : public QObject
{
    Q_PROPERTY(VectorType type READ type)
    Q_PROPERTY(QVariantList quat READ quat WRITE setQuat)
    Q_PROPERTY(QVariantList vec3 READ vec3 WRITE setVec3)
    Q_PROPERTY(float scalar READ scalar WRITE setScalar)

    Q_OBJECT
public:

    enum VectorType {
        None, Scalar, Vector3, Quaternion
    };
    Q_ENUMS(VectorType)

    VectorVariant(QObject* parent);
    VectorVariant(QObject* parent,glm::vec3 val);
    VectorVariant(QObject* parent,glm::quat val);

    VectorType type() const;

    glm::vec3 getRawVec3() const;
    glm::quat getRawQuat() const;

    QVariantList quat() const;
    QVariantList vec3() const;
    float scalar() const;

public slots:
    void setRawQuat(glm::quat quat);
    void setRawVec3(glm::vec3 vec3);

    void setQuat(QVariantList quat);
    void setVec3(QVariantList vec3);
    void setScalar(float scalar);

private:
    QVector<float> v;
};

class CoffeePhysicsEvent : public QObject {

public:
    enum PropertyType {
        //Events: sent from the physics system. The physics system shall not react to these.
        EventUpdateProperties, //for per-tick updates. less function calls.
        EventCollision,

        //Actions: Alterations done to the laws of physics. Be careful out there.
        ActionApplyForce, //applyCentralForce(vec3)
        ActionApplyRelativeForce, //applyForce(vec3,vec3)
        ActionApplyImpulse,
        ActionApplyRelativeImpulse,
        ActionApplyTorque,
        ActionApplyTorqueImpulse,

        ActionSetTransform, //position, rotation. no scale. this is not thermodynamics apparently.

        ActionSetPosition,
        ActionSetVelocity,
        ActionSetAcceleration,

        ActionSetAngularVelocity,
        ActionSetOrientation,

        ActionSetActivationState,
        ActionSetAngularFactor,
        ActionSetFriction,
        ActionSetRollingFriction,
        ActionSetLinearFactor
    };
    Q_ENUMS(PropertyType)

private:
    Q_PROPERTY(PropertyType type READ type WRITE setType)
    Q_PROPERTY(QObjectList targets READ targets)
    Q_PROPERTY(QVariantList data READ getData WRITE setData)

    Q_OBJECT

    PropertyType m_type;
    QVariantList data;
    QList<PhysicsObject*> m_target;

public:
    CoffeePhysicsEvent(QObject* parent);

    PropertyType type() const {return m_type;}
    QObjectList targets();
    QVariantList getData();
    QList<PhysicsObject*> *targetsList();

public slots:
    //the event can affect multiple objects
    void addTarget(QObject* t);
    int removeTarget(QObject *t);
    void clearTargets();

    //the data is set as a list of values. ordering of the elements decides their usage.
    //I know it's silly, but for now we'll let it go.
    //we should be able to swap this under the hood if we need to.
    void setData(const QVariantList &v);

    void setType(PropertyType type){m_type = type;}
};

#endif // GENERICPHYSICSINTERFACE_H
