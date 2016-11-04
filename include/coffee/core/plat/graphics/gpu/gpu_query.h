#pragma once

#include "../../../types/tdef/integertypes.h"


namespace Coffee{

class SWVersionInfo;
class HWDeviceInfo;

namespace GpuInfo{

struct MemStatus
{
    uint64 total, used, free;
};
struct PciStatus
{
    uint8 generation, curr_gen;
    uint8 linkWidth, curr_linkWidth;
};
struct TempRange
{
    scalar current;
    scalar max;
};
struct ClockRange
{
    uint32 current, min, max;
};
struct UsageMeter
{
    uint8 mem, gpu;
};
struct TransferStatus
{
    uint32 rx, tx;
};
enum class Clock
{
    Graphics, Memory,
};
enum class PMode
{
    Performance, Powersave
};

using gpucount_t = uint32;
using proc_t = uint32;

struct GpuQueryInterface
{
    using PGetDriver = SWVersionInfo(*)();
    using PGetNumGpus = gpucount_t(*)();
    using PGpuModel = HWDeviceInfo(*)(gpucount_t);
    using PMemoryInfo = MemStatus(*)(gpucount_t);
    using PProcMemoryUse = uint64(*)(gpucount_t,proc_t);
    using PGetTemperature = TempRange(*)(gpucount_t);
    using PGetClock = ClockRange(*)(gpucount_t,Clock);
    using PGetPowerMode = PMode(*)(gpucount_t);
    using PGetUsage = UsageMeter(*)(gpucount_t);
    using PGetPcieTransfer = TransferStatus(*)(gpucount_t);

    PGetDriver GetDriver;
    PGetNumGpus GetNumGpus;
    PGpuModel GpuModel;
    PMemoryInfo MemoryInfo;
    PProcMemoryUse ProcMemoryUse;
    PGetTemperature GetTemperature;
    PGetClock GetClock;
    PGetPowerMode GetPowerMode;
    PGetUsage GetUsage;
    PGetPcieTransfer GetPcieTransfer;
};

extern GpuQueryInterface GetDefault();

/*!
 * \brief Load GPU query library externally
 * Will naturally fail if required libraries do not exist
 * \param loc Where to put the query pointers
 * \return True if external library was loaded, false if internal dummy
 */
extern bool LoadDefaultGpuQuery(GpuQueryInterface* loc);

}
}

/*!
 * \brief Allow creating a constructor in external library
 */
struct GpuQueryFunction
{
    using Getter = Coffee::GpuInfo::GpuQueryInterface(*)();

    Getter ptr;
};

/*!
 * \brief Sample of what type of function should go into an external library
 * \param gpu
 * \return
 */
extern "C" GpuQueryFunction* GetGpuQueryDefault();
