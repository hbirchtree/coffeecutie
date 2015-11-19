#ifndef WIN_CORE
#define WIN_CORE

#include "coffee/core/plat/platform_detect.h"

#ifdef COFFEE_WINDOWS

#include <Windows.h>

#include "coffee/core/CTypes"
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
        extern void* coffee_memory_map_file(cstring filename, szptr offset, szptr size,int*);
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

namespace CResources{
namespace CFiles{

inline static bool coffee_file_mkdir(cstring dname, bool createParent = false)
{
    if(!createParent)
    {
        return CreateDirectory(dname,NULL);
    }else{
        cstring_w path = c_cpy_string(dname);
        szptr bufsize = c_strlen(path) + 2;
        path = (cstring_w)c_realloc(path,bufsize);

        path[bufsize - 2] = '\\';
        path[bufsize - 1] = 0;

        //Beware of magic cookies!
        cstring_w folder = (cstring_w)c_calloc(sizeof(byte),255);
        cstring_w end;

        end = strchr(path, L'\\');
        while (end != NULL)
        {
            strncpy(folder,path,end-path+1);
            if (!CreateDirectory(folder, NULL))
            {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS)
                    cWarning("Error while creating directories: %i",err);
            }
            end = strchr(++end, L'\\');
        }

        c_free(folder);
        c_free(path);
        return false;
    }
}

}
}



}

#endif //_WIN32
#endif //WIN_CORE
