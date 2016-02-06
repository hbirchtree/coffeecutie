#ifndef COFFEE_COBJECT_H
#define COFFEE_COBJECT_H

#include <coffee.h>
#include "cthreading.h"

namespace Coffee {

using namespace CFunctional::Threads;

class CObject
{
public:
    CObject();
    CObject(CObject* parent);
    ~CObject();

    CObject* parent();
    void setParent(CObject* parent);

    //Object name, must be freed
    cstring objectName();
    void setObjectName(cstring name);

protected:
    std::thread::id m_thread;

private:
    void removeChild(CObject* child);

    std::vector<CObject*> m_children;
    CObject* m_parent;
    CString m_objectName;
};
} // namespace Coffee

#endif // COFFEE_COBJECT_H
