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

} //Coffee

#endif

#include "unix/environment.h"
#include "windows/environment.h"