#pragma once

#include <coffee/core/base/files/url.h>
#include <coffee/core/plat/memory/string_ops.h>

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
     * \brief Check if env var given by argument exists
     * \return
     */
    STATICINLINE bool ExistsVar(cstring var)
    {
        (void)var;
        return false;
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
     * \brief Get user home directory, $HOME on *nix, %USERPROFILE% on Windows
     * \return
     */
    STATICINLINE Url GetUserHome()
    {
        return Url();
    }

    /*!
     * \brief GetUserData, takes arguments of organization name and application name respectively
     * \param orgname
     * \param appname
     * \return
     */
    STATICINLINE Url GetUserData(cstring orgname, cstring appname)
    {
        (void)orgname;
        (void)appname;
        /* Typically ConcatPath(*user data dir*, ConcatPath(orgname, appname)) */
        return Url();
    }

    /*!
     * \brief Directory of the running application's executable
     * \return
     */
    STATICINLINE Url ApplicationDir()
    {
        return Url();
    }
    /*!
     * \brief Current directory where we are executing
     * \return
     */
    STATICINLINE Url CurrentDir()
    {
        return Url();
    }

    /*!
     * \brief Get map of env vars
     * \return
     */
    STATICINLINE Variables Environment()
    {
        return Variables();
    }

    STATICINLINE CString ConcatPath(cstring p1, cstring p2)
    {
        if(str::len(p1) > 0)
            return CString(p1) + "/" + p2;
        else
            return p2;
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

    STATICINLINE CmdFormat CombineFormat(u8 fg, u8 bg)
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
