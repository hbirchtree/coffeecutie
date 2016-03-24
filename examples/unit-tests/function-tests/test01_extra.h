#include <coffee/core/types/tdef/integertypes.h>

using namespace Coffee;

class TestClass
{
public:
    virtual ~TestClass()
    {
    }
    virtual void fun()
    {
        value1 += value2;
    }
    void directfun()
    {
        value1 += value2;
    }
protected:
    int32 value1;
    int32 value2;
};

class TestClassVirt : public TestClass
{
public:
    virtual ~TestClassVirt()
    {
    }
    virtual void fun()
    {
       value1 += value2;
    }
};

class TestClassExt : public TestClass
{
public:
    virtual ~TestClassExt()
    {
    }
    virtual void fun();
    void directfun();
};

struct TestStaticClass
{
    STATICINLINE void StaticClassCall(int32& v1, int32 v2)
    {
        v1 += v2;
    }
};

STATICINLINE void StaticCall(int32& v1, int32 v2)
{
    v1 += v2;
}

extern void ExternCall(int32& v1, int32& v2);