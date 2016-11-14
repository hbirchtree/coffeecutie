#include <coffee/core/plat/environment/linux/environment.h>

namespace Coffee{
namespace Environment{
namespace Linux{

CString LinuxEnv::GetUserData(cstring orgname, cstring appname)
{
    const constexpr cstring var_snappy = "SNAP_USER_COMMON";

    if(LinuxEnv::ExistsVar(var_snappy))
    {
        return LinuxEnv::GetVar(var_snappy);
    }else{
        CString homedir = GetUserHome();
        homedir = ConcatPath(homedir.c_str(),".local/share");
        homedir = ConcatPath(homedir.c_str(),orgname);
        return ConcatPath(homedir.c_str(),appname);
    }
}

}

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
