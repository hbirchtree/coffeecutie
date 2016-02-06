#ifndef ENVIRONMENT_DETAILS
#define ENVIRONMENT_DETAILS

#include <coffee/core/CTypes>

namespace Coffee{
namespace Env{
struct TermScreen
{
    static bool UsingAlternateBuffer;
};

extern cstring_w GetVar(cstring var);

extern bool SetVar(cstring var, cstring value);

extern bool UnsetVar(cstring var);

extern bool ClearEnv();

extern cstring GetPathSep();

extern cstring_w ConcatPath(cstring_w target, cstring v2);

extern cstring_w GetUserHome();

extern cstring_w GetUserData(cstring orgname, cstring appname);

extern cstring_w ApplicationDir();

extern cstring_w CurrentDir();

}
} //Coffee

#endif
