#include <coffee/core/CObject>

namespace Coffee {

CObject::CObject():
    m_parent(nullptr)
{
}

CObject::CObject(CObject *parent):
    CObject()
{
    m_parent = parent;
    if(parent){
        parent->m_children.push_back(this);
    }
}

CObject::~CObject()
{
    if(m_parent)
        m_parent->removeChild(this);
    for(CObject* child : m_children)
        delete child;
}

CObject *CObject::parent() const
{
    return m_parent;
}

CObject *CObject::parent()
{
    return m_parent;
}

void CObject::setParent(CObject *parent)
{
    if(parent!=this){
        for(auto it=m_parent->m_children.begin();
            it!=m_parent->m_children.end();
            it++)
            if(*it==this){
                m_parent->m_children.erase(it);
                break;
            }
        this->m_parent = parent;
        m_parent->m_children.push_back(this);
    }
}

cstring CObject::objectName() const
{
    return m_objectName.c_str();
}

void CObject::setObjectName(cstring name)
{
    this->m_objectName = name;
}

void CObject::removeChild(CObject *child)
{
    for(auto it=m_children.begin();it!=m_children.end();it++)
        if(*it == child)
        {
            m_children.erase(it);
            break;
        }
}

} // namespace Coffee

