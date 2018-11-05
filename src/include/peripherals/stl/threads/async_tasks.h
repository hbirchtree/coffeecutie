#pragma once

#include <coffee/core/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>

namespace stl_types {
namespace Threads {

template<typename... Args>
/*!
 * \brief Launch a task to run independently. Used as fire-and-forget, never to
 * return. \param function Function to run \param a Argument list passed to
 * function
 */
FORCEDINLINE void RunIndependent(Function<void(Args...)> function, Args... a)
{
    Thread m_thread(function, a...);
    m_thread.detach();
}

template<typename ReturnType, typename... Args>
FORCEDINLINE SharedFuture<ReturnType> RunThread(
    Function<ReturnType(Args...)> function, Args... a)
{
    SharedFuture<ReturnType> rv;
    Thread                   m_thread(function, a...);
}

template<
    typename ReturnType,
    typename... Args
#if defined(COFFEE_NO_FUTURES)
    /* We don't want to let void in here, because it doesn't work with our
     * Future-implementation. The next declaration handles this peacefully.
     */
    ,
    class =
        typename std::enable_if<!std::is_void<ReturnType>::value, bool>::type
#endif
    >
/*!
 * \brief Launch a task asynchronously, returning a future value
 * \param function
 * \param a Argument list passed to function
 * \return A future object for return value
 */
FORCEDINLINE Future<ReturnType> RunAsync(
    Function<ReturnType(Args...)> function, Args... a)
{
#if defined(COFFEE_NO_FUTURES)
    ReturnType r = function(a...);
    return {r};
#else
    return std::async(std::launch::async, function, a...);
#endif
}

#if defined(COFFEE_NO_FUTURES)
template<typename... Args>
FORCEDINLINE Future<void> RunAsync(Function<void(Args...)> function, Args... a)
{
    function(a...);
    return {};
}
#endif

template<typename T>
/*!
 * \brief Check if a future is available. This is provided as a shortcut.
 * \param f A future to be checked
 * \return True if the value is available
 */
FORCEDINLINE bool FutureAvailable(C_UNUSED(Future<T> const& f))
{
#if defined(COFFEE_NO_FUTURES)
    return true;
#else
    if(!f.valid())
        return true;

    return f.wait_for(Chrono::seconds(0)) == FutureStatus::ready;
#endif
}

/*!
 * \brief Sleep for mic microseconds, may not work accurately on mobile devices
 * \param mic
 */
FORCEDINLINE void sleepMicros(u64 mic)
{
    CurrentThread::sleep_for(Chrono::microseconds(mic));
}
/*!
 * \brief Sleep for mll milliseconds
 * \param mll
 */
FORCEDINLINE void sleepMillis(u64 mll)
{
    CurrentThread::sleep_for(Chrono::milliseconds(mll));
}

} // namespace Threads
} // namespace stl_types
