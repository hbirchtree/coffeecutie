#pragma once

#include <coffee/core/types/composite_types.h>
#include <coffee/core/plat/plat_environment.h>

namespace CoffeeExt{

struct QtSystem : Coffee::Subsystem
{
    STATICINLINE void Init()
    {
	int v = 1;
        Coffee::CString execname = Coffee::Env::ExecutableName();
	Coffee::cstring exec_c = execname.c_str();
	Init(v,(char**)&exec_c);
    }
    static bool Init(int& argc, char** argv);
    static bool Deinit();

    static bool Process(int timeout = 20);

    static int Exec();

    static Coffee::CString ApplicationName();
    static Coffee::CString OrganizationName();
    static void SetApplicationName(Coffee::cstring name, Coffee::cstring orgname = nullptr);
};

}
