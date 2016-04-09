#include <coffee/core/plat/environment/posix/environment.h>

namespace Coffee{
namespace Environment{
namespace Posix{

CString PosixEnvironmentFun::BaseName(const CString &n)
{
    cstring_w cp = AllocT<sbyte_t>(n.size()+1);

    StrCpy(cp,n.c_str());
    cp[n.size()] = 0;

    cstring_w r = basename(cp);

    CString imm;
    imm.insert(0,r,StrLen(r));

    CFree(cp);
    return imm;
}

CString PosixEnvironmentFun::ApplicationDir()
{
    CString tmp = ExecutableName();
    CString stmp = dirname(&tmp[0]);
    return stmp;
}

CString PosixEnvironmentFun::CurrentDir()
{
    CString dir;
    dir.resize(PATH_MAX);
    char* p = getcwd(&dir[0],PATH_MAX);
    dir.resize(strchr(dir.c_str(),'\0')-dir.c_str());
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
        v1.insert(0,(cstring)envar,q-envar);
        v2.insert(0,(cstring)q+1,StrLen(q+1));

        e.insert(VarPair(v1,v2));

        v1.clear();
        v2.clear();

        vn++;
        envar = environ[vn];
    }
    return e;
}

}
}
}
