#ifndef COFFEE_CORE_UNIT_TESTS_MICRO_TESTS_H
#define COFFEE_CORE_UNIT_TESTS_MICRO_TESTS_H

#include "../types/composite_types.h"
#include <coffee/core/CDebug>

namespace CoffeeTests
{

using namespace Coffee;

static const cstring test_string = "Glibber glabber text: {0}, {1}, {2}";

class TestClass
{
public:
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
    virtual void fun()
    {
       value1 += value2;
    }
};

class TestClassExt : public TestClass
{
public:
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

STATICINLINE void FunctionCallTest()
{
    CElapsedTimerMicro t;

    uint64 time_1_1;
    uint64 time_1_2;
    uint64 time_2;
    uint64 time_3_1;
    uint64 time_3_2;
    uint64 time_4_1;
    uint64 time_4_2;
    uint64 time_5;

    {
        uint32 iterations = 8192*1000*10;
        uint32 i;

        TestClass t1;
        TestClassVirt t2;
        TestClassExt t3;

        int32 v1 = 1,v2 = 2;

        /* Virtual function, base class */
        t.start();
        for(i=0;i<iterations;i++)
            t1.fun();
        time_1_1 = t.elapsed();

        /* Non-virtual function, base class */
        t.start();
        for(i=0;i<iterations;i++)
            t1.directfun();
        time_1_2 = t.elapsed();

        /* Virtual overridden function */
        t.start();
        for(i=0;i<iterations;i++)
            t2.fun();
        time_2 = t.elapsed();

        /* Extern virtual overridden function */
        t.start();
        for(i=0;i<iterations;i++)
            t3.fun();
        time_3_1 = t.elapsed();

        /* Extern base function */
        t.start();
        for(i=0;i<iterations;i++)
            t3.fun();
        time_3_2 = t.elapsed();

        /* Direct function, inlined */
        t.start();
        for(i=0;i<iterations;i++)
            StaticCall(v1,v2);
        time_4_1 = t.elapsed();

        /* Static function, inlined */
        t.start();
        for(i=0;i<iterations;i++)
            TestStaticClass::StaticClassCall(v1,v2);
        time_4_2 = t.elapsed();

        /* External function */
        t.start();
        for(i=0;i<iterations;i++)
            ExternCall(v1,v2);
        time_5 = t.elapsed();
    }

    cDebug("Function overhead times:");
    cBasicPrint("Direct: {0}",time_1_2);
    cBasicPrint("Virtual: {0}",time_1_1);
    cBasicPrint("Overridden virtual: {0}",time_2);
    cBasicPrint("External virtual: {0}",time_3_1);
    cBasicPrint("External direct: {0}",time_3_2);
    cBasicPrint("Force inlined: {0}",time_4_1);
    cBasicPrint("Static inlined: {0}",time_4_2);
    cBasicPrint("External: {0}",time_5);
}

STATICINLINE void StaticDebugPrintCall()
{
    cDebug(test_string,1L,2L,3.0);
}

STATICINLINE void StaticfprintfPrintCall()
{
    fprintf(stderr,"Glibber glabber text: %i %i %f\n",1,2,(bigscalar)3.0);
}

STATICINLINE void StaticBasicPrintCall()
{
    cBasicPrint(test_string,1L,2L,3.0);
}

STATICINLINE void PrintCallTest()
{
    CElapsedTimer t;
    uint32 iterations = 8192;
    uint32 i;

    uint64 fprint_time;
    uint64 debug_time;
    uint64 basic_time;

    {
        t.start();
        for(i=0;i<iterations;i++)
            StaticfprintfPrintCall();
        fprint_time = t.elapsed();

        t.start();
        for(i=0;i<iterations;i++)
            StaticBasicPrintCall();
        basic_time = t.elapsed();

        t.start();
        for(i=0;i<iterations;i++)
            StaticDebugPrintCall();
        debug_time = t.elapsed();
    }

    cDebug("Printing overhead times:");
    cBasicPrint("fprintf: {0}",fprint_time);
    cBasicPrint("Basic: {0}",basic_time);
    cBasicPrint("Debug: {0}",debug_time);
}

}


#endif
