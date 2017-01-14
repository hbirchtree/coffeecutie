#pragma once

#include "cmd_interface.h"
#include "../plat_environment.h"

#ifndef COFFEE_WINDOWS

#include <sys/wait.h>
#include <stdio.h>
#include <string>

#endif

namespace Coffee{
namespace Environment{

struct Command
{
    CString program;
    Vector<CString> argv;
    Vector<cstring> envp;

    cstring workdir;

    Vector<CString> _string_store;
};

struct CProcess
{
    using Environment = Env::Variables;

	STATICINLINE int ExecuteSystem(Command const& cmd_)
	{
#if defined(COFFEE_ANDROID) || defined(__EMSCRIPTEN__)
        return -1;
#elif !defined(COFFEE_WINDOWS_UWP)
		CString cmd = cmd_.program;
		for (CString const& arg : cmd_.argv)
		{
			cmd.push_back(' ');
			cmd.append(arg);
		}
		return system(cmd.c_str());
#else
		return -1;
#endif
	}

    STATICINLINE int Execute(Command const& cmd_)
    {
#if defined(COFFEE_ANDROID) || defined(__EMSCRIPTEN__)
        return -1;
#elif defined(COFFEE_UNIXPLAT)
        Command cmd = cmd_;

        pid_t child;
        int child_sig;

        cmd.argv.insert(cmd.argv.begin(),cmd.program);
        cmd.argv.push_back(nullptr);

        if(cmd.envp.size() > 0)
            cmd.envp.push_back(nullptr);

        child = fork();

        if(child == 0)
        {
#ifndef COFFEE_APPLE
            child_sig = execvpe(
                        cmd.program.c_str(),
                        (cstring_w const*)cmd.argv.data(),

                        (cmd.envp.size()>0)
                        ? (cstring_w const*)cmd.envp.data()
                        : environ);
#endif
            exit(0);
        }

        pid_t tpid;
        do{
            tpid = wait(&child_sig);
        }while(tpid != child);

        return child_sig;
#else
		return ExecuteSystem(cmd_);
#endif
    }

    STATICINLINE int ExecuteLogged(Command const& cmd_, CString* out, CString* err = nullptr)
    {
#if defined(COFFEE_ANDROID) || defined(__EMSCRIPTEN__)
        return -1;
#elif defined(COFFEE_UNIXPLAT)
        CString cmd = cmd_.program;
	for(CString const& arg : cmd_.argv)
        {
            cmd.push_back(' ');
            cmd.append(arg);
	}

        out->resize(100);

        uint64 i = 0;
        CString tmp;
        tmp.resize(100);
        FILE* pip = popen(cmd.c_str(),"r");
        if(pip != nullptr)
        {
            while(fgets(&tmp[0],tmp.capacity(),pip) != nullptr)
            {
                fprintf(stderr,"%s",tmp.c_str());
                out->insert(out->size(),&tmp[0],StrLen(tmp.c_str()));
                tmp.clear();
                tmp.resize(100);
            }
            out->at(i) = 0;
            return pclose(pip);
        }else{
            return -1;
        }
#else
		return ExecuteSystem(cmd_);
#endif
    }
};

}

using Proc = Environment::CProcess;
using Proc_Cmd = Environment::Command;

}
