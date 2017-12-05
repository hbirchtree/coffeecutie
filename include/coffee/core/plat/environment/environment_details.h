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

    /*!
     * \brief Get executable name, without path
     * \return
     */
    STATICINLINE CString ExecutableName()
    {
        return CString();
    }

    /*!
     * \brief Get base name of an arbitrary path, should work like basename
     * \return
     */
    STATICINLINE CString BaseName(cstring path)
    {
        (void)path;
        return CString();
    }
    /*!
     * \brief Directory name from arbitrary path, should work like dirname
     * \return
     */
    STATICINLINE CString DirName(cstring path)
    {
        (void)path;
        return CString();
    }

    /*!
     * \brief Check if env var given by argument exists
     * \return
     */
    STATICINLINE bool ExistsVar(cstring var)
    {
        (void)var;
        return true;
    }
    /*!
     * \brief Get value from env
     * \return
     */
    STATICINLINE CString GetVar(cstring var)
    {
        (void)var;
        return CString();
    }
    /*!
     * \brief Set env var to value
     * \return
     */
    STATICINLINE bool SetVar(cstring var, cstring value)
    {
        (void)var;
        (void)value;
        return false;
    }

    /*!
     * \brief Prepend to a variable, useful for $PATH vars
     * \return
     */
    STATICINLINE bool PrependVar(cstring var,cstring value)
    {
        (void)var;
        (void)value;
        return false;
    }
    /*!
     * \brief Append to var, useful for $PATH vars when not overriding
     * \return
     */
    STATICINLINE bool AppendVar(cstring var,cstring value)
    {
        (void)var;
        (void)value;
        return false;
    }

    /*!
     * \brief Unset an env var
     * \return
     */
    STATICINLINE bool UnsetVar(cstring var)
    {
        (void)var;
        return false;
    }
    /*!
     * \brief Clear env vars, makes them empty
     * \return
     */
    STATICINLINE bool ClearEnv()
    {
        return false;
    }

    /*!
     * \brief Get path separation character, / or \\
     * \return
     */
    STATICINLINE CString GetPathSep()
    {
        return CString();
    }
    STATICINLINE CString ConcatPath(cstring p1, cstring p2)
    {
        (void)p1;
        (void)p2;

        return CString(p1) + '/' + p2;
    }
    /*!
     * \brief Get user home directory, $HOME on *nix, %USERPROFILE% on Windows
     * \return
     */
    STATICINLINE CString GetUserHome()
    {
        return CString();
    }

    /*!
     * \brief GetUserData, takes arguments of organization name and application name respectively
     * \param orgname
     * \param appname
     * \return
     */
    STATICINLINE CString GetUserData(cstring orgname, cstring appname)
    {
        (void)orgname;
        (void)appname;
        /* Typically ConcatPath(*user data dir*, ConcatPath(orgname, appname)) */
        return CString();
    }

    /*!
     * \brief Directory of the running application's executable
     * \return
     */
    STATICINLINE CString ApplicationDir()
    {
        return CString();
    }
    /*!
     * \brief Current directory where we are executing
     * \return
     */
    STATICINLINE CString CurrentDir()
    {
        return CString();
    }

    /*!
     * \brief Get map of env vars
     * \return
     */
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
