#include <coffee/core/base.h>

#include "levels/all_levels.h"

namespace Coffee {
namespace CGL {

#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
using CGL33 = CGL_33<GLVER_33>;
using CGL43 = CGL_43<GLVER_43>;
using CGL45 = CGL_45<GLVER_45>;
using CGL46 = CGL_45<GLVER_46>;

using CGL_Lowest = CGL33;
#else
using CGLES20 = CGL_20<GLESVER_20>;
using CGLES30 = CGL_30<GLESVER_30>;
using CGLES32 = CGL_32<GLESVER_32>;

using CGL_Lowest = CGLES20;
#endif

} // namespace CGL
} // namespace Coffee

#include "levels/gl_loader.h"
#include "levels/gl_shared_util.h"
