#include <platforms/pimpl_state.h>

#include <peripherals/stl/types.h>

using namespace stl_types;

namespace platform {
namespace detail {

GlobalState::~GlobalState()
{
}

state_pimpl::state_pimpl() :
    m_LockState(nullptr), SwapState(nullptr), PeekState(nullptr),
    ProfilerEnabled(nullptr), GetProfilerStore(nullptr),
    GetProfilerTStore(nullptr), GetAppData(nullptr)
{
}

} // namespace detail

UqPtr<detail::state_pimpl> state = MkUq<detail::state_pimpl>();
} // namespace platform
