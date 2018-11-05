#pragma once

#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>

namespace Coffee {

class NamedObject
{
public:
    cstring objectName() const;
    void setObjectName(cstring name);
private:
    CString m_objectName;
};

class MultiParentObject : public NamedObject
{
public:
    void addChild(MultiParentObject* child);
    void removeChild(MultiParentObject* child);

    Vector<MultiParentObject*> const& children() const;

protected:
    Vector<MultiParentObject*> m_children;
};

class CObject : public MultiParentObject
{
public:
    CObject();
    CObject(CObject* parent);
    ~CObject();

    void addChild(CObject* child) = delete;

    CObject* parent() const;
    CObject* parent();
    void setParent(CObject* parent);

protected:
    void removeChild(CObject* child);
    ThreadId m_thread;

private:
    CObject* m_parent;
};
}
