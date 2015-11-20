#ifndef WIN_CORE
#define WIN_CORE

#include "coffee/core/plat/platform_detect.h"

#ifdef COFFEE_WINDOWS

#include <Windows.h>

#include "coffee/core/CTypes"
#include "coffee/core/base/cdebug.h"

namespace Coffee{
namespace CFunctional {
    namespace CDebugHelpers {
        inline static cstring_w* coffee_callstack(szptr* len, uint32 stackreduce = 0)
        {
            C_STUBBED("Callstack dumping");
            *len = 0;
            return nullptr;
        }
    }


    extern uint64 _win_api_get_time();

    template<typename T>
    class _cbasic_windows_timer : public _cbasic_timer<T>
    {
    public:
        virtual ~_cbasic_windows_timer() {}
        void start()
        {
            _start = _curr_time();
        }
        T elapsed()
        {
            return _curr_time() - _start;
        }
    protected:
        virtual T _curr_time() = 0;
    private:
        T _start = 0;
    };

    class _windows_CElapsedTimer : public _cbasic_windows_timer<uint64>
    {
    protected:
        uint64 _curr_time()
        {
            return _win_api_get_time()/1000;
        }
    };
    class _windows_CElapsedTimerMicro : public _cbasic_windows_timer<uint64>
    {
    protected:
        uint64 _curr_time()
        {
            return _win_api_get_time();
        }
    };
    class _windows_CElapsedTimerD : public _cbasic_windows_timer<bigscalar>
    {
    protected:
        bigscalar _curr_time()
        {
            return ((bigscalar)_win_api_get_time())/1000000.0;
        }
    };
}

}

#endif //_WIN32
#endif //WIN_CORE
