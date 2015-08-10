#ifndef GENERICPHYSICSINTERFACE_H
#define GENERICPHYSICSINTERFACE_H

#include "general/data/coffeepropertycontainer.h"
#include "glm/glm/common.hpp"
#include "engine/scripting/qscriptvectorvalue.h"

class PhysicsObject;

class CoffeePhysicsEvent : public CoffeePropertyContainer {
    Q_OBJECT

public:
    enum PhysicShape {
        NullShape, //We want a single property as

        SphereShape,BoxShape,
        CylinderShape,CapsuleShape,
        ConeShape,

        StaticPlaneShape,

        TriangleMesh,
        ConvexShape,ConvexHullShape,

        HeightmapShape,

        CompoundShape
    };
    Q_ENUMS(PhysicShape)

    enum PropertyIntent {
        NullIntent, //seen as invalid

        CreateIntent, //if it is sent to create an object
        DefineIntent, //If it is sent to define a property of an object
        RemoveIntent, //if we want to remove objects
    };
    Q_ENUMS(PropertyIntent)

    enum PropertyEnum {
        NullProperty, //A null property is not allowed

        ShapeProperty, //only valid if intent is CreateIntent

        //Forces
        ForceApplication,
        RelativeForceApplication,
        ImpulseApplication,
        RelativeImpulseApplication,
        TorqueApplication,
        TorqueImpulseApplication,

        RelativePositionProperty, //For forces

        //Just the transform, position and orientation
        TransformProperty,
        ScaleProperty,

        //Positional
        PositionProperty,
        VelocityProperty,
        AccelerationProperty,

        CMPositionProperty, //Center of Mass position

        //Orientation
        OrientationProperty,
        AngularVelocityProperty,

        //Misc. properties for the object
        MassProperty,
        LocalInertiaProperty,
        ActivationStateProperty,
        AngularFactorProperty,
        FrictionProperty,
        RestitutionProperty,
        RollingFrictionProperty,
        LinearFactorProperty,
        PlaneConstantProperty
    };
    Q_ENUMS(PropertyEnum)
    Q_DECLARE_FLAGS(PropertyFlags,PropertyEnum)

    Q_PROPERTY(uint propertyCombination READ propertyCombination)
    Q_PROPERTY(PropertyIntent intent READ intent WRITE setIntent)
    Q_PROPERTY(QObjectList targets READ targets WRITE setTargets)
    Q_PROPERTY(bool dispose READ dispose WRITE setDispose)

public:
    CoffeePhysicsEvent(QObject* parent);
    uint32_t propertyCombination() const;

    QList<uint32_t> getPropertyList() const;

    uint32_t getInt(PropertyEnum property);
    ScalarDataType getScalar(PropertyEnum property);
    glm::vec3 getVector3(PropertyEnum property);
    glm::quat getQuaternion(PropertyEnum property);

    QObjectList targets() const;
    QList<PhysicsObject*> physicsTargets() const;

    PropertyIntent intent() const;

    bool dispose() const;

public slots:
    void setInt(PropertyEnum property, quint32 value); //used for enum and similar
    void setScalar(PropertyEnum property, ScalarValue* value);
    void setVector3(PropertyEnum property, Vector3Value* vector);
    void setQuaternion(PropertyEnum property, QuatValue* quaternion);

    void setTargets(QObjectList targets);

    void setIntent(PropertyIntent intent);

    void setDispose(bool dispose);

private:
    void setProperty(PropertyEnum property, uint64_t size, void* data);
    property_takeout getProperty(PropertyEnum property);

    QFlags<PropertyFlags> m_propertyCombination = 0;
    QHash<uint32_t, uint32_t> m_propertyMapping; // property : id
    uint8_t m_intent = DefineIntent;

    QObjectList m_targets;
    bool m_dispose = false;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CoffeePhysicsEvent::PropertyFlags)

#endif // GENERICPHYSICSINTERFACE_H
