#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee{

struct EnvInterface
{
    using Variables = Map<CString,CString>;
    using VarPair = std::pair<CString,CString>;

    struct TermScreen
    {
	static bool UsingAlternateBuffer;
    };

    static CString ExecutableName(cstring_w = nullptr);
    static CString GetVar(cstring);
    static bool SetVar(cstring, cstring);
    static bool UnsetVar(cstring);
    static bool ClearEnv();

    static CString GetPathSep();
    static CString ConcatPath(cstring, cstring);
    static CString GetUserHome();

    static CString GetUserData(cstring, cstring);

    static CString ApplicationDir();
    static CString CurrentDir();

    static Variables Environment()
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

} //Coffee

#endif

#include "unix/environment.h"
#include "windows/environment.h"
