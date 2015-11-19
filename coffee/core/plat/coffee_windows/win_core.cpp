#include "win_core.h"

#include "coffee/core/plat/platform_detect.h"
#include <mutex>
#include <atomic>

#ifdef COFFEE_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Coffee{
namespace CResources{

struct CWinFile
{
    HANDLE hd;
    void* data;
};

CWinFile* _winapi_open_file_read(cstring file){
    CWinFile *f = new CWinFile;
	cstring wfname = c_str_replace(file, "/", "\\");
    f->hd = CreateFile(wfname, GENERIC_READ,
                      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    return f;
}
szptr coffee_file_get_size(cstring file)
{
    CWinFile* fp = _winapi_open_file_read(file);

    szptr ret = _winapi_file_get_size(fp);

    CloseHandle(fp->hd);
    delete fp;
    return ret;
}
szptr _winapi_file_get_size(CWinFile *fp)
{
    if (fp->hd == INVALID_HANDLE_VALUE)
        return 0;

    LARGE_INTEGER sz;
    szptr ret = 0;
    if (GetFileSizeEx(fp->hd, &sz))
        ret = (sz.LowPart | sz.HighPart << 32);
    return ret;
}
}
namespace CMemoryManagement{
void* coffee_memory_map_file(cstring filename, szptr offset, szptr size, int*)
{
    CResources::CWinFile* fp = CResources::_winapi_open_file_read(filename);
    szptr len = CResources::_winapi_file_get_size(fp);

    HANDLE map = CreateFileMapping(fp, NULL, PAGE_READONLY, 0, 0, 0);
    if (map == NULL)
    {
        return nullptr;
    }

    void* ptr = MapViewOfFile(map, FILE_MAP_READ, 0, 0, size);

    //Close file handle at some point!
    return ptr;
}
bool coffee_memory_unmap_file(void* ptr, szptr size)
{
    return UnmapViewOfFile(ptr);
}

}

namespace CFunctional {

	struct WindowsPerformanceCounterData {
		WindowsPerformanceCounterData()
		{
			QueryPerformanceFrequency(&freq);
		}

		std::mutex perf_lock;
		LARGE_INTEGER freq;
		std::atomic_bool loaded;
	};

	static WindowsPerformanceCounterData _win_perfcounter_data;

	uint64 _win_api_get_time()
	{
		LARGE_INTEGER tmp;
		QueryPerformanceCounter(&tmp);
		uint64 tick = tmp.QuadPart;
		return (tick*1000000)/_win_perfcounter_data.freq.QuadPart;
	}
}
}

#endif
