#pragma once

#include "groups.h"
#include "commands.h"

namespace gl {
namespace core {
#ifdef GL_VERSION_1_0
using v10 = ::gl::impl::core<Version<1, 0>>;
#endif
#ifdef GL_VERSION_1_1
using v11 = ::gl::impl::core<Version<1, 1>>;
#endif
#ifdef GL_VERSION_1_2
using v12 = ::gl::impl::core<Version<1, 2>>;
#endif
#ifdef GL_VERSION_1_3
using v13 = ::gl::impl::core<Version<1, 3>>;
#endif
#ifdef GL_VERSION_1_4
using v14 = ::gl::impl::core<Version<1, 4>>;
#endif
#ifdef GL_VERSION_1_5
using v15 = ::gl::impl::core<Version<1, 5>>;
#endif
#ifdef GL_VERSION_2_0
using v20 = ::gl::impl::core<Version<2, 0>>;
#endif
#ifdef GL_VERSION_2_1
using v21 = ::gl::impl::core<Version<2, 1>>;
#endif
#ifdef GL_VERSION_3_0
using v30 = ::gl::impl::core<Version<3, 0>>;
#endif
#ifdef GL_VERSION_3_1
using v31 = ::gl::impl::core<Version<3, 1>>;
#endif
#ifdef GL_VERSION_3_2
using v32 = ::gl::impl::core<Version<3, 2>>;
#endif
#ifdef GL_VERSION_3_3
using v33 = ::gl::impl::core<Version<3, 3>>;
#endif
#ifdef GL_VERSION_4_0
using v40 = ::gl::impl::core<Version<4, 0>>;
#endif
#ifdef GL_VERSION_4_1
using v41 = ::gl::impl::core<Version<4, 1>>;
#endif
#ifdef GL_VERSION_4_2
using v42 = ::gl::impl::core<Version<4, 2>>;
#endif
#ifdef GL_VERSION_4_3
using v43 = ::gl::impl::core<Version<4, 3>>;
#endif
#ifdef GL_VERSION_4_4
using v44 = ::gl::impl::core<Version<4, 4>>;
#endif
#ifdef GL_VERSION_4_5
using v45 = ::gl::impl::core<Version<4, 5>>;
#endif
#ifdef GL_VERSION_4_6
using v46 = ::gl::impl::core<Version<4, 6>>;
#endif
} // core
namespace es {
#ifdef GL_ES_VERSION_2_0
using v20 = ::gl::impl::es<Version<2, 0>>;
#endif
#ifdef GL_ES_VERSION_3_0
using v30 = ::gl::impl::es<Version<3, 0>>;
#endif
#ifdef GL_ES_VERSION_3_1
using v31 = ::gl::impl::es<Version<3, 1>>;
#endif
#ifdef GL_ES_VERSION_3_2
using v32 = ::gl::impl::es<Version<3, 2>>;
#endif
} // es
} // gl
