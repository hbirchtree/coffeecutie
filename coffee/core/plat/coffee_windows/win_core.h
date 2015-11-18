#ifndef WIN_CORE
#define WIN_CORE

#include "coffee/core/plat/platform_detect.h"

#ifdef COFFEE_WINDOWS

#include <Windows.h>

#include "coffee/core/types/types.h"
#include "coffee/core/base/cdebug.h"

namespace Coffee{
    inline static void coffee_enable_core_dump()
    {

    }
namespace CResources{
        struct CWinFile;

        extern CWinFile* _winapi_open_file_read(cstring file);
        extern szptr _winapi_file_get_size(CWinFile *fp);
        extern szptr coffee_file_get_size(cstring file);
}
namespace CMemoryManagement{
        extern void* coffee_memory_map_file(cstring filename, szptr offset, szptr size);
        extern bool coffee_memory_unmap_file(void* ptr, szptr size);
}

namespace CFunctional {
    namespace CDebugHelpers {
        inline static cstring_w* coffee_callstack(szptr* len, uint32 stackreduce = 0)
        {
            C_STUBBED("Callstack dumping");
            *len = 0;
            return nullptr;
        }
        inline static cstring_w coffee_clock_string()
        {
            cwstring clock_fmt = c_str_wideconvert("HH:mm:ss");

            size_t len = GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT,
                                         TIME_FORCE24HOURFORMAT,
                                         NULL,
                                         clock_fmt,
                                         NULL,
                                         0);
            cwstring_w wstr = (cwstring_w)c_calloc(sizeof(int16),len);
            GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT,
                            TIME_FORCE24HOURFORMAT,
                            NULL,
                            clock_fmt,
                            wstr,
                            len);
            cstring_w str = c_str_narrowconvert(wstr);
            c_free(wstr);
            return str;
        }
        inline static void coffee_clock_free(cstring_w arg)
        {
            c_free(arg);
        }
    }


    //Stub for timers
    template<typename T>
    class _coffee_stub_timer
    {
    public:
        void start() {}
        T elapsed() { return 0; }
    };
    class CElapsedTimer : public _coffee_stub_timer<uint64> {};
    class CElapsedTimerMicro : public _coffee_stub_timer<uint64> {};
    class CElapsedTimerD : public _coffee_stub_timer<double> {};
}

namespace CResources{
namespace CFiles{

inline static bool coffee_file_mkdir(cstring dname, bool createParent = false)
{
    if(!createParent)
    {
        return CreateDirectory(dname,NULL);
    }else{
        return false;
    }
}

}
}


}

#endif //_WIN32
#endif //WIN_CORE
