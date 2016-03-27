#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee{
namespace Environment{

struct EnvInterface
{
    using Variables = Map<CString,CString>;
    using VarPair = std::pair<CString,CString>;

    struct TermScreen
    {
	static bool UsingAlternateBuffer;
    };

    STATICINLINE CString ExecutableName(cstring_w = nullptr)
    {
        return CString();
    }

    STATICINLINE CString BaseName(CString const&)
    {
        return CString();
    }

    STATICINLINE CString GetVar(cstring)
    {
        return CString();
    }
    STATICINLINE bool SetVar(cstring, cstring)
    {
        return false;
    }
    STATICINLINE bool UnsetVar(cstring)
    {
        return false;
    }
    STATICINLINE bool ClearEnv()
    {
        return false;
    }

    STATICINLINE CString GetPathSep()
    {
        return CString();
    }
    STATICINLINE CString ConcatPath(cstring, cstring)
    {
        return CString();
    }
    STATICINLINE CString GetUserHome()
    {
        return CString();
    }

    STATICINLINE CString GetUserData(cstring, cstring)
    {
        return CString();
    }

    STATICINLINE CString ApplicationDir()
    {
        return CString();
    }
    STATICINLINE CString CurrentDir()
    {
        return CString();
    }

    STATICINLINE Variables Environment()
    {
        return Variables();
    }
};

struct EnvColorCodes
{
    static constexpr cstring Reset = "";

    /* Font color */

    /* Basics */
    static constexpr cstring FontBlack = "";
    static constexpr cstring FontWhite = "";

    /* Primaries */
    static constexpr cstring FontRed = "";
    static constexpr cstring FontBlue = "";
    static constexpr cstring FontGreen = "";
    static constexpr cstring FontYellow = "";

    /* Expanded */
    static constexpr cstring FontCyan = "";
    static constexpr cstring FontDeepViolet = "";
    static constexpr cstring FontBrown = "";
    static constexpr cstring FontGray = "";
    static constexpr cstring FontDarkGreen = "";
    static constexpr cstring FontLightGray = "";
    static constexpr cstring FontLightCyan = "";
    static constexpr cstring FontPink = "";

    /* Background color */

    /* Basics */
    static constexpr cstring BgBlack = "";
    static constexpr cstring BgWhite = "";

    /* Primaries */
    static constexpr cstring BgRed = "";
    static constexpr cstring BgBlue = "";
    static constexpr cstring BgGreen = "";
    static constexpr cstring BgDarkGreen = "";
    static constexpr cstring BgYellow = "";

    /* Expanded */
    static constexpr cstring BgCyan = "";
    static constexpr cstring BgDeepViolet = "";
    static constexpr cstring BgBrown = "";
    static constexpr cstring BgGray = "";
    static constexpr cstring BgLightGray = "";
    static constexpr cstring BgLightCyan = "";
    static constexpr cstring BgPink = "";
};

}
} //Coffee

#endif

#include "android/environment.h"
#include "linux/environment.h"
#include "windows/environment.h"
