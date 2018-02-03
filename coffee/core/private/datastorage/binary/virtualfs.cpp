#include <coffee/core/datastorage/binary/virtualfs.h>
#include <coffee/core/base/files/url.h>

namespace Coffee{
namespace VirtFS{

Resource::Resource(const VFS *base, const Url &url):
    filesystem(base),
    file(VFS::GetFile(base, url.internUrl.c_str()))
{
}

Bytes Resource::data() const
{
    if(!file)
        return {};

    return VFS::GetData(filesystem, file);
}

}
}
