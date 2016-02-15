#include "../../platform_detect.h"
#ifdef COFFEE_WINDOWS

#pragma once

#include "../sysinfo_def.h"

namespace Coffee {
	namespace Windows {
		struct WindowsSysInfo : SysInfoDef
		{

			STATICINLINE uint32 CpuCount()
			{
				return 1;
			}
			STATICINLINE uint32 CoreCount()
			{
				return 1;
			}
			STATICINLINE bool MemVirtualAvailable()
			{
				return false;
			}
			STATICINLINE int64 MemTotal()
			{
				return 0;
			}
			STATICINLINE int64 MemAvailable()
			{
				return 0;
			}
			STATICINLINE int64 SwapTotal()
			{
				return 0;
			}
			STATICINLINE int64 SwapAvailable()
			{
				return 0;
			}
			STATICINLINE HWDeviceInfo Processor()
			{
				return HWDeviceInfo("Generic","Generic");
			}
			STATICINLINE bigscalar ProcessorFrequency()
			{
				return 0;
			}
			STATICINLINE bool HasFPU()
			{
				return false;
			}
			STATICINLINE int64 ProcessorCacheSize()
			{
				return 0;
			}
			STATICINLINE bool HasHyperThreading()
			{
				return false;
			}
			STATICINLINE NetStatusFlags NetStatus()
			{
				return NetStatLocalOnly;
			}
		};
	}

	using SysInfo = Windows::WindowsSysInfo;
}

#endif