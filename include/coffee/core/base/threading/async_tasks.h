#pragma once

#include "../../coffee_mem_macros.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stlfunctypes.h"

namespace Coffee{
namespace Threads{

template<typename... Args>
/*!
 * \brief Launch a task to run independently. Used as fire-and-forget, never to return.
 * \param function Function to run
 * \param a Argument list passed to function
 */
FORCEDINLINE void RunIndependent(
        Function<void(Args...)> function,
        Args... a)
{
    Thread m_thread(function,a...);
    m_thread.detach();
}

template<typename ReturnType, typename... Args> FORCEDINLINE
SharedFuture<ReturnType> RunThread(
        Function<ReturnType(Args...)> function,
        Args... a)
{
    SharedFuture<ReturnType> rv;
    Thread m_thread(function,a...);
}

template<typename ReturnType,
         typename... Args>
/*!
 * \brief Launch a task asynchronously, returning a future value
 * \param function
 * \param a Argument list passed to function
 * \return A future object for return value
 */
FORCEDINLINE Future<ReturnType> RunAsync(
        Function<ReturnType(Args...)> function, Args... a)
{
    return std::async(std::launch::async,function,a...);
}

template<typename T>
/*!
 * \brief Check if a future is available. This is provided as a shortcut.
 * \param f A future to be checked
 * \return True if the value is available
 */
FORCEDINLINE bool FutureAvailable(Future<T> const& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

/*!
 * \brief Sleep for mic microseconds, may not work accurately on mobile devices
 * \param mic
 */
FORCEDINLINE void sleepMicros(uint64 mic)
{
    std::this_thread::sleep_for(std::chrono::microseconds(mic));
}
/*!
 * \brief Sleep for mll milliseconds
 * \param mll
 */
FORCEDINLINE void sleepMillis(uint64 mll)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(mll));
}

}
}