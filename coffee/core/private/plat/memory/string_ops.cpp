#include <coffee/core/plat/memory/string_ops.h>

namespace Coffee{
namespace Mem{

cwstring_w Convert::NarrowWide(cstring str)
{
    size_t sz = strlen(str)+1;
    cwstring_w out = (cwstring_w)Calloc(sizeof(int16),sz);
    mbstowcs(out,str,sz);
    return out;
}

cstring_w Convert::WideNarrow(cwstring str)
{
    size_t sz = wcslen(str)+1;
    cstring_w out = (cstring_w)Calloc(sizeof(int8),sz);
    wcstombs(out,str,sz);
    out[sz-1] = 0;
    return out;
}

bool Convert::strtobool(cstring n, bool* valid)
{
    static const constexpr cstring str[] = {
        "true", "false",
    };

    if(valid)
        *valid = true;

    if(StrICmp(n, str[0]))
        return true;
    else if(StrICmp(n, str[1]))
        return true;

    if(valid)
        *valid = false;

    return false;
}

}
}
