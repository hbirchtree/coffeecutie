#pragma once

#include <coffee/core/function_export.h>

typedef const char* cstring;

#ifdef __cplusplus
extern "C" {
#endif

COFFAPI void cDebug(cstring fmt);

#ifdef __cplusplus
}
#endif
