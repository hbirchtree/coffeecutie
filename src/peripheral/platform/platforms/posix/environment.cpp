#include <platforms/posix/environment.h>

#if defined(COFFEE_APPLE)
#include <platforms/osx/environment.h>

#if !defined(COFFEE_APPLE_MOBILE)
#include <libproc.h>
#endif

#endif

#include <libgen.h>
#include <limits.h>
#include <unistd.h>

namespace platform {
namespace env {
namespace posix {

using namespace libc;
using namespace url::constructors;

bool EnvironmentF::PrependVar(cstring var, cstring val)
{
    CString new_val = val;
    new_val += GetVar(var);
    return SetVar(var, new_val.c_str());
}

bool EnvironmentF::AppendVar(cstring var, cstring val)
{
    CString new_val = GetVar(var);
    new_val += val;
    return SetVar(var, new_val.c_str());
}

CString EnvironmentF::ConcatPath(cstring v1, cstring v2)
{
    return v1 + GetPathSep() + v2;
}

Url EnvironmentF::CurrentDir()
{
    CString dir;
    dir.resize(PATH_MAX);
    getcwd(&dir[0], dir.size());
    auto idx = dir.find('\0');
    if(idx != CString::npos)
        dir.resize(idx);
    return MkUrl(dir, RSCA::SystemFile);
}

EnvInterface::Variables EnvironmentF::Environment()
{
    Variables e;
    char*     envar = environ[0];
    CString   v1, v2;
    szptr     vn = 0;
    cstring   q;
    while(envar)
    {
        q = libc::str::find(envar, "=");
        if(!q)
            continue;
        v1.insert(0, C_CAST<cstring>(envar), C_CAST<size_t>(q - envar));
        v2.insert(0, C_CAST<cstring>(q + 1), libc::str::len(q + 1));

        e.insert(VarPair(v1, v2));

        v1.clear();
        v2.clear();

        vn++;
        envar = environ[vn];
    }
    return e;
}

} // namespace posix

#if defined(COFFEE_APPLE)
CString mac::EnvironmentF::ExecutableName(cstring_w)
{
#if !defined(COFFEE_APPLE_MOBILE)
    int   ret;
    pid_t pid;
    char  pathbuf[PROC_PIDPATHINFO_MAXSIZE] = {};

    pid = getpid();
    ret = proc_pidpath(pid, pathbuf, sizeof(pathbuf));
    CString out;
    out.insert(0, pathbuf, sizeof(pathbuf));
    szptr e = out.find('\0');
    out.resize(e);
    return out;
#else
    return "App";
#endif
}
#endif

CString& posix::TerminalColorCodes::ColorText(
    CString& text, EnvColorCodes::CmdFormat fmt)
{
#if !defined(COFFEE_ANDROID)
    static const constexpr cstring s[] = {
        "\033[0m", // Reset

        /* Foreground */

        "\033[1m\033[30m", // Black
        "\033[1m\033[37m", // White
        "\033[0m\033[30m", // FontGray
        "\033[0m\033[37m", // FontLightGray

        "\033[0m\033[31m", // Red
        "\033[0m\033[32m", // Green
        "\033[1m\033[34m", // Blue
        "\033[1m\033[33m", // Yellow

        "\033[1m\033[36m", // Cyan
        "\033[1m\033[35m", // FontDeepViolet
        "\033[0m\033[33m", // FontBrown
        "\033[1m\033[32m", // FontDarkGreen
        "\033[1m\033[36m", // FontLightCyan
        "\033[0m\033[35m", // FontPink

        /* Background */

        "\033[1m\033[40m", // BgBlack
        "\033[1m\033[47m", // BgWhite
        "\033[0m\033[40m", // BgGray
        "\033[0m\033[47m", // BgLightGray

        "\033[0m\033[41m", // BgRed
        "\033[0m\033[42m", // BgGreen
        "\033[1m\033[44m", // BgBlue
        "\033[1m\033[43m", // BgYellow

        "\033[1m\033[46m", // BgCyan
        "\033[1m\033[45m", // BgDeepViolet
        "\033[0m\033[43m", // BgBrown
        "\033[1m\033[42m", // BgDarkGreen
        "\033[1m\033[46m", // BgLightCyan
        "\033[1m\033[41m", // BgPink
    };

    u8 fg = (fmt & 0xFF);
    u8 bg = (fmt >> 8) & 0xFF;

    if(fg < CmdColor::MaxColor)
        text = s[fg + 1] + text;
    if(bg < CmdColor::MaxColor)
        text = s[bg + 1 + CmdColor::MaxColor] + text;

    /* Add terminator */
    text += s[0];

    return text;
#else
    return text;
#endif
}

} // namespace env
} // namespace platform
