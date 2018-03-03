#include <coffee/core/argument_handling.h>

namespace Coffee{

AppArg initargs;

AppArg &GetInitArgs()
{
    return initargs;
}

CString Strings::to_string(const AppArg &args)
{
    CString out = "AppArg(";

    bool touched = false;

    for(cstring_w arg : args.arguments())
    {
        if(touched)
            out += ", ";
        ((out += "'") += (arg) ? arg : "NULL") += "'";
        touched = true;
    }

    return out + ")";
}

}
