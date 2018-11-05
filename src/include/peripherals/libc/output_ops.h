#pragma once

#include <stdio.h>

#include <coffee/core/base.h>
#include <peripherals/libc/types.h>

namespace Coffee {
namespace Mem {
/* Input/output */
FORCEDINLINE cstring Gets(cstring_w target, i32 size, FILE* strm)
{
    return fgets(target, size, strm);
}

FORCEDINLINE void Puts(FILE* strm, cstring output)
{
    fputs(output, strm);
    fflush(strm);
}

} // namespace Mem
} // namespace Coffee
