#include <coffee/core/unit_tests/micro_tests.h>

namespace CoffeeTests
{

void TestClassExt::fun()
{
    value1 += value2;
}

void TestClassExt::directfun()
{
    value1 += value2;
}

void ExternCall(int32& v1, int32& v2)
{
    v1 += v2;
}

}