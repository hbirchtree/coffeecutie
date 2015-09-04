#include "cobject.h"

namespace Coffee {

CObject::CObject()
{
    m_thread = std::this_thread::get_id();
}

CObject::CObject(CObject *parent) : CObject()
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

const char* CObject::objectName()
{
    return coffee_cpy_string(m_objectName.c_str());
}

void CObject::setObjectName(const char* name)
{
    this->m_objectName = name;
}

} // namespace Coffee

