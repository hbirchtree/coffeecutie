#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include <coffee/core/CTypes>

namespace Coffee{

struct EnvInterface
{
    struct TermScreen
    {
	static bool UsingAlternateBuffer;
    };

    static CString ExecutableName(cstring_w);
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
};

//namespace Env{

//extern cstring_w GetVar(cstring var);

//extern bool SetVar(cstring var, cstring value);

//extern bool UnsetVar(cstring var);

//extern bool ClearEnv();

//extern cstring GetPathSep();

//extern cstring_w ConcatPath(cstring_w target, cstring v2);

//extern cstring_w GetUserHome();

//extern cstring_w GetUserData(cstring orgname, cstring appname);

//extern cstring_w ApplicationDir();

//extern cstring_w CurrentDir();

//}
} //Coffee

#endif

#include "unix/environment.h"
