#ifndef COFFEE_PLAT_MEMORY_H
#define COFFEE_PLAT_MEMORY_H

#include "../platform_detect.h"

#include "stlstring_ops.h"
#include "memory_ops.h"
#include "string_ops.h"
#include "output_ops.h"
#include "endian_ops.h"

#include <cctype>

namespace Coffee{

static FILE* DefaultDebugOutputPipe = stderr;
static FILE* DefaultPrintOutputPipe = stdout;
static FILE* DefaultInputPipe = stdin;

namespace Mem{

using Search::StrFind;
using Search::CStrReplace;

}

using namespace Mem;

}

#endif