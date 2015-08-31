#ifndef COFFEE_COBJECT_H
#define COFFEE_COBJECT_H

#include "coffee.h"

namespace Coffee {

class CObject
{
public:
    CObject();
    CObject(CObject* parent);
    ~CObject();

    CObject* parent();
    void setParent(CObject* parent);

    //Object name
    CString objectName();
    void setObjectName(const CString &name);

private:
    std::vector<CObject*> m_children;
    CObject* m_parent;
    CString m_objectName;
};

} // namespace Coffee

#endif // COFFEE_COBJECT_H
