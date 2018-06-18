#include <coffee/graphics/common/query/gpu_query.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>
#include <coffee/core/string_casting.h>

#include <drm.h>
#include <xf86drm.h>

#include <sys/fcntl.h>
#include <sys/ioctl.h>

extern "C" {

//uint64_t get_nouveau_property(int fd);

}

namespace Coffee{
namespace GpuInfo{

int device(gpucount_t i)
{
    CString dev_name = "/dev/dri/card" + cast_pod(i);
    return open(dev_name.c_str(), 0);
}

SWVersionInfo GetDriver()
{
    return SWVersionInfo("DRM", 1, 0);
}

gpucount_t GetNumGpus()
{
    gpucount_t num = 0;

    DirFun::DirList files;
    DirFun::Ls(MkUrl("/dev/dri",
                     ResourceAccess::SpecifyStorage
                     |ResourceAccess::SystemFile),
               files);
    for(DirFun::DirItem_t const& e : files)
        if(e.name.substr(0, 4) == "card")
            num++;
    return num;
}

HWDeviceInfo GetModel(gpucount_t i)
{
    int dev = device(i);

    drmVersionPtr ver = drmGetVersion(dev);
    drmDevicePtr dev_ptr;
    drmGetDevice(dev, &dev_ptr);

    CString dev_id =
            cStringFormat(
                "{0}:{1}",
                StrUtil::pointerify(dev_ptr->deviceinfo.pci->vendor_id),
                StrUtil::pointerify(dev_ptr->deviceinfo.pci->device_id));

    auto h = HWDeviceInfo(ver->desc, dev_id, ver->name);
    drmFreeVersion(ver);
    drmFreeDevice(&dev_ptr);

    return h;
}

GpuQueryInterface GetDRM()
{
    return {
        GetDriver,
                GetNumGpus,
                GetModel,
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
    if(drmAvailable() && Coffee::GpuInfo::GetNumGpus())
        return new GpuQueryFunction{ Coffee::GpuInfo::GetDRM };

    return nullptr;
}
