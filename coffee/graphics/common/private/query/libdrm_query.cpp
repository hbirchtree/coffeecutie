#include <coffee/graphics/common/query/gpu_query.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/core/CFiles>
#include <coffee/core/string_casting.h>

#include <libdrm/drm.h>

#include <sys/fcntl.h>
#include <sys/ioctl.h>

extern "C" {

uint64_t get_nouveau_property(int fd);

}

namespace Coffee{
namespace GpuInfo{

SWVersionInfo GetDriver()
{
    drm_version_t ver = {};
    int fd = open("/dev/dri/card0", 0);
    ioctl(fd, DRM_IOCTL_VERSION, &ver);

    return SWVersionInfo("DRM", 1, 0);
}

int device(gpucount_t i)
{
    CString dev_name = "/dev/dri/card" + cast_pod(i);
    return open(dev_name.c_str(), 0);
}

gpucount_t GetNumGpus()
{
    gpucount_t num = 0;

    DirFun::DirList files;
    DirFun::Ls("/dev/dri", files);
    for(DirFun::DirItem_t const& e : files)
        if(e.name.substr(0, 4) == "card")
            num++;
    return num;
}

HWDeviceInfo GetModel(gpucount_t i)
{
    int dev = device(i);

    drm_version_t ver = {};
    ioctl(dev, DRM_IOCTL_VERSION, &ver);

    CString name;
    name.resize(ver.name_len);
    ver.name = &name[0];
    CString desc;
    desc.resize(ver.desc_len);
    ver.desc = &desc[0];
    CString date;
    date.resize(ver.date_len);
    ver.date = &date[0];

    ioctl(dev, DRM_IOCTL_VERSION, &ver);

    get_nouveau_property(dev);

    return HWDeviceInfo(desc, "", name + " " + date);
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
    return new GpuQueryFunction{ Coffee::GpuInfo::GetDRM };
}
