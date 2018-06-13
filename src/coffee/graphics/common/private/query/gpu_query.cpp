#include <coffee/graphics/common/query/gpu_query.h>
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

bool LoadDefaultGpuQuery(GpuQueryInterface& loc, gpu_query_error &ec)
{
    using namespace Library;

    Vector<cstring> libs = {
        "CoffeeNVIDIAQuery",
        "CoffeeAMDQuery",
        "CoffeeIntelQuery",
        "CoffeeLibDRMQuery",
        "CoffeeOpenGLQuery"
    };
    for(cstring mf : libs)
    {
        CString error;
        auto lib = FunctionLoader::GetLibrary(
                    mf, FunctionLoader::GlobalSymbols, nullptr, &error);
        if(!lib)
        {
            ec = error;
            ec = GpuQueryError::LibraryLoading;

            continue;
        }
        auto ldr = ObjectLoader::
                GetConstructor<GpuQueryFunction>(lib, "GetGpuQuery", &error);
        if(!ldr.loader)
        {
            ec = error;
            ec = GpuQueryError::SymbolResolution;

            FunctionLoader::UnloadLibrary(lib);
            continue;
        }
        GpuQueryFunction* fun = ldr.loader();
        if(!fun)
        {
            ec = mf;
            ec = GpuQueryError::Initialization;

            FunctionLoader::UnloadLibrary(lib);
            continue;
        }

        if(!fun->ptr)
        {
            ec = mf;
            ec = GpuQueryError::LibraryImplementation;
            continue;
        }

        auto loader = fun->ptr();
        loc = loader;

        ec = C_CAST<GpuQueryError>(0);
        ec = "";

        return true;
    }
    loc = GetDefault();
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

const char *gpu_query_category::name() const noexcept
{
    return "GpuQuery";
}

std::string gpu_query_category::message(int error_code) const
{
    using GQE = GpuQueryError;

    switch(C_CAST<GQE>(error_code))
    {
    case GQE::LibraryLoading:
        return "Library loading";
    case GQE::SymbolResolution:
        return "Symbol resolution";
    case GQE::Initialization:
        return "Initialization error";
    case GQE::InternalError:
        return "Internal error";
    case GQE::DependencyError:
        return "Unmet dependencies";
    case GQE::ArgumentError:
        return "Called with bad parameters";
    case GQE::LibraryImplementation:
        return "Library implementation";
    }
}

}

GpuQueryFunction *GetGpuQueryDefault()
{
    return new GpuQueryFunction { Coffee::GpuInfo::GetDefault };
}
