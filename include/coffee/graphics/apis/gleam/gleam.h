#include <coffee/core/plat/platform_detect.h>

#ifdef COFFEE_GLEAM_DESKTOP
#include "levels/desktop/gl45.h"
#else

#if defined(COFFEE_ONLY_GLES20)
#include "levels/es/gles20.h"
#else
#include "levels/es/gles32.h"
#endif

#endif

#include "levels/gl_shared_util.h"
