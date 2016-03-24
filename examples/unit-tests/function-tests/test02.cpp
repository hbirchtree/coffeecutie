#include <coffee/core/CUnitTesting>
#include "test01_extra.h"

using namespace Coffee;

static const cstring test_string = "Glibber glabber text: {0}, {1}, {2}";



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

bool printing_tests()
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

    return true;
}

static const constexpr CoffeeTest::Test _tests[1] = {
    printing_tests
};

COFFEE_RUN_TESTS(_tests);