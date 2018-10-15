#pragma once

#include "../platform_detect.h"

#include "endian_ops.h"
#include "memory_ops.h"
#include "output_ops.h"
#include "stlstring_ops.h"
#include "string_ops.h"

#include <cctype>

namespace Coffee {

static FILE* DefaultDebugOutputPipe = stderr;
static FILE* DefaultPrintOutputPipe = stdout;
static FILE* DefaultInputPipe       = stdin;

} // namespace Coffee
