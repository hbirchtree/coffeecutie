#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#pragma once

#include <coffee/core/CTypes>
#include "../file_def.h"

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
        struct WinFileFun : CResources::FileFunDef
		{
			struct FileHandle
			{
				HANDLE file;
				HANDLE mapping;
			};

			STATICINLINE FileHandle* Open()
			{
				return nullptr;
			}

			/*TODO: Implement Windows file mapping functions*/
			STATICINLINE void* Map(cstring, ResourceAccess, szptr, szptr, int*)
			{
				return nullptr;
			}
			STATICINLINE bool  Unmap(void*, szptr)
			{
				return false;
			}
			STATICINLINE bool Exists(cstring)
			{
				return false;
			}
		};
	}

	namespace CResources {
		using DirFun = Windows::WinDirFun;
		using FileFun = Windows::WinFileFun;
	}
}

#endif
