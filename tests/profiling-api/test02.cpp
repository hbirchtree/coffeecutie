#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool example_test()
{
    return true;
}

const constexpr CoffeeTest::Test _run_tests[1] = {
    {example_test /* Test function */,
     "Short title","Longer description",
     false /* Optional? */,
     false /* Cascading failure? */},
    /* Empty tests are ignored */
};

COFFEE_RUN_TESTS(_run_tests);
