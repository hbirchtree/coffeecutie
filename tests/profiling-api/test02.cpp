#include <coffee/core/CUnitTesting>

using namespace Coffee;

bool example_test()
{
    return true;
}

COFFEE_TESTS_BEGIN(1)

    {example_test /* Test function */,
     "Short title",
     "Longer description",
     false /* Optional? */,
     false /* Cascading failure? */}

COFFEE_TESTS_END()
