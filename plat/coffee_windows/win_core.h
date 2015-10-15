#ifdef _WIN32

#ifndef WIN_CORE
#define WIN_CORE

#include <Windows.h>
#include "plat/platform_detect.h"
#include "coffee_types.h"
#include "coffee/cdebug.h"

namespace Coffee{
	inline static void coffee_enable_core_dump()
	{

	}
namespace CResources{
	static HANDLE _winapi_open_file_read(cstring file)
	{
		return CreateFile(file, GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	static szptr _winapi_file_get_size(HANDLE fp)
	{
		if (fp == INVALID_HANDLE_VALUE)
			return 0;

		LARGE_INTEGER sz;
		szptr ret = 0;
		if (GetFileSizeEx(fp, &sz))
			ret = sz;
		return ret;
	}

	static szptr coffee_file_get_size(cstring file)
	{
		HANDLE fp = _winapi_open_file_read(file);

		szptr ret = _winapi_file_get_size(fp);

		CloseHandle(fp);
		return ret;
	}
}
namespace CMemoryManagement{
	static void* coffee_memory_map_file(cstring filename, szptr offset, szptr size)
	{
		HANDLE fp = CResources::_winapi_open_file_read(filename);
		szptr len = CResources::_winapi_file_get_size(fp);

		HANDLE map = CreateFileMapping(fp, NULL, PAGE_READONLY, 0, 0, 0);
		if (map == 0)return nullptr;

		void* ptr = MapViewOfFile(map, FILE_MAP_READ, 0, 0, size);

		return ptr;
	}
	static bool coffee_memory_unmap_file(void* ptr, szptr size)
	{
		return UnmapViewOfFile(ptr);
	}
}

namespace CFunctional {
	namespace CDebugHelpers {
		inline static cstring_w* coffee_callstack(szptr* len, uint32 stackreduce = 0)
		{
			C_STUBBED("Callstack dumping");
			len = 0;
			return nullptr;
		}
		inline static cstring_w coffee_clock_string()
		{
			C_STUBBED("Clock strings");
			return "STUBBED";
		}
		inline static void coffee_clock_free(cstring_w arg)
		{
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


}

#endif //WIN_CORE

#endif //_WIN32
