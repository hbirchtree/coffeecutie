#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"

namespace Coffee{
namespace Environment{

struct EnvInterface
{
    using Variables = Map<CString,CString>;
    using VarPair = Pair<CString,CString>;

    struct TermScreen
    {
	static bool UsingAlternateBuffer;
    };

    STATICINLINE CString ExecutableName()
    {
        return CString();
    }

    STATICINLINE CString BaseName(cstring)
    {
        return CString();
    }
    STATICINLINE CString DirName(cstring)
    {
        return CString();
    }

    STATICINLINE bool ExistsVar(cstring)
    {
        return true;
    }
    STATICINLINE CString GetVar(cstring)
    {
        return CString();
    }
    STATICINLINE bool SetVar(cstring, cstring)
    {
        return false;
    }

    STATICINLINE bool PrependVar(cstring,cstring)
    {
        return false;
    }
    STATICINLINE bool AppendVar(cstring,cstring)
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

    STATICINLINE CString GetUserData(cstring orgname, cstring appname)
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

namespace CmdColor
{
enum CmdColor_t : uint8
{
    Black, White, Gray, LightGray,

    Red, Green, Blue, Yellow,
    Cyan, DeepViolet, Brown,
    DarkGreen, LightCyan,
    Pink,

    MaxColor
};
}

struct EnvColorCodes
{
    using CmdFormat = uint16;
    using CmdColor = CmdColor::CmdColor_t;

    STATICINLINE CmdFormat CombineFormat(uint8 fg, uint8 bg)
    {
        return static_cast<uint16>(fg + (bg << 8));
    }

    STATICINLINE CString& ColorText(CString& text, CmdFormat)
    {
        return text;
    }
};

}
} //Coffee

#endif
