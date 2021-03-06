#include <coffee/core/CProfiling>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/time_types.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

const u32 testing_amount = 100;

using Clock = Chrono::high_resolution_clock;

bool context_pushing()
{
    /* Test performance of context pushing */
    auto start = Clock::now();

    /* Pushing context */
    for(u32 i = 0; i < testing_amount; i++)
        Profiler::PushContext("Test context");

    cDebug("Pushing context: {0}", Clock::now() - start);

    start = Clock::now();

    /* Popping context */
    for(u32 i = 0; i < testing_amount; i++)
        Profiler::PopContext();

    cDebug("Popping context: {0}", Clock::now() - start);

    return true;
}

bool profile_pushing()
{
    auto start = Clock::now();

    /* Test performance of profiling function */
    for(u32 i = 0; i < testing_amount; i++)
        Profiler::Profile("Value");

    cDebug("Profiling: {0}", Clock::now() - start);

    return true;
}

COFFEE_TESTS_BEGIN(2)

    {context_pushing, "Pushing profiling context"},
    {profile_pushing, "Pushing profiling point"}

COFFEE_TESTS_END()
