#pragma once

#include "cmd_interface.h"
#include "../plat_environment.h"

namespace Coffee{
namespace Environment{

struct CProcess
{
    using Environment = Env::Variables;

    STATICINLINE int Execute(cstring cmd)
    {
        /*TODO: Replace system()*/
        return system(cmd);
    }

    STATICINLINE int Execute(cstring program, cstring_w const* argv, cstring_w const* env)
    {
        if(!env)
            return execvp(program,argv);
        else
            return execvpe(program,argv,env);
    }

    STATICINLINE int Execute_s(cstring program, Vector<cstring> const& args, Environment const* env)
    {
        Vector<cstring> arg_v = args;
        arg_v.push_back(nullptr);

        /*TODO: Implement functionality for env*/

        /*NOTE: The C-cast below is quite triggering */
        return Execute(program,(cstring_w const*)args.data(),nullptr);
    }
};

}

using Proc = Environment::CProcess;

}
