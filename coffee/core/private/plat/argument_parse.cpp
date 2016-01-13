#include <plat/argument_parse.h>

#if defined(COFFEE_WINDOWS)

#include <Windows.h>
#include <WinUser.h>

#elif defined(COFFEE_LINUX)

#include <stdlib.h>

#endif

namespace Coffee{
#if defined(COFFEE_LINUX)
cstring_w executable_name(cstring_w n)
{
    C_UNUSED(n);
    return realpath("/proc/self/exe",nullptr);
}

#elif defined(COFFEE_WINDOWS)

cstring_w coffee_executable_name(cstring_w path)
{
    if(!path)
        path = (cstring_w)c_alloc(COFFEE_MAX_FILEPATH_BUFFER_SIZE);

    DWORD size = GetModuleFileNameA(NULL,path,COFFEE_MAX_FILEPATH_BUFFER_SIZE);
        path[size] = '\0';
        return path;
}

#endif

}
