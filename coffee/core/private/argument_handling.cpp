#include <coffee/core/argument_handling.h>

namespace Coffee{

AppArg initargs;

const AppArg &GetInitArgs()
{
    return initargs;
}
}