#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/string_casting.h>

#define CFILES_TAG "File::"

namespace Coffee {
namespace CResources {

struct Resource::ResourceData
{
    FileFun::FileMapping       m_mapping;
    UqPtr<FileFun::FileHandle> m_handle;
    Bytes                      m_resourceBuffer;
    Url                        m_url;
};

void Resource::RscData_deleter::operator()(Resource::ResourceData* data)
{
    delete data;
}

Resource::Resource(cstring rsrc, RSCA acc) :
    Resource(MkUrl(rsrc, acc & RSCA::StorageMask))
{
}

Resource::Resource(const Url& url) :
    m_resource(*url), m_platform_data(MkUqDST<ResourceData, RscData_deleter>()),
    data(nullptr), size(0), flags(Undefined)
{
    m_platform_data->m_url = url;

    Profiler::DeepProfile(CFILES_TAG "Resource created");
}

Resource::Resource(Resource&& rsc)
{
    this->data            = rsc.data;
    this->size            = rsc.size;
    this->flags           = rsc.flags;
    this->m_platform_data = std::move(rsc.m_platform_data);
    this->m_resource      = std::move(rsc.m_resource);

    rsc.data            = nullptr;
    rsc.flags           = FileFlags::Undefined;
    rsc.m_platform_data = nullptr;
    rsc.size            = 0;
}

Resource::~Resource()
{
}

cstring Resource::resource() const
{
    return m_resource.c_str();
}

bool Resource::valid() const
{
    return !m_resource.empty();
}

Resource& Resource::operator=(Bytes&& data)
{
    Bytes& internal_store = this->m_platform_data->m_resourceBuffer;

    internal_store = std::move(data);
    this->data     = internal_store.data;
    this->size     = internal_store.size;
    return *this;
}

Resource::operator Path() const
{
    return Path(m_resource);
}

Resource::operator Bytes()
{
    if(data && size)
        return Bytes::From(data, size);

    if(flags == Undefined && FileMap(*this, RSCA::ReadOnly))
        return FileGetDescriptor(*this);
    else
        return Bytes();
}

bool FileExists(const Resource& resc)
{
    file_error ec;
    return FileFun::Exists(resc.m_platform_data->m_url, ec);
}

bool FileMap(Resource& resc, RSCA acc, szptr size)
{
    file_error ec;

    if(resc.flags & Resource::Mapped)
    {
        Profiler::DeepProfile(CFILES_TAG "File already mapped");
        return true;
    }

    Profiler::DeepPushContext(CFILES_TAG "File mapping");

    resc.size = FileFun::Size(resc.m_platform_data->m_url, ec);
    resc.size = CMath::max(resc.size, size);

    if(resc.size == 0)
    {
        C_ERROR_CHECK(ec);
        Profiler::DeepProfile(CFILES_TAG "File not found");
        Profiler::DeepPopContext();
        return false;
    }

    resc.m_platform_data->m_mapping =
        FileFun::Map(resc.m_platform_data->m_url, acc, 0, resc.size, ec);

    if(ec)
    {
        C_ERROR_CHECK(ec);
        resc.size = 0;
        Profiler::DeepProfile(CFILES_TAG "Mapping failed");
        Profiler::DeepPopContext();
        return false;
    }

    resc.data  = resc.m_platform_data->m_mapping.data;
    resc.flags = resc.flags | Resource::Mapped;

    Profiler::DeepProfile(CFILES_TAG "File mapped");

    Profiler::DeepPopContext();

    return true;
}

bool FileUnmap(Resource& resc)
{
    file_error ec;

    Profiler::DeepPushContext(CFILES_TAG "File mapping");
    if(!(resc.flags & Resource::Mapped))
    {
        Profiler::DeepProfile(CFILES_TAG "Non-mapped file called for unmap");
        Profiler::DeepPopContext();
        return false;
    }

    bool s = FileFun::Unmap(std::move(resc.m_platform_data->m_mapping), ec);

    if(!s)
    {
        C_ERROR_CHECK(ec);
        Profiler::DeepProfile(CFILES_TAG "Unmapping failed");
        Profiler::DeepPopContext();
        return false;
    }

    resc.m_platform_data->m_mapping = {};
    resc.data = nullptr;
    resc.size = 0;

    resc.flags ^= Resource::Mapped;

    Profiler::DeepProfile(CFILES_TAG "File unmapped");
    Profiler::DeepPopContext();

    return s;
}

bool FileOpenMap(Resource& resc, szptr size, RSCA acc)
{
    file_error ec;

    FileFun::Truncate(resc.m_platform_data->m_url, size, ec);

    resc.m_platform_data->m_mapping =
        FileFun::Map(resc.m_platform_data->m_url, acc, 0, size, ec);

    if(ec)
        return false;

    resc.data = resc.m_platform_data->m_mapping.data;
    resc.size = resc.m_platform_data->m_mapping.size;

    resc.flags |= Resource::Mapped;

    return true;
}

void FileFree(Resource& resc)
{
    if(!(resc.flags & Resource::FileIO))
        return;

    Bytes local = std::move(resc.m_platform_data->m_resourceBuffer);

    resc.m_platform_data->m_resourceBuffer = {};

    Profiler::DeepProfile(CFILES_TAG "File buffer free'd");
}

bool FilePull(Resource& resc)
{
    DProfContext a(CFILES_TAG "File reading");

    file_error ec;

    auto fp = FileFun::Open(resc.m_platform_data->m_url, RSCA::ReadOnly, ec);

    if(ec)
    {
        C_ERROR_CHECK(ec);
        Profiler::DeepProfile(CFILES_TAG "File not found");
        return false;
    }

    auto& data = resc.m_platform_data->m_resourceBuffer;
    data       = FileFun::Read(fp, -1, ec);
    resc.data  = data.data;
    resc.size  = data.size;

    if(!FileFun::Close(std::move(fp), ec) || ec)
        return false;

    if(!resc.data)
    {
        C_ERROR_CHECK(ec);
        Profiler::DeepProfile(CFILES_TAG "File read failure");
        return false;
    }

    resc.flags = resc.flags | Resource::FileIO;

    Profiler::DeepProfile(CFILES_TAG "File read");

    return true;
}

bool FileCommit(Resource& resc, RSCA acc)
{
    DProfContext a(CFILES_TAG "File write");

    file_error ec;

    auto fp = FileFun::Open(resc.m_platform_data->m_url, acc, ec);

    if(ec)
    {
        C_ERROR_CHECK(ec);
        Profiler::DeepProfile(CFILES_TAG "File not created");
        return false;
    }

    bool stat = FileFun::Write(fp, resc, ec);

    if(!FileFun::Close(std::move(fp), ec))
    {
        C_ERROR_CHECK(ec);
        Profiler::DeepProfile(CFILES_TAG "File failed to close");
        return false;
    }

    return stat;
}

bool FileMkdir(Url const& dirname, bool recursive)
{
    Profiler::DeepProfile(CFILES_TAG "Directory creation");
    file_error ec;

    bool status = DirFun::MkDir(dirname, recursive, ec);

    if(!status)
    {
        C_ERROR_CHECK(ec);
        Profiler::DeepProfile(CFILES_TAG "Directory creation failed");
    }

    return status;
}

} // namespace CResources

namespace Strings {
CString to_string(const CResources::Resource& r)
{
    return CString("rsc(") + StrUtil::pointerify(r.data) + "+" +
           cast_pod(r.size) + ")";
}
} // namespace Strings
} // namespace Coffee
