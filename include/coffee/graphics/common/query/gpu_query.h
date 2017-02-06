#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <iterator>

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

class GpuView
{
    gpucount_t m_gpu;
    GpuQueryInterface& m_interface;
public:
    GpuView(gpucount_t i, GpuQueryInterface& interface):
        m_gpu(i),
        m_interface(interface)
    {
    }

#define GPU_GLUE(outtype, fun, interface) outtype fun() const {return m_interface.interface(m_gpu);}

    GPU_GLUE(HWDeviceInfo, model, GpuModel)
    GPU_GLUE(MemStatus, mem, MemoryInfo)
//    GPU_GLUE(uint64, memUsage, ProcMemoryUse)
    GPU_GLUE(TempRange, temp, GetTemperature)
//    GPU_GLUE(ClockRange, clock, GetClock)
    GPU_GLUE(PMode, pMode, GetPowerMode)
    GPU_GLUE(UsageMeter, usage, GetUsage)
    GPU_GLUE(TransferStatus, bus, GetPcieTransfer)

#undef GPU_GLUE
};

class GpuQueryView
{
    GpuQueryInterface& m_interface;

public:
    class GpuQueryIterator : public std::iterator<std::forward_iterator_tag, GpuView>
    {
        friend class GpuQueryView;

        gpucount_t m_gpu;
        GpuQueryView* m_view;

        GpuQueryIterator(GpuQueryView& view, gpucount_t i):
            m_gpu(i),
            m_view(&view)
        {
        }

    public:

        bool operator !=(GpuQueryIterator const& other)
        {
            return m_gpu != other.m_gpu;
        }
        bool operator ==(GpuQueryIterator const& other)
        {
            return !(m_gpu != other.m_gpu);
        }

        GpuQueryIterator& operator++()
        {
            m_gpu++;
            return *this;
        }

        GpuView operator*()
        {
            return GpuView(m_gpu, m_view->m_interface);
        }

        operator CString()
        {
            return (*(*this)).model().model;
        }
    };

    using iterator = GpuQueryIterator;

    GpuQueryView(GpuQueryInterface& interface):
        m_interface(interface)
    {
    }

    iterator begin()
    {
        return iterator(*this, 0);
    }
    iterator end()
    {
        return iterator(*this, m_interface.GetNumGpus());
    }
};

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
