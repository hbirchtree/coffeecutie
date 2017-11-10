#include <coffee/core/CUnitTesting>
#include <coffee/core/CMD>
#include "test01_extra.h"

using namespace Coffee;

bool function_test()
{
    CElapsedTimerMicro t;

    uint64 time_1_1;
    uint64 time_1_2;
    uint64 time_2;
    uint64 time_3_1;
    uint64 time_3_2;
    uint64 time_4_1;
    uint64 time_4_2;
    uint64 time_4_3;
    uint64 time_5;

    {
        uint32 iterations = UInt32_Max;
        uint32 i;

        TestClass t1;
        TestClassVirt t2;
        TestClassExt t3;

        int32 v1 = 1,v2 = 2;

        Profiler::Profile("Setup");

        /* Virtual function, base class */
        t.start();
        for(i=0;i<iterations;i++)
            t1.fun();
        time_1_1 = t.elapsed();
        Profiler::Profile("Virtual, base class");

        /* Non-virtual function, base class */
        t.start();
        for(i=0;i<iterations;i++)
            t1.directfun();
        time_1_2 = t.elapsed();
        Profiler::Profile("Non-virtual, base class");

        /* Virtual overridden function */
        t.start();
        for(i=0;i<iterations;i++)
            t2.fun();
        time_2 = t.elapsed();
        Profiler::Profile("Virtual overridden, base class");

        /* Extern virtual overridden function */
        t.start();
        for(i=0;i<iterations;i++)
            t3.fun();
        time_3_1 = t.elapsed();
        Profiler::Profile("External virtual overridden, base class");

        /* Extern base function */
        t.start();
        for(i=0;i<iterations;i++)
            t3.directfun();
        time_3_2 = t.elapsed();
        Profiler::Profile("External, base class");

        /* Direct function, inlined */
        t.start();
        for(i=0;i<iterations;i++)
            StaticCall(v1,v2);
        time_4_1 = t.elapsed();
        Profiler::Profile("Inlined direct, no class");

        /* Static function, inlined */
        t.start();
        for(i=0;i<iterations;i++)
            TestStaticClass::StaticClassCall(v1,v2);
        time_4_2 = t.elapsed();
        Profiler::Profile("Static inlined, base class");

        /* Static function, inlined, overridden */
        t.start();
        for(i=0;i<iterations;i++)
            TestStaticOverrideClass::StaticClassCall(v1,v2);
        time_4_3 = t.elapsed();
        Profiler::Profile("Static overridden inline, base class");

        /* External function */
        t.start();
        for(i=0;i<iterations;i++)
            ExternCall(v1,v2);
        time_5 = t.elapsed();
        Profiler::Profile("External, no class");
    }

    Vector<uint64> timing;
    Vector<cstring> titles;

    timing.push_back(time_1_2);
    titles.push_back("Direct");

    timing.push_back(time_1_1);
    titles.push_back("Virtual");

    timing.push_back(time_2);
    titles.push_back("Overridden virtual");

    timing.push_back(time_3_1);
    titles.push_back("External virtual");

    timing.push_back(time_3_2);
    titles.push_back("External direct");

    timing.push_back(time_4_1);
    titles.push_back("Force inlined");

    timing.push_back(time_4_2);
    titles.push_back("Static inlined");

    timing.push_back(time_4_3);
    titles.push_back("Static override inlined");

    timing.push_back(time_5);
    titles.push_back("External");

    Table::Header head;
    head.push_back("Type");
    head.push_back("Time");

    Table::Table table;
    table.push_back(Table::GenColumn(&titles[0],titles.size()));
    table.push_back(Table::GenColumn(&timing[0],timing.size()));

    cBasicPrint("{0}",Table::GenTable(table,head));

    return true;
}

const constexpr CoffeeTest::Test _tests[1] = {
    function_test
};

COFFEE_RUN_TESTS(_tests);
