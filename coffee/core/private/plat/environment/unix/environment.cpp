#include <coffee/core/plat/environment/unix/environment.h>

#if defined(COFFEE_APPLE)
#include <coffee/core/plat/environment/osx/environment.h>

#if !defined(COFFEE_APPLE_MOBILE)
#include <libproc.h>
#endif

#endif

#include <unistd.h>
#include <libgen.h>
#include <limits.h>

namespace Coffee{
namespace Environment{
namespace Posix{

CString PosixEnvironmentFun::BaseName(cstring n)
{
#if !defined(COFFEE_USE_POSIX_BASENAME)
    if(StrLen(n)<1)
        return ".";
    // This one is fast, but does not handle rootfs
    int64 idx = Search::ChrFindR(n,'/')-n;
    if(idx < 0)
        return n;
    CString out;
    out.insert(0,&n[idx+1],StrLen(n)-idx-1);
    if(out.empty())
        out = ".";
    return out;
#else
    // This one is slower, but more compliant
    cstring dname = n;
    cstring_w out = AllocT<sbyte_t>(StrLen(dname)+1);
    StrCpy(out,dname);
    CString out_s = basename(out);
    CFree(out);
    return out_s;
#endif
}

CString PosixEnvironmentFun::DirName(cstring fname)
{
#if !defined(COFFEE_USE_POSIX_BASENAME)
    int64 idx = Search::ChrFindR(fname,'/')-fname;
    if(idx < 0)
        return fname;
    CString out;
    out.insert(0,&fname[0],idx);
    if(out.empty())
        out = ".";
    return out;
#else
    // This one is slower, but more compliant
    cstring_w out = AllocT<sbyte_t>(StrLen(fname)+1);
    StrCpy(out,fname);
    CString out_s = dirname(out);
    CFree(out);
    return out_s;
#endif
}

bool PosixEnvironmentFun::PrependVar(cstring var, cstring val)
{
    CString new_val = val;
    new_val += GetVar(var);
    return SetVar(var,new_val.c_str());
}

bool PosixEnvironmentFun::AppendVar(cstring var, cstring val)
{
    CString new_val = GetVar(var);
    new_val += val;
    return SetVar(var,new_val.c_str());
}

CString PosixEnvironmentFun::ConcatPath(cstring v1, cstring v2)
{
    return v1+GetPathSep()+v2;
}

CString PosixEnvironmentFun::CurrentDir()
{
    CString dir;
    dir.resize(PATH_MAX);
    dir.resize(C_CAST<size_t>(strchr(dir.c_str(),'\0')-dir.c_str()));
    return dir;
}

EnvInterface::Variables PosixEnvironmentFun::Environment()
{
    Variables e;
    char* envar = environ[0];
    CString v1,v2;
    szptr vn = 0;
    cstring q;
    while(envar)
    {
        q = StrFind(envar,"=");
        if(!q)
            continue;
        v1.insert(0,C_CAST<cstring>(envar),C_CAST<size_t>(q-envar));
        v2.insert(0,C_CAST<cstring>(q+1),StrLen(q+1));

        e.insert(VarPair(v1,v2));

        v1.clear();
        v2.clear();

        vn++;
        envar = environ[vn];
    }
    return e;
}

}

#if defined(COFFEE_APPLE)
CString Coffee::Environment::Mac::MacEnv::ExecutableName(cstring_w)
{
#if !defined(COFFEE_APPLE_MOBILE)
    int ret;
    pid_t pid;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE] = {};

    pid = getpid();
    ret = proc_pidpath(pid,pathbuf,sizeof(pathbuf));
    CString out;
    out.insert(0,pathbuf,sizeof(pathbuf));
    szptr e = out.find('\0');
    out.resize(e);
    return out;
#else
    return "App";
#endif
}
#endif

CString &Posix::PosixTerminalColorCodes::ColorText(CString &text, EnvColorCodes::CmdFormat fmt)
{
#if !defined(COFFEE_ANDROID)
    static const constexpr cstring s[] = {
        "\033[0m", //Reset

        /* Foreground */

        "\033[1m\033[30m", //Black
        "\033[1m\033[37m", //White
        "\033[0m\033[30m", //FontGray
        "\033[0m\033[37m", //FontLightGray

        "\033[0m\033[31m", //Red
        "\033[0m\033[32m", //Green
        "\033[1m\033[34m", //Blue
        "\033[1m\033[33m", //Yellow

        "\033[1m\033[36m", //Cyan
        "\033[1m\033[35m", //FontDeepViolet
        "\033[0m\033[33m", //FontBrown
        "\033[1m\033[32m", //FontDarkGreen
        "\033[1m\033[36m", //FontLightCyan
        "\033[0m\033[35m", //FontPink

        /* Background */

        "\033[1m\033[40m", //BgBlack
        "\033[1m\033[47m", //BgWhite
        "\033[0m\033[40m", //BgGray
        "\033[0m\033[47m", //BgLightGray

        "\033[0m\033[41m", //BgRed
        "\033[0m\033[42m", //BgGreen
        "\033[1m\033[44m", //BgBlue
        "\033[1m\033[43m", //BgYellow

        "\033[1m\033[46m", //BgCyan
        "\033[1m\033[45m", //BgDeepViolet
        "\033[0m\033[43m", //BgBrown
        "\033[1m\033[42m", //BgDarkGreen
        "\033[1m\033[46m", //BgLightCyan
        "\033[1m\033[41m", //BgPink
    };

    uint8 fg = (fmt & 0xFF);
    uint8 bg = (fmt >> 8) & 0xFF;

    if(fg < CmdColor::MaxColor)
        text = s[fg + 1] + text;
    if(bg < CmdColor::MaxColor)
        text = s[bg + 1 + CmdColor::MaxColor] + text;

    /* Add terminator */
    text += s[0];

    return text;
#endif
}

}
}
