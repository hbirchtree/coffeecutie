#pragma once

#include "coffee_version.h"
#include <coffee/core/types/tdef/stlfunctypes.h>

namespace Coffee {

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

struct InternalState;
struct InternalThreadState;

namespace Profiling {
struct PContext;
struct ThreadState;
} // namespace Profiling

namespace State {

struct GlobalState
{
    virtual ~GlobalState();
};

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

extern Profiling::PContext*    GetProfilerStore();
extern Profiling::ThreadState* GetProfilerTStore();

extern ThreadId& GetCurrentThreadId();

#if defined(COFFEE_USE_TERMINAL_CTL)
extern bool& GetAlternateTerminal();
#endif

extern Mutex& GetPrinterLock();

extern BuildInfo& GetBuildInfo();

extern CoffeeApplicationData& GetAppData();

extern ShPtr<GlobalState> SwapState(cstring key, ShPtr<GlobalState> const& ptr);
extern ShPtr<GlobalState> const& PeekState(cstring key);

} // namespace State

} // namespace Coffee
