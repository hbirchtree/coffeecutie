#pragma once

#include "cmd_interface.h"
#include "../plat_environment.h"

namespace Coffee{
namespace Environment{

struct Command
{
    cstring program;
    cstring_w const* argv;
    cstring_w const* envp;
};

struct CProcess
{
    using Environment = Env::Variables;

    STATICINLINE int Execute(Command const& cmd)
    {
	if(!cmd.envp)
	    return execvp(cmd.program,cmd.argv);
        else
	    return execvpe(cmd.program,cmd.argv,cmd.envp);
    }

    STATICINLINE int Execute_s(cstring program, Vector<cstring> const& args, Environment const* env)
    {
        Vector<cstring> arg_v = args;
        arg_v.push_back(nullptr);

        /*TODO: Implement functionality for env*/

        /*NOTE: The C-cast below is quite triggering */
	return Execute({program,(cstring_w const*)args.data(),nullptr});
    }
};

}

using Proc = Environment::CProcess;
using Proc_Cmd = Environment::Command;

}
