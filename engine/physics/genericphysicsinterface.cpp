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
