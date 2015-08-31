#include "cobject.h"

namespace Coffee {

CObject::CObject()
{

}

CObject::CObject(CObject *parent)
{
    if(parent){
        parent->m_children.push_back(this);
        this->m_parent = parent;
    }
}

CObject::~CObject()
{
    for(CObject* child : m_children)
        delete child;
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
    }else{

    }
}

CString CObject::objectName()
{
    return m_objectName;
}

void CObject::setObjectName(const CString &name)
{
    this->m_objectName = name;
}

} // namespace Coffee

