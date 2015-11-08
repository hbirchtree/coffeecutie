#ifdef _WIN32

#ifndef WIN_CORE
#define WIN_CORE

#include "coffee/core/plat/platform_detect.h"
#include "coffee/core/coffee_types.h"
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
