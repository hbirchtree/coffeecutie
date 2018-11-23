#pragma once

#include <peripherals/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace args {

using namespace ::libc_types;
using namespace ::stl_types;

struct ArgumentParser;

struct AppArg
{
    friend struct ArgumentParser;

    void rebuildArgs();

    static AppArg Clone(i32 argc, cstring_w* argv);

    cstring_w programName() const;

    Vector<cstring_w> const& arguments() const;
    Vector<CString> const&   originalArguments() const;

    Vector<cstring_w> m_ptrStorage;

  private:
    cstring_w       m_programName;
    Vector<CString> m_storage;
    bool            string_containment;
};

struct ArgumentResult
{
    Map<CString, u32>     switches;
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

    void addSwitch(
        cstring name, cstring longname, cstring shortname, cstring help);

    void addArgument(
        cstring name, cstring longname, cstring shortname, cstring help);

    void addPositionalArgument(cstring name, cstring help);

    ArgumentResult parseArguments(AppArg& args);

    CString helpMessage() const;

    Vector<aargswitch> switches;
    Vector<aargswitch> arguments;
    Vector<apos>       posargs;
};

} // namespace args
} // namespace platform
