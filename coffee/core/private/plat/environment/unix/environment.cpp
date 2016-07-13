#include <coffee/core/plat/environment/unix/environment.h>

#include <unistd.h>
#include <libgen.h>
#include <limits.h>

namespace Coffee{
namespace Environment{
namespace Posix{

CString PosixEnvironmentFun::BaseName(cstring n)
{
#ifndef COFFEE_USE_POSIX_BASENAME
    // This one is fast, but does not handle rootfs
    int64 idx = Search::ChrFindR(n,'/')-n;
    if(idx < 0)
        return n;
    CString out;
    out.insert(0,&n[idx+1],StrLen(n)-idx-1);
    if(out.empty())
        out = ".";
    return out;
#else
    // This one is slower
    sbyte_t* out = AllocT<sbyte_t>(StrLen(dname));
    CString out_s = basename(out);
    CFree(out);
    return out_s;
#endif
}

CString PosixEnvironmentFun::DirName(cstring fname)
{
    int64 idx = Search::ChrFindR(fname,'/')-fname;
    if(idx < 0)
        return fname;
    CString out;
    out.insert(0,&fname[0],idx);
    if(out.empty())
        out = ".";
    return out;
}

bool PosixEnvironmentFun::PrependVar(cstring var, cstring val)
{
    CString new_val = val;
    new_val += GetVar(var);
    return SetVar(var,new_val.c_str());
}

bool PosixEnvironmentFun::AppendVar(cstring var, cstring val)
{
    CString new_val = GetVar(var);
    new_val += val;
    return SetVar(var,new_val.c_str());
}

CString PosixEnvironmentFun::ConcatPath(cstring v1, cstring v2)
{
    return v1+GetPathSep()+v2;
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

#if defined(COFFEE_APPLE)
CString Coffee::Environment::Mac::MacEnv::ExecutableName(cstring_w)
{
    int ret;
    pid_t pid;
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE] = {};

    pid = getpid();
    ret = proc_pidpath(pid,pathbuf,sizeof(pathbuf));
    CString out;
    out.insert(0,pathbuf,sizeof(pathbuf));
    szptr e = out.find('\0');
    out.resize(e);
    return out;
}
#endif

}
}
