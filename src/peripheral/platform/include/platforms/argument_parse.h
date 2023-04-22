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

    std::string programName() const;

    std::vector<cstring_w> const& arguments() const;
    std::vector<std::string> const&   originalArguments() const;

    std::vector<cstring_w> m_ptrStorage;

  private:
    cstring_w       m_programName;
    std::vector<std::string> m_storage;
};

struct ArgumentResult
{
    std::map<std::string, u32>     switches;
    std::map<std::string, std::string> arguments;
    std::map<std::string, std::string> positional;

    std::string of(std::string const& name) const;
    std::string pos(std::string const& name) const;
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

    std::string helpMessage() const;

    std::vector<aargswitch> switches;
    std::vector<aargswitch> arguments;
    std::vector<apos>       posargs;
};

} // namespace args
} // namespace platform
