#include "test01_extra.h"

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