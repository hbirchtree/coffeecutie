#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#pragma once

#include <coffee/core/CTypes>
#include "../cfile.h"

namespace Coffee {
	namespace Windows {
		struct WinDirFun : CResources::DirFunDef
		{
			/*TODO: Implement Windows directory functions*/
			STATICINLINE bool MkDir(cstring, bool)
			{
				return false;
			}
			STATICINLINE bool RmDir(cstring)
			{
				return false;
			}
		};
                struct WinFileFun : CResources::CPlatFileFun
		{
			/*TODO: Implement Windows file mapping functions*/
			STATICINLINE void* Map(cstring, ResourceAccess, szptr, szptr, int*)
			{
				return nullptr;
			}
			STATICINLINE bool  Unmap(void*, szptr)
			{
				return false;
			}
		};
	}

	using DirFun = Windows::WinDirFun;
	using FileFun = Windows::WinFileFun;
}

#endif
