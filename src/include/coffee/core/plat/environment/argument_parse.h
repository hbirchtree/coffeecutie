#ifndef ARGUMENT_PARSE
#define ARGUMENT_PARSE

#define COFFEE_MAX_FILEPATH_BUFFER_SIZE 255
#define COFFEE_ARG_SWITCH '-'

#include "../platform_detect.h"
#include "../memory/cmemory.h"

namespace Coffee{

struct _cbasic_arg_container;

using AppArg = _cbasic_arg_container;

struct ArgumentResult
{
    Map<CString, u32> switches;
    Map<CString, CString> arguments;
    Map<CString, CString> positional;
};

struct ArgumentParser
{
    struct aargswitch
    {
        cstring name;
        cstring longname;
        cstring shortname;
        cstring help;
    };
    struct apos
    {
        cstring name;
        cstring help;
    };

    void addSwitch(cstring name, cstring longname, cstring shortname,
                   cstring help);

    void addArgument(cstring name, cstring longname, cstring shortname,
                     cstring help);

    void addPositionalArgument(cstring name,
                               cstring help);

    ArgumentResult parseArguments(AppArg& args);

    CString helpMessage() const;

    Vector<aargswitch> switches;
    Vector<aargswitch> arguments;
    Vector<apos> posargs;
};

}

#endif
