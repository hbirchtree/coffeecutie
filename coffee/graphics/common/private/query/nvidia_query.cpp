#include <coffee/graphics/common/query/gpu_query.h>
#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <nvml.h>

namespace Coffee{
namespace GpuInfo{

SWVersionInfo GetDriver()
{
    CString driver_ver;
    driver_ver.resize(NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);

    nvmlSystemGetDriverVersion(&driver_ver[0], NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);

    driver_ver.resize(driver_ver.find('\0'));

    CString major = driver_ver.substr(0, driver_ver.find('.'));
    CString minor = driver_ver.substr(driver_ver.find('.')+1, driver_ver.length());

    return SWVersionInfo("NVIDIA",
                         Mem::Convert::strtouint(major.c_str()),
                         Mem::Convert::strtouint(minor.c_str()));
}

gpucount_t GetNumGpus()
{
    uint32 c = 0;
    nvmlDeviceGetCount(&c);
    return c;
}

GpuQueryInterface GetNvidia()
{
    return {
        GetDriver,
                GetNumGpus,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
    };
}

}
}

extern "C" GpuQueryFunction* GetGpuQuery();

GpuQueryFunction* GetGpuQuery()
{
    if(nvmlInit() == NVML_SUCCESS)
        return new GpuQueryFunction { Coffee::GpuInfo::GetNvidia };
    else
    {
//        fprintf(stderr, "NVML could not be initialized\n");
        return nullptr;
    }
}
