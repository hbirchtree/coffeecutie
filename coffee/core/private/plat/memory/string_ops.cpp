#include <coffee/core/plat/memory/string_ops.h>

namespace Coffee{
namespace Mem{

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
