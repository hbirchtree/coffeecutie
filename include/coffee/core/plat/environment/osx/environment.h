#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../posix/environment.h"

#include <libproc.h>
#include <errno.h>

#include "../../memory/stlstring_ops.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacEnv : Posix::PosixEnvironmentFun
{
    STATICINLINE CString ExecutableName(cstring_w = nullptr)
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
    STATICINLINE CString GetUserData(cstring orgname, cstring appname)
    {
        CString dir = GetVar("HOME");
        dir = ConcatPath(dir.c_str(),"Library/Application Support");
        dir = ConcatPath(dir.c_str(),orgname);
        dir = ConcatPath(dir.c_str(),appname);
        return "";
    }
};

struct MacColorMap : Posix::PosixTerminalColorCodes
{
};

}
}

using Env_ = Environment::Mac::MacEnv;
using ColorMap_ = Environment::Mac::MacColorMap;

}
#endif
