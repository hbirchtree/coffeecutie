#include <coffee/core/CUnitTesting>

#include <coffee/core/task_queue/quick_tasks.h>

using namespace Coffee;

bool linear_lerp()
{
    auto lower_test = Lerp<>::Linear(1.f, 2.f, 0.25f);
    auto upper_test = Lerp<>::Linear(1.f, 2.f, 0.75f);

    if(!CMath::floating_equal(lower_test, 1.25f))
        return false;

    if(!CMath::floating_equal(upper_test, 1.75f))
        return false;

    return true;
}

COFFEE_TEST_SUITE(1) = {
    {linear_lerp, "Linear lerp"},
};

COFFEE_EXEC_TESTS()
