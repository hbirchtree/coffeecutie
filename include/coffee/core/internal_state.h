#pragma once

#include <coffee/core/types/tdef/stlfunctypes.h>

namespace Coffee{

struct InternalState;
struct InternalThreadState;

namespace Profiling{
struct ProfilerDataStore;
}

namespace State{

template<typename T>
using P = ShPtr<T>;

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

extern Profiling::ProfilerDataStore *GetProfilerStore();

extern ThreadId& GetCurrentThreadId();

extern bool& GetAlternateTerminal();

extern Mutex& GetPrinterLock();

}

}
