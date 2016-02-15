#include <coffee/core/plat/windows/win_core.h>

#include <coffee/core/plat/platform_detect.h>
#include <coffee/core/coffee_macros.h>
#include <mutex>
#include <atomic>

#ifdef COFFEE_WINDOWS____
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Coffee{

void coffee_enable_core_dump()
{
    C_STUBBED("Core dumping");
    //We don't support this yet.
}

namespace CResources{
namespace CFiles{

bool coffee_file_mkdir(cstring dname, bool createParent)
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
szptr coffee_file_get_size(cstring file)
{
    CWinFile* fp = _winapi_open_file_read(file);

    szptr ret = _winapi_file_get_size(fp);

    CloseHandle(fp->hd);
    delete fp;
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
    namespace CDebugHelpers{
        cstring_w coffee_clock_string()
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
        void coffee_clock_free(cstring_w arg)
        {
            c_free(arg);
        }
    }

    struct WindowsPerformanceCounterData {
        WindowsPerformanceCounterData()
        {
            QueryPerformanceFrequency(&freq);
        }

        std::mutex perf_lock;
        LARGE_INTEGER freq;
        std::atomic_bool loaded;
    };

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
