#pragma once

#include "commands.h"
#include "groups.h"

namespace gl {
namespace core {
constexpr bool enabled =
#ifdef GL_VERSION_1_0
    true
#else
    false
#endif
    ;

using v10 = ::gl::impl::core<Version<1, 0>>;
using v11 = ::gl::impl::core<Version<1, 1>>;
using v12 = ::gl::impl::core<Version<1, 2>>;
using v13 = ::gl::impl::core<Version<1, 3>>;
using v14 = ::gl::impl::core<Version<1, 4>>;
using v15 = ::gl::impl::core<Version<1, 5>>;
using v20 = ::gl::impl::core<Version<2, 0>>;
using v21 = ::gl::impl::core<Version<2, 1>>;
using v30 = ::gl::impl::core<Version<3, 0>>;
using v31 = ::gl::impl::core<Version<3, 1>>;
using v32 = ::gl::impl::core<Version<3, 2>>;
using v33 = ::gl::impl::core<Version<3, 3>>;
using v40 = ::gl::impl::core<Version<4, 0>>;
using v41 = ::gl::impl::core<Version<4, 1>>;
using v42 = ::gl::impl::core<Version<4, 2>>;
using v43 = ::gl::impl::core<Version<4, 3>>;
using v44 = ::gl::impl::core<Version<4, 4>>;
using v45 = ::gl::impl::core<Version<4, 5>>;
using v46 = ::gl::impl::core<Version<4, 6>>;
#ifdef GL_VERSION_4_6
using highest = Version<4, 6>;
#elif defined(GL_VERSION_4_5)
using highest = Version<4, 5>;
#elif defined(GL_VERSION_4_4)
using highest = Version<4, 4>;
#elif defined(GL_VERSION_4_3)
using highest = Version<4, 3>;
#elif defined(GL_VERSION_4_2)
using highest = Version<4, 2>;
#elif defined(GL_VERSION_4_1)
using highest = Version<4, 1>;
#elif defined(GL_VERSION_4_0)
using highest = Version<4, 0>;
#elif defined(GL_VERSION_3_3)
using highest = Version<3, 3>;
#elif defined(GL_VERSION_3_2)
using highest = Version<3, 2>;
#elif defined(GL_VERSION_3_1)
using highest = Version<3, 1>;
#elif defined(GL_VERSION_3_0)
using highest = Version<3, 0>;
#elif defined(GL_VERSION_2_1)
using highest = Version<2, 1>;
#elif defined(GL_VERSION_2_0)
using highest = Version<2, 0>;
#elif defined(GL_VERSION_1_5)
using highest = Version<1, 5>;
#elif defined(GL_VERSION_1_4)
using highest = Version<1, 4>;
#elif defined(GL_VERSION_1_3)
using highest = Version<1, 3>;
#elif defined(GL_VERSION_1_2)
using highest = Version<1, 2>;
#elif defined(GL_VERSION_1_1)
using highest = Version<1, 1>;
#elif defined(GL_VERSION_1_0)
using highest = Version<1, 0>;
#endif
using lowest = Version<1, 0>;
} // namespace core

namespace es {
constexpr bool enabled =
#ifdef GL_ES_VERSION_2_0
    true
#else
    false
#endif
    ;

using v20 = ::gl::impl::es<Version<2, 0>>;
using v30 = ::gl::impl::es<Version<3, 0>>;
using v31 = ::gl::impl::es<Version<3, 1>>;
using v32 = ::gl::impl::es<Version<3, 2>>;
#ifdef GL_ES_VERSION_3_2
using highest = Version<3, 2>;
#elif defined(GL_ES_VERSION_3_1)
using highest = Version<3, 1>;
#elif defined(GL_ES_VERSION_3_0)
using highest = Version<3, 0>;
#elif defined(GL_ES_VERSION_2_0)
using highest = Version<2, 0>;
#endif
using lowest = Version<2, 0>;
} // namespace es
} // namespace gl
