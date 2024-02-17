#pragma once

#import <UIKit/UIKit.h>

#include <peripherals/stl/functional_types.h>

namespace uikit {

struct GestureRecognizers
{
    using TapRecog = stl_types::Function<void()>;
};

} // namespace uikit
