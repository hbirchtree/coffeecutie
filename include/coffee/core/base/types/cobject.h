#ifndef COFFEE_COBJECT_H
#define COFFEE_COBJECT_H

#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee {

class CObject
{
public:
    CObject();
    CObject(CObject* parent);
    ~CObject();
//    C_DELETE_COPY_CONSTRUCTOR(CObject);

    CObject* parent() const;
    CObject* parent();
    void setParent(CObject* parent);

    //Object name, must be freed
    cstring objectName() const;
    void setObjectName(cstring name);

    Vector<CObject*> const& children() const;

protected:
    ThreadId m_thread;

private:
    void removeChild(CObject* child);

    Vector<CObject*> m_children;
    CObject* m_parent;
    CString m_objectName;
};
} // namespace Coffee

#endif // COFFEE_COBJECT_H
