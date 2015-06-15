#ifndef GENERICPHYSICSINTERFACE_H
#define GENERICPHYSICSINTERFACE_H

#include "general/common.h"
#include "general/data/mutabledatacontainer.h"
#include "engine/scripting/qscriptvectorvalue.h"

class GenericPhysicsInterface
{
public:
    virtual QString systemName();

private:
    GenericPhysicsInterface(QObject *parent);
};

class PhysicalPropertyClass :public QObject {
    //To get the enum as a static meta-object
    Q_OBJECT
public:
    PhysicalPropertyClass();
    enum PhysicsProperty {
        PhysProp_Pos,
        PhysProp_Orientation,
        PhysProp_AngularVelocity,
        PhysProp_Gravity,
        PhysProp_Velocity,
        PhysProp_Force,
        PhysProp_Impulse,
        PhysProp_Activation
    };
    Q_ENUMS(PhysicsProperty)
};

class CoffeePhysicsEvent : public QObject {

    enum PropertyType {
        UpdateProperties, //for per-tick updates. less calls.

        EventCollision,

        ActionApplyAcceleration, //applyCentralForce(vec3)
        ActionApplyRelativeAcceleration, //applyForce(vec3,vec3)
        ActionApplyImpulse,
        ActionApplyRelativeImpulse,
        ActionApplyTorque,
        ActionApplyTorqueImpulse,

        ActionSetActivationState,
        ActionSetVelocity,
        ActionSetAcceleration,
        ActionSetAngularFactor,
        ActionSetFriction,
        ActionSetRollingFriction,
        ActionSetLinearFactor
    };
    Q_ENUMS(PropertyType)

    Q_PROPERTY(PropertyType type READ type WRITE setType)
    Q_PROPERTY(float float1 READ getFloatVal1 WRITE setFloatVal1)
    Q_PROPERTY(bool bool1 READ getBoolVal1 WRITE setBoolVal1)

    Q_OBJECT

    PropertyType m_type;

public:
    CoffeePhysicsEvent(QObject* parent) : QObject(parent){}

    PropertyType type() const {return m_type;}

public slots:
    void setType(PropertyType type){m_type = type;}
};

class VectorVariant : public QObject
{
public:
    typedef float numbertype;

    enum VectorType {
        NumberType,Vector3Type,Vector4Type,QuaternionType
    };

    VectorVariant(QObject* parent,const glm::vec3 &v) : QObject(parent){
        m_data = new MutableDataContainer<glm::vec3>(this,v);
    }
    VectorVariant(QObject* parent,const glm::vec4 &v) : QObject(parent){
        m_data = new MutableDataContainer<glm::vec4>(this,v);
    }
    VectorVariant(QObject* parent,const glm::quat &v) : QObject(parent){
        m_data = new MutableDataContainer<glm::quat>(this,v);
    }

    VectorType type(){
        return m_type;
    }

    void setValue(numbertype val){
        if(m_data)
            m_data->deleteLater();
        m_data = new MutableDataContainer<numbertype>(this,val);
        m_type = NumberType;
    }
    void setValue(glm::vec3 val){
        if(m_data)
            m_data->deleteLater();
        m_data = new MutableDataContainer<glm::vec3>(this,val);
        m_type = Vector3Type;
    }
    void setVec(numbertype x, numbertype y, numbertype z){
        if(m_data)
            m_data->deleteLater();
        m_data = new MutableDataContainer<glm::vec3>(this,glm::vec3(x,y,z));
        m_type = Vector3Type;
    }
    void setValue(glm::vec4 val){
        if(m_data)
            m_data->deleteLater();
        m_data = new MutableDataContainer<glm::vec4>(this,val);
        m_type = Vector4Type;
    }
    void setVec(numbertype x, numbertype y, numbertype z,numbertype w){
        if(m_data)
            m_data->deleteLater();
        m_data = new MutableDataContainer<glm::vec4>(this,glm::vec4(x,y,z,w));
        m_type = Vector4Type;
    }
    void setValue(glm::quat val){
        if(m_data)
            m_data->deleteLater();
        m_data = new MutableDataContainer<glm::quat>(this,val);
        m_type = QuaternionType;
    }
    void setQuat(numbertype w, numbertype x, numbertype y, numbertype z){
        if(m_data)
            m_data->deleteLater();
        m_data = new MutableDataContainer<glm::quat>(this,glm::quat(w,x,y,z));
        m_type = QuaternionType;
    }

    numbertype toNumber() const{
        MutableDataContainer<numbertype>* p = dynamic_cast<MutableDataContainer<numbertype>*>(m_data.data());
        if(p)
            return p->getValue();
        return numbertype();
    }
    glm::vec3 toVector3() const{
        MutableDataContainer<glm::vec3>* p = dynamic_cast<MutableDataContainer<glm::vec3>*>(m_data.data());
        if(p)
            return p->getValue();
        return glm::vec3();
    }
    glm::vec4 toVector4() const{
        MutableDataContainer<glm::vec4>* p = dynamic_cast<MutableDataContainer<glm::vec4>*>(m_data.data());
        if(p)
            return p->getValue();
        return glm::vec4();
    }
    glm::quat toQuaternion() const{
        MutableDataContainer<glm::quat>* p = dynamic_cast<MutableDataContainer<glm::quat>*>(m_data.data());
        if(p)
            return p->getValue();
        return glm::quat();
    }

private:
    VectorVariant(QObject* parent) : QObject(parent){}
    QPointer<QObject> m_data;
    VectorType m_type;
};

#endif // GENERICPHYSICSINTERFACE_H
