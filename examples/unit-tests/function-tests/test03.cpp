#include <coffee/core/CUnitTesting>
#include <coffee/core/types/cdef/funtypes.h>

using namespace Coffee;

bool signal_test()
{
    /* Testing function slot/signal classes */
    CRect t(0,0,10,10);
    CFunctionSlot<CRect,int32> b(&t,&CRect::area);

    CFunctionSignal<CRect,int32> sigtest;

    return sigtest.call(b)==t.w*t.h;
}

const constexpr CoffeeTest::Test _tests[1] = {
    {signal_test}
};

COFFEE_RUN_TESTS(_tests);