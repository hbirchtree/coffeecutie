#include <coffee/core/CObject>

#include <algorithm>

#include <coffee/core/CDebug>

namespace Coffee {

CObject::CObject() : m_parent(nullptr)
{
}

CObject::CObject(CObject* parent) : CObject()
{
    m_parent = parent;
    if(parent)
    {
        parent->m_children.push_back(this);
    }
}

CObject::~CObject()
{
    if(m_parent)
        m_parent->removeChild(this);
    for(auto child : m_children)
        delete child;
}

CObject* CObject::parent() const
{
    return m_parent;
}

CObject* CObject::parent()
{
    return m_parent;
}

void CObject::setParent(CObject* parent)
{
    if(parent == this)
        return;
    if(m_parent)
        m_parent->removeChild(this);
    if(parent)
        parent->m_children.push_back(this);
    this->m_parent = parent;
}

cstring NamedObject::objectName() const
{
    return m_objectName.c_str();
}

void NamedObject::setObjectName(cstring name)
{
    this->m_objectName = name;
}

const Vector<MultiParentObject*>& MultiParentObject::children() const
{
    return m_children;
}

void CObject::removeChild(CObject* child)
{
    MultiParentObject::removeChild(child);
}

void MultiParentObject::addChild(MultiParentObject* child)
{
    m_children.push_back(child);
}

void MultiParentObject::removeChild(MultiParentObject* child)
{
    std::remove_if(
        m_children.begin(), m_children.end(), [&](MultiParentObject* it) {
            if(it == child)
                return true;
            return false;
        });
}

} // namespace Coffee
