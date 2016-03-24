#include "../../platform_detect.h"
#ifdef COFFEE_WINDOWS

#pragma once

#include "../sysinfo_def.h"

#include <coffee/core/plat/plat_windows.h>

#include "../../memory/cmemory.h"
#include <intrin.h>

namespace Coffee {
	namespace Windows {
		struct WindowsSysInfo : SysInfoDef
		{
			enum CPUID_Data
			{
				BasicInfo = 0x1,

				Extended_Start = 0x80000000,

				CPUBrand_1 = 0x80000002,
				CPUBrand_2 = 0x80000003,
				CPUBrand_3 = 0x80000004,

				CacheSizes = 0x80000006,
			};

			enum CPUID_Features
			{
				FPU,
				Virtual8086,
				DebuggingExt,
				PageSizeExt,
				TimeStamp,
				WRDMSR,
				PAE,
				MachCheckExcept,
				CMPXCHG8B,
				APIC,
				Unk1,
				SYSENTEREXIT,
				MTRR,
				PTE,
				MachCheckArch,
				CondMove,
				PageAttrTable,
				PageSizeExtended,
				ProcSerial,
				CFLUSH,
				Unk2,
				DbgStore,
				ThermMonClk,
				MMX,
				FXSaveStor,
				SSE,
				SSE2,
				SelfSnoop,
				Hyperthreading,
				ThermMon,
				Unk4,
				PendBrk,
			};

			struct proc_info
			{
				struct  cache
				{
					uint16 l1;
					uint16 l2;
					uint16 l3;
				};
				struct proc
				{
					cache cache;
					uint32 cores;
					uint32 threads;
				};

				Vector<proc> processors;
			};

			STATICINLINE uint32 CountThreads(ULONG_PTR bitm)
			{
				uint32 bitc = 0;
				DWORD lshift = sizeof(ULONG_PTR) * 8 - 1;
				ULONG_PTR bitt = (ULONG_PTR)1 << lshift;
				
				for (uint32 i = 0;i <= lshift;++i)
				{
					bitc += ((bitm&bitt) ? 1 : 0);
					bitt /= 2;
				}
				return bitc;
			}

			STATICINLINE proc_info GetProcInfo()
			{
				PSYSTEM_LOGICAL_PROCESSOR_INFORMATION st = nullptr;
				DWORD rsize = 0;
				BOOL done = false;

				while (!done)
				{
					BOOL rc = GetLogicalProcessorInformation(st, &rsize);
					if (!rc)
					{
						if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
						{
							if (st)
								CFree(st);
							else
								st = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)Alloc(rsize);
						}
						else
							return {};
					}
					else
						break;
				}

				PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = st;
				DWORD byte_offset = 0;
				PCACHE_DESCRIPTOR cache;

				proc_info info;
				info.processors.push_back({});
				proc_info::proc* proc = &info.processors.back();

				uint32 procc = 0;

				while (byte_offset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= rsize)
				{
					switch (ptr->Relationship)
					{
					case RelationNumaNode:
						break;
					case RelationProcessorPackage:
						if (++procc == 1)
							break;
						info.processors.push_back({});
						proc = &info.processors.back();
						break;
					case RelationProcessorCore:
						proc->cores++;
						proc->threads += CountThreads(ptr->ProcessorMask);
						break;
					case RelationCache:
						cache = &ptr->Cache;
						if (cache->Level == 1)
						{
							proc->cache.l1++;
						}
						else if (cache->Level == 2)
						{
							proc->cache.l2++;
						}
						else if (cache->Level == 3)
						{
							proc->cache.l3++;
						}
						break;
					}
					byte_offset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
					ptr++;
				}

				CFree(st);

				return info;
			}

			STATICINLINE uint32 CpuCount()
			{
				proc_info info = GetProcInfo();
				return info.processors.size();
			}
			STATICINLINE uint32 CoreCount()
			{
				uint32 c = 0;

				proc_info info = GetProcInfo();

				for (proc_info::proc const& p : info.processors)
					c += p.cores;

				return c;
			}
			STATICINLINE bool MemVirtualAvailable()
			{
				return true;
			}
			STATICINLINE uint64 MemTotal()
			{
				MEMORYSTATUSEX st;
				st.dwLength = sizeof(st);

				GlobalMemoryStatusEx(&st);

				return st.ullTotalPhys;
			}
			STATICINLINE uint64 MemAvailable()
			{
				MEMORYSTATUSEX st;
				st.dwLength = sizeof(st);

				GlobalMemoryStatusEx(&st);

				return st.ullAvailPhys;
			}
			STATICINLINE uint64 SwapTotal()
			{
				MEMORYSTATUSEX st;
				st.dwLength = sizeof(st);

				GlobalMemoryStatusEx(&st);

				return st.ullTotalPageFile;
			}
			STATICINLINE uint64 SwapAvailable()
			{
				MEMORYSTATUSEX st;
				st.dwLength = sizeof(st);

				GlobalMemoryStatusEx(&st);

				return st.ullAvailPageFile;
			}
			STATICINLINE HWDeviceInfo Processor()
			{
				char CPUString[0x20];
				char CPUBrandString[0x40];

				int CPUInfo[4];

				__cpuid(CPUInfo,0);

				MemClear(CPUString, 0x20);
				MemClear(CPUBrandString, 0x40);

				MemCpy(CPUString,&CPUInfo[1],sizeof(int)*3);

				__cpuid(CPUInfo, Extended_Start);
				int nExIds = CPUInfo[0];

				for (int i = Extended_Start;i < nExIds;++i)
				{
					__cpuid(CPUInfo,i);

					if (i > CPUBrand_3)
						break;

					if (i == CPUBrand_1)
					{
						MemCpy(CPUBrandString + 0, CPUInfo, sizeof(CPUInfo));
					}
					else if (i == CPUBrand_2)
					{
						MemCpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
					}
					else if (i == CPUBrand_3)
					{
						MemCpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
					}
				}

				return HWDeviceInfo(CPUString,CPUBrandString,"0x0");
			}
			STATICINLINE bigscalar ProcessorFrequency()
			{
				LARGE_INTEGER e;
				QueryPerformanceFrequency(&e);
				return ((e.HighPart << 32)+(e.LowPart))/1000000.0;
			}
			STATICINLINE bool HasFPU()
			{
				return IsProcessorFeaturePresent(PF_FLOATING_POINT_EMULATED)==0;
			}
			STATICINLINE uint64 ProcessorCacheSize()
			{
				int64 csize = 0;

				int CPUInfo[4];

				__cpuid(CPUInfo, Extended_Start);
				int nIds = CPUInfo[0];


				for (int i = Extended_Start;i < nIds;++i)
				{
					if (i > 1)
						break;

					if (i == CacheSizes)
					{
						csize = (CPUInfo[2] >> 16) & 0xffff;
						csize *= 1024;
					}
				}

				return csize;
			}
			STATICINLINE bool HasHyperThreading()
			{
				bool dht = false;

				int CPUInfo[4];

				__cpuid(CPUInfo, 0);
				int nIds = CPUInfo[0];


				for (int i = 0;i < nIds;++i)
				{
					if (i > 1)
						break;

					if (i == 1)
					{
						dht = CPUInfo[3] & Hyperthreading;
					}
				}

				return dht;
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