#include "genericphysicsinterface.h"

#include "engine/physics/physicsobject.h"

CoffeePhysicsEvent::CoffeePhysicsEvent(QObject *parent) : QObject(parent){}

QObjectList CoffeePhysicsEvent::targets()
{
    QObjectList l;
    for(PhysicsObject* p : m_target)
        l.append(p);
    return l;
}

QVariantList CoffeePhysicsEvent::getData()
{
    return data;
}

QList<PhysicsObject *> *CoffeePhysicsEvent::targetsList()
{
    return &m_target;
}

void CoffeePhysicsEvent::addTarget(QObject *t)
{
    PhysicsObject* p =qobject_cast<PhysicsObject*>(t);
    if(!p)
        return;
    m_target.append(p);
}

int CoffeePhysicsEvent::removeTarget(QObject *t)
{
    PhysicsObject* p =qobject_cast<PhysicsObject*>(t);
    if(!p)
        return 0;
    return m_target.removeAll(p);
}

void CoffeePhysicsEvent::clearTargets()
{
    m_target.clear();
}

void CoffeePhysicsEvent::setData(const QVariantList &v)
{
    data = v;
}

VectorVariant::VectorVariant(QObject *parent) : QObject(parent){}

VectorVariant::VectorVariant(QObject *parent, glm::vec3 val) :VectorVariant(parent){
    setRawVec3(val);
}

VectorVariant::VectorVariant(QObject *parent, glm::quat val) :VectorVariant(parent){
    setRawQuat(val);
}

VectorVariant::VectorType VectorVariant::type() const
{
    switch(v.size()){
    case 1:
        return Scalar;
    case 3:
        return Vector3;
    case 4:
        return Quaternion; //we might add another bit to differentiate a vec4 and quat
    default:
        return None;
    }
}

glm::vec3 VectorVariant::getRawVec3() const
{
    if(type()!=Vector3||v.size()!=3)
        return glm::vec3();
    return glm::vec3(v.at(0),v.at(1),v.at(2));
}

glm::quat VectorVariant::getRawQuat() const
{
    if(type()!=Quaternion)
        return glm::quat();
    return glm::quat(v.at(0),v.at(1),v.at(2),v.at(3));
}

QVariantList VectorVariant::quat() const
{
    if(type()!=Quaternion)
        return QVariantList();
    return QVariantList() << v.at(0) << v.at(1) << v.at(2) << v.at(3);
}

QVariantList VectorVariant::vec3() const
{
    if(type()!=Vector3)
        return QVariantList();
    return QVariantList() << v.at(0) << v.at(1) << v.at(2);
}

float VectorVariant::scalar() const
{
    if(type()!=Scalar)
        return 0;
    return v.at(0);
}

void VectorVariant::setRawQuat(glm::quat quat)
{
    v << quat.w << quat.x << quat.y << quat.z;
}

void VectorVariant::setRawVec3(glm::vec3 vec3)
{
    v << vec3.x << vec3.y << vec3.z;
}

void VectorVariant::setQuat(QVariantList quat)
{
    if(quat.size()!=4)
        return;
    v.clear();
    for(QVariant v : quat)
        this->v.append(v.toFloat());
}

void VectorVariant::setVec3(QVariantList vec3)
{
    if(vec3.size()!=3)
        return;
    v.clear();
    for(QVariant v : vec3)
        this->v.append(v.toFloat());
}

void VectorVariant::setScalar(float scalar)
{
    v.clear();
    v.append(scalar);
}
