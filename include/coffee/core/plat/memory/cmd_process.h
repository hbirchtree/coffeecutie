#pragma once

#include "cmd_interface.h"
#include "../plat_environment.h"

#ifndef COFFEE_WINDOWS

#include <sys/wait.h>
#include <stdio.h>

#endif

namespace Coffee{
namespace Environment{

struct Command
{
    cstring program;
    Vector<cstring> argv;
    Vector<cstring> envp;

    cstring workdir;
};

struct CProcess
{
    using Environment = Env::Variables;

	STATICINLINE int ExecuteSystem(Command const& cmd_)
	{
		CString cmd = cmd_.program;
		for (cstring arg : cmd_.argv)
		{
			cmd.push_back(' ');
			cmd.append(arg);
		}
		return system(cmd.c_str());
	}

    STATICINLINE int Execute(Command const& cmd_)
    {
#if defined(COFFEE_UNIXPLAT)
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
            child_sig = execvpe(
                        cmd.program,
                        (cstring_w const*)cmd.argv.data(),

                        (cmd.envp.size()>0)
                        ? (cstring_w const*)cmd.envp.data()
                        : environ);

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
#if defined(COFFEE_UNIXPLAT)
        CString cmd = cmd_.program;
        for(cstring arg : cmd_.argv)
        {
            cmd.push_back(' ');
            cmd.append(arg);
        }

        cmd.append(" 2>&1");

        out->resize(100);

        uint64 i = 0;
        FILE* pip = popen(cmd.c_str(),"r");
        if(pip != nullptr)
        {
            while(!feof(pip))
            {
                fread(&(*out)[i++],1,1,pip);
                if(i >= out->size())
                    out->resize(out->size()*2);
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
