#include <coffee/core/CUnitTesting>
#include <peripherals/semantic/handle.h>

using namespace Coffee;

using semantic::handle_modes::auto_close;

template<typename T>
DENYINLINE void handle_out_of_scope(T some_value)
{
    semantic::generic_handle_t<T, auto_close> handle(some_value);
    handle.handle_check();
    handle.release();
}

template<typename T>
DENYINLINE void handle_released(T some_value)
{
    semantic::generic_handle_t<T, auto_close> handle(some_value);

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

static int transfer_value_handle = -1;
static int transfer_num_calls    = 0;

void transfer_close_function(int h)
{
    transfer_value_handle = h;
    transfer_num_calls++;
}

bool handle_transfer()
{
    using semantic::generic_handle_t;

    /* std::move() handle */
    try
    {
        generic_handle_t<int, auto_close, -1, transfer_close_function> hnd1;
        generic_handle_t<int, auto_close, -1, transfer_close_function> hnd2;

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

    assert::Equals(transfer_value_handle, 1);
    assert::Equals(transfer_num_calls, 1);

    return true;
}

COFFEE_TESTS_BEGIN(3)

    {integer_handle, "Integer handles"},
    {pointer_handle, "Pointer handles"},
    {handle_transfer, "Transfer of handle"}

COFFEE_TESTS_END()
