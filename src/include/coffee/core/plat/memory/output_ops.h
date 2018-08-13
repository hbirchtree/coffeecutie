#pragma once

#include <stdio.h>

#include "../../types/tdef/integertypes.h"
#include "../../coffee_mem_macros.h"

namespace Coffee{
namespace Mem{
/* Input/output */
FORCEDINLINE cstring Gets(cstring_w target, i32 size, FILE* strm)
{
    return fgets(target,size,strm);
}

FORCEDINLINE void Puts(FILE* strm, cstring output)
{
    fputs(output,strm);
    fflush(strm);
}

}
}
