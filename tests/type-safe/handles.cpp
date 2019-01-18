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
    } catch(...)
    {
    }

    handle_out_of_scope<unsigned int>(0);

#if MODE_DEBUG
    try
    {
        handle_out_of_scope<unsigned int>(1);

        return false;
    } catch(...)
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

        return false;
    } catch(...)
    {
    }
#endif

    handle_released<void*>(nullptr);
    handle_released<void*>(&sample);

    return true;
}

COFFEE_TEST_SUITE(2) = {
    {integer_handle, "Integer handles"},
    {pointer_handle, "Pointer handles"},
};

COFFEE_EXEC_TESTS()
