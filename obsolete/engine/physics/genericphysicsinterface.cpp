#include "genericphysicsinterface.h"

#include "engine/physics/physicsobject.h"

CoffeePhysicsEvent::CoffeePhysicsEvent(QObject *parent) : CoffeePropertyContainer(parent){}

uint32_t CoffeePhysicsEvent::propertyCombination() const
{
    return m_propertyCombination;
}

QList<uint32_t> CoffeePhysicsEvent::getPropertyList() const
{
    return m_propertyMapping.keys();
}

CoffeePhysicsEvent::PropertyIntent CoffeePhysicsEvent::intent() const
{
    return (PropertyIntent)m_intent;
}

bool CoffeePhysicsEvent::dispose() const
{
    return m_dispose;
}

uint32_t CoffeePhysicsEvent::getInt(CoffeePhysicsEvent::PropertyEnum property)
{
    property_takeout d = getProperty(property);
    if(d.size==sizeof(uint32_t)){
        uint32_t r;
        memcpy(&r,d.data,d.size);
        return r;
    }else
        return 0;
}

ScalarDataType CoffeePhysicsEvent::getScalar(CoffeePhysicsEvent::PropertyEnum property)
{
    property_takeout d = getProperty(property);
    if(d.size==sizeof(ScalarDataType)){
        ScalarDataType r;
        memcpy(&r,d.data,d.size);
        return r;
    }else
        return ScalarDataType();
}

glm::vec3 CoffeePhysicsEvent::getVector3(CoffeePhysicsEvent::PropertyEnum property)
{
    property_takeout d = getProperty(property);
    if(d.size==sizeof(glm::vec3)){
        glm::vec3 r;
        memcpy(&r,d.data,d.size);
        return r;
    }else
        return glm::vec3();
}

glm::quat CoffeePhysicsEvent::getQuaternion(CoffeePhysicsEvent::PropertyEnum property)
{
    property_takeout d = getProperty(property);
    if(d.size==sizeof(glm::quat)){
        glm::quat r;
        memcpy(&r,d.data,d.size);
        return r;
    }else
        return glm::quat();
}

QObjectList CoffeePhysicsEvent::targets() const
{
    return m_targets;
}

QList<PhysicsObject *> CoffeePhysicsEvent::physicsTargets() const
{
    QList<PhysicsObject*> obj;
    for(QObject* o : m_targets)
        obj.append(qobject_cast<PhysicsObject*>(o));
    return obj;
}

CoffeePropertyContainer::property_takeout CoffeePhysicsEvent::getProperty(CoffeePhysicsEvent::PropertyEnum property)
{
    return CoffeePropertyContainer::getProperty(m_propertyMapping.value(property));
}

void CoffeePhysicsEvent::setInt(CoffeePhysicsEvent::PropertyEnum property, quint32 value)
{
    setProperty(property,sizeof(value),&value);
}

void CoffeePhysicsEvent::setScalar(CoffeePhysicsEvent::PropertyEnum property, ScalarValue *value)
{
    ScalarDataType val = value->getValue();
    setProperty(property,sizeof(val),&val);
}

void CoffeePhysicsEvent::setVector3(CoffeePhysicsEvent::PropertyEnum property, Vector3Value *vector)
{
    glm::vec3 val = vector->getValue();
    setProperty(property,sizeof(val),&val);
}

void CoffeePhysicsEvent::setQuaternion(CoffeePhysicsEvent::PropertyEnum property, QuatValue *quaternion)
{
    glm::quat val = quaternion->getValue();
    setProperty(property,sizeof(val),&val);
}

void CoffeePhysicsEvent::setIntent(PropertyIntent intent)
{
    m_intent = intent;
}

void CoffeePhysicsEvent::setDispose(bool dispose)
{
    m_dispose = dispose;
}

void CoffeePhysicsEvent::setTargets(QObjectList targets)
{
    //Only add objects that inherit/are instances of PhysicsObject
    m_targets.clear();
    for(QObject* o : targets){
        if(qobject_cast<PhysicsObject*>(o))
            m_targets.append(o);
    }
}

void CoffeePhysicsEvent::setProperty(CoffeePhysicsEvent::PropertyEnum property, uint64_t size, void *data)
{
    if(property==NullProperty)
        return;

    uint32_t id;
    if(!m_propertyMapping.contains(property)){
        id = CoffeePropertyContainer::addProperty(size,data);
        m_propertyMapping.insert(property,id);
    }else{
        id = m_propertyMapping.value(property);
        if(!CoffeePropertyContainer::setProperty(id,size,data)){
            qDebug("Failed to set physics property!");
        }
    }

    m_propertyCombination |= property;
}
