#include <coffee/core/plat/graphics/gpu/gpu_query.h>
#include <coffee/core/plat/plat_linking.h>
#include <coffee/core/CDebug>

namespace Coffee{
namespace GpuInfo{

/*!
 * \brief Dummy functions for GpuQuery interface
 */
struct _GpuQueryDef
{
    static SWVersionInfo GetDriver()
    {
        return SWVersionInfo("Generic", 1, 0);
    }
    static gpucount_t GetNumGpus()
    {
        return 0;
    }

    static HWDeviceInfo GpuModel(gpucount_t)
    {
        return HWDeviceInfo("Generic", "0x0");
    }
    static MemStatus MemoryInfo(gpucount_t)
    {
        return {};
    }
    static uint64 ProcMemoryUse(gpucount_t,proc_t)
    {
        return 0;
    }
    static TempRange GetTemperature(gpucount_t)
    {
        return {};
    }
    static ClockRange GetClock(gpucount_t, Clock)
    {
        return {};
    }
    static PMode GetPowerMode(gpucount_t)
    {
        return {};
    }
    static UsageMeter GetUsage(gpucount_t)
    {
        return {};
    }
    static TransferStatus GetPcieTransfer(gpucount_t)
    {
        return {};
    }
};

bool LoadDefaultGpuQuery(GpuQueryInterface *loc)
{
    using namespace Library;

    Vector<cstring> libs = {
        "coffee-nvidia",
        "coffee-amd",
        "coffee-intel"
    };
    CString error;
    for(cstring mf : libs)
    {
        auto lib = FunctionLoader::GetLibrary(
                    mf, FunctionLoader::GlobalSymbols, nullptr, &error);
        if(!lib)
        {
            cVerbose(6, "Library loading error: {0}", error);
            continue;
        }
        auto ldr = ObjectLoader::
                GetConstructor<GpuQueryFunction>(lib, "GetGpuQuery", &error);
        if(!ldr.loader)
        {
            cVerbose(6, "Symbol resolution error: {0}", error);
            FunctionLoader::UnloadLibrary(lib);
            continue;
        }
        GpuQueryFunction* fun = ldr.loader();
        if(!fun)
        {
            cVerbose(6, "Failed to initialize {0}", mf);
            FunctionLoader::UnloadLibrary(lib);
            continue;
        }
        *loc = fun->ptr();
        return true;
    }
    *loc = GetDefault();
    return false;
}

GpuQueryInterface GetDefault()
{
    using namespace Coffee::GpuInfo;

    return {
        _GpuQueryDef::GetDriver,
                _GpuQueryDef::GetNumGpus,
                _GpuQueryDef::GpuModel,
                _GpuQueryDef::MemoryInfo,
                _GpuQueryDef::ProcMemoryUse,
                _GpuQueryDef::GetTemperature,
                _GpuQueryDef::GetClock,
                _GpuQueryDef::GetPowerMode,
                _GpuQueryDef::GetUsage,
                _GpuQueryDef::GetPcieTransfer
    };
}

}
}

GpuQueryFunction *GetGpuQueryDefault()
{
    return new GpuQueryFunction { Coffee::GpuInfo::GetDefault };
}
