#pragma once

#include "coffee_version.h"
#include <peripherals/stl/functional_types.h>
#include <coffee/core/base_state.h>

namespace Coffee {

namespace Profiling {
struct PContext;
struct ThreadState;
} // namespace Profiling

struct BuildInfo
{
    cstring compiler;
    cstring architecture;
    cstring platform;
    cstring build_version;

    CString default_window_name;

#if defined(COFFEE_ANDROID)
    CString plat_tmp_string;
#endif
};

namespace State {

struct InternalState;
struct InternalThreadState;

template<typename T>
using P = ShPtr<T>;

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

extern ThreadId& GetCurrentThreadId();

#if defined(COFFEE_USE_TERMINAL_CTL)
extern bool& GetAlternateTerminal();
#endif

extern Mutex& GetPrinterLock();

extern BuildInfo& GetBuildInfo();

extern AppData& GetAppData();

} // namespace State

} // namespace Coffee
