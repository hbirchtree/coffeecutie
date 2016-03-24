#include <coffee/core/CUnitTesting>
#include <coffee/core/CProfiling>
#include <coffee/core/CDebug>

using namespace Coffee;

const uint32 testing_amount = 100;

bool context_pushing()
{
    CElapsedTimerMicro t;

    /* Test performance of context pushing */

    t.start();

    /* Pushing context */
    for(uint32 i=0;i<testing_amount;i++)
        Profiler::PushContext("Test context");

    cDebug("Pushing context: {0}",t.elapsed());

    t.start();

    /* Popping context */
    for(uint32 i=0;i<testing_amount;i++)
        Profiler::PopContext();

    cDebug("Popping context: {0}",t.elapsed());

    return true;
}

bool profile_pushing()
{
    CElapsedTimerMicro t;
    t.start();

    /* Test performance of profiling function */
    for(uint32 i=0;i<testing_amount;i++)
        Profiler::Profile("Value");

    cDebug("Profiling: {0}",t.elapsed());

    return true;
}

static const constexpr CoffeeTest::Test _tests[2] = {
    context_pushing,
    profile_pushing
};

COFFEE_RUN_TESTS(_tests);