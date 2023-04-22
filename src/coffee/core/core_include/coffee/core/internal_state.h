#pragma once

#include <coffee/core/base_state.h>
#include <coffee/core/libc_types.h>
#include <peripherals/build/application.h>
#include <peripherals/stl/thread_types.h>
#include <peripherals/stl/types.h>

namespace Coffee {

namespace Profiling {
struct PContext;
struct ThreadState;
} // namespace Profiling

struct BuildInfo
{
    std::string default_window_name;

#if defined(COFFEE_ANDROID)
    std::string plat_tmp_string;
#endif
};

namespace State {

struct InternalState;
struct InternalThreadState;

template<typename T>
using P = std::shared_ptr<T>;

extern P<InternalState>                    internal_state;
extern thread_local P<InternalThreadState> thread_state;

extern P<InternalState> CreateNewState();

extern P<InternalThreadState> CreateNewThreadState();

/*!
 * \brief Set internal state object for engine.
 *  Contains core functionality.
 * Setting this to nullptr will certainly implode.
 *
 * \param state
 */
extern void SetInternalState(P<InternalState> state);

/*!
 * \brief Clear all stored states
 */
extern void ClearStates();

/*!
 * \brief Get the current state object pointer.
 * This should always be handled as opaque.
 * \return
 */
extern P<InternalState>& GetInternalState();

/*!
 * \brief Same as above, but for thread-local storage
 * \param state
 */
extern void SetInternalThreadState(P<InternalThreadState> state);

/*!
 * \brief Same as above, but for thread-local storage
 * \return
 */
extern P<InternalThreadState>& GetInternalThreadState();

/*
 *
 * Accessors for contained data
 *
 */

extern stl_types::ThreadId& GetCurrentThreadId();

extern stl_types::Mutex& GetPrinterLock();

extern BuildInfo& GetBuildInfo();

} // namespace State

} // namespace Coffee
