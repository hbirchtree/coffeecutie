#include <coffee/core/CUnitTesting>
#include <peripherals/semantic/handle.h>

using namespace Coffee;

template<typename T>
DENYINLINE void handle_out_of_scope(T some_value)
{
    semantic::generic_handle_t<T, false> handle(some_value);
    handle.handle_check();
    handle.release();
}

template<typename T>
DENYINLINE void handle_released(T some_value)
{
    semantic::generic_handle_t<T, false> handle(some_value);

    handle.release();

    return;
}

bool integer_handle()
{
    try
    {
        throw resource_leak("derp");
    } catch(resource_leak const&)
    {
    }

    handle_out_of_scope<unsigned int>(0);

#if MODE_DEBUG
    try
    {
        handle_out_of_scope<unsigned int>(1);

        Throw(test_failure("handle error not triggered on out of scope"));
    } catch(resource_leak const&)
    {
    }
#endif

    handle_released<unsigned int>(0);
    handle_released<unsigned int>(1);

    return true;
}

bool pointer_handle()
{
    int sample = 1;

    handle_out_of_scope<void*>(nullptr);

#if MODE_DEBUG
    try
    {
        handle_out_of_scope<void*>(&sample);

        Throw(test_failure("handle error not triggered on out of scope"));
    } catch(resource_leak const&)
    {
    }
#endif

    handle_released<void*>(nullptr);
    handle_released<void*>(&sample);

    return true;
}

bool handle_transfer()
{
    using semantic::generic_handle_t;

    /* std::move() handle */
    try
    {
        generic_handle_t<int, true, -1> hnd1;
        generic_handle_t<int, true, -1> hnd2;

        /* Verify initial value */
        assert::Equals(C_OCAST<int>(hnd1), -1);
        assert::Equals(C_OCAST<int>(hnd2), -1);

        /* See that value is transferred, source is left invalid */
        hnd1 = 1;
        hnd2 = std::move(hnd1);

        assert::Equals(C_OCAST<int>(hnd1), -1);
        assert::Equals(C_OCAST<int>(hnd2), 1);
    } catch(resource_leak const&)
    {
    }

    return true;
}

COFFEE_TEST_SUITE(3) = {{integer_handle, "Integer handles"},
                        {pointer_handle, "Pointer handles"},
                        {handle_transfer, "Transfer of handle"}};

COFFEE_EXEC_TESTS()
