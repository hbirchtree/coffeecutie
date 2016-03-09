#include <coffee/core/plat/platform_detect.h>

#ifdef COFFEE_GLEAM_DESKTOP
#include "levels/desktop/gl45.h"
#else
#include "levels/es/gles30.h"
#endif
