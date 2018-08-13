#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <iterator>

namespace Coffee{

enum class GpuQueryError
{
    SymbolResolution = 1,
    LibraryLoading,
    Initialization,
    InternalError,
    DependencyError,

    LibraryImplementation,

    ArgumentError,
};

struct gpu_query_category : error_category
{
    virtual const char *name() const noexcept override;
    virtual std::string message(int error_code) const override;
};

using gpu_query_error = domain_error_code<GpuQueryError, gpu_query_category>;

struct SWVersionInfo;
struct HWDeviceInfo;

namespace GpuInfo{

struct MemStatus
{
    u64 total, used, free;
};
struct PciStatus
{
    u8 generation, curr_gen;
    u8 linkWidth, curr_linkWidth;
};
struct TempRange
{
    scalar current;
    scalar max;
};
struct ClockRange
{
    u32 current, min, max;
};
struct UsageMeter
{
    u8 mem, gpu, decoder, encoder;
};
struct TransferStatus
{
    u32 rx, tx;
};
enum class Clock
{
    Graphics, Memory, VideoDecode, VideoEncode
};
enum class PMode
{
    Performance, Powersave
};

using gpucount_t = u32;
using proc_t = u32;

struct GpuQueryInterface
{
    using PGetDriver = SWVersionInfo(*)();
    using PGetNumGpus = gpucount_t(*)();
    using PGpuModel = HWDeviceInfo(*)(gpucount_t);
    using PMemoryInfo = MemStatus(*)(gpucount_t);
    using PProcMemoryUse = u64(*)(gpucount_t,proc_t);
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
extern bool LoadDefaultGpuQuery(GpuQueryInterface &loc, gpu_query_error& ec);

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

#define GPU_GLUE(outtype, fun, interface) outtype fun() const \
    { if(m_interface.interface) return m_interface.interface(m_gpu); return {};}
#define GPU_GLUE_2(outtype, fun, t2, interface) outtype fun(t2 arg2) const \
    {if(m_interface.interface) return m_interface.interface(m_gpu, arg2); return {};}

    GPU_GLUE(HWDeviceInfo, model, GpuModel)
    GPU_GLUE(MemStatus, mem, MemoryInfo)
    GPU_GLUE(TempRange, temp, GetTemperature)
    GPU_GLUE(PMode, pMode, GetPowerMode)
    GPU_GLUE(UsageMeter, usage, GetUsage)
    GPU_GLUE(TransferStatus, bus, GetPcieTransfer)

    GPU_GLUE_2(u64, memUsage, proc_t, ProcMemoryUse)
    GPU_GLUE_2(ClockRange, clock, Clock, GetClock)

#undef GPU_GLUE_2
#undef GPU_GLUE
};

class GpuQueryView
{
    GpuQueryInterface& m_interface;

public:
    class GpuQueryIterator : public Iterator<ForwardIteratorTag, GpuView>
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
