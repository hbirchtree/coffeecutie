#pragma once

#include <future>
#include <functional>
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Threads{

template<typename... Args>
/*!
 * \brief Launch a task to run independently. Used as fire-and-forget, never to return.
 * \param function Function to run
 */
FORCEDINLINE void RunIndependent(std::function<void()> function,
                                          Args... a)
{
    std::thread m_thread(function,a...);
    m_thread.detach();
}

template<typename ReturnType,
         typename... Args>
/*!
 * \brief Launch a task asynchronously, returning a future value
 * \param function
 * \return A future object for return value
 */
FORCEDINLINE std::future<ReturnType> RunAsync(
        std::function<ReturnType(Args...)> function, Args... a)
{
    return std::async(std::launch::async,function,a...);
}

template<typename T>
/*!
 * \brief Check if a future is available. This is provided as a shortcut.
 * \param f A future to be checked
 * \return True if the value is available
 */
FORCEDINLINE bool FutureAvailable(std::future<T> const& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

}
}