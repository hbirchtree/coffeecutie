#pragma once

#include <coffee/components/subsystem.h>
#include <coffee/components/types.h>

namespace comp_app {
namespace detail {
using namespace Coffee::Components;
}

ENTCOMP_CREATE_TAG(WindowTag);
ENTCOMP_CREATE_TAG(EventBusTag);
ENTCOMP_CREATE_TAG(InputStateTag);

struct Windowing : detail::RestrictedSubsystem<WindowTag>
{
    virtual void test() = 0;
};

} // namespace comp_app
