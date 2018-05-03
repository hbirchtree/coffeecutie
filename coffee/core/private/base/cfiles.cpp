#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/plat/plat_environment.h>

#define CFILES_TAG "File::"

namespace Coffee {
namespace CResources {

struct Resource::ResourceData
{
    FileFun::FileMapping m_mapping;
    FileFun::FileHandle* m_handle;
    Url                  m_url;
};

Resource::Resource(cstring rsrc, bool absolute, ResourceAccess acc) :
    Resource(MkUrl(rsrc, acc & ResourceAccess::StorageMask))
{
}

Resource::Resource(cstring rsrc, ResourceAccess acc) :
    Resource(rsrc, false, acc)
{
}

Resource::Resource(const Url& url) :
    m_resource(*url), m_platform_data(new ResourceData), data(nullptr), size(0),
    flags(Undefined)
{
    m_platform_data->m_url = url;

    Profiler::DeepProfile(CFILES_TAG "Resource created");
}

Resource::Resource(Resource&& rsc)
{
    this->data            = rsc.data;
    this->size            = rsc.size;
    this->flags           = rsc.flags;
    this->m_platform_data = rsc.m_platform_data;
    this->m_resource      = std::move(rsc.m_resource);

    rsc.data            = nullptr;
    rsc.flags           = FileFlags::Undefined;
    rsc.m_platform_data = nullptr;
    rsc.size            = 0;
}

Resource::~Resource()
{
    if(flags & Mapped)
        FileUnmap(*this);
    if(flags & FileIO)
        FileFree(*this);

    if(m_platform_data)
        delete m_platform_data;
}

cstring Resource::resource() const
{
    return m_resource.c_str();
}

bool Resource::valid() const
{
    return !m_resource.empty();
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
    return FileFun::Exists(resc.m_platform_data->m_url);
}

bool FileMap(Resource& resc, ResourceAccess acc, szptr size)
{
    if(resc.flags & Resource::Mapped)
    {
        Profiler::DeepProfile(CFILES_TAG "File already mapped");
        return true;
    }

    Profiler::DeepPushContext(CFILES_TAG "File mapping");

    resc.size = FileFun::Size(resc.m_platform_data->m_url);
    resc.size = CMath::max(resc.size, size);

    if(resc.size == 0)
    {
        Profiler::DeepProfile(CFILES_TAG "File not found");
        Profiler::DeepPopContext();
        return false;
    }

    int err = 0;
    resc.m_platform_data->m_mapping =
        FileFun::Map(resc.m_platform_data->m_url, acc, 0, resc.size, &err);

    if(!resc.m_platform_data->m_mapping.ptr)
    {
    /* Externalize error checkers */
#ifndef COFFEE_WINDOWS
        CString error = strerror(err);
#else
        CString error = win_strerror(err);
#endif
        cWarning(
            CFILES_TAG "Failed to map file {2}:{0}: {1}",
            err,
            error,
            resc.resource());
        resc.size = 0;
        Profiler::DeepProfile(CFILES_TAG "Mapping failed");
        Profiler::DeepPopContext();
        return false;
    }

    resc.data  = resc.m_platform_data->m_mapping.ptr;
    resc.flags = resc.flags | Resource::Mapped;

    Profiler::DeepProfile(CFILES_TAG "File mapped");

    Profiler::DeepPopContext();

    return true;
}

bool FileUnmap(Resource& resc)
{
    Profiler::DeepPushContext(CFILES_TAG "File mapping");
    if(!(resc.flags & Resource::Mapped))
    {
        Profiler::DeepProfile(CFILES_TAG "Non-mapped file called for unmap");
        Profiler::DeepPopContext();
        return false;
    }

    bool s = FileFun::Unmap(&resc.m_platform_data->m_mapping);

    if(!s)
    {
        Profiler::DeepProfile(CFILES_TAG "Unmapping failed");
        Profiler::DeepPopContext();
        return false;
    }

    resc.data = nullptr;
    resc.size = 0;

    resc.flags ^= Resource::Mapped;

    Profiler::DeepProfile(CFILES_TAG "File unmapped");
    Profiler::DeepPopContext();

    return s;
}

bool FileOpenMap(Resource& resc, szptr size, RSCA acc)
{
    FileFun::Truncate(resc.m_platform_data->m_url, size);

    int error = 0;
    resc.m_platform_data->m_mapping =
        FileFun::Map(resc.m_platform_data->m_url, acc, 0, size, &error);

    if(!resc.m_platform_data->m_mapping.ptr)
    {
        cWarning(CFILES_TAG "Failed to map file {0}",
                 resc.resource());
        return false;
    }

    resc.data = resc.m_platform_data->m_mapping.ptr;
    resc.size = resc.m_platform_data->m_mapping.size;

    resc.flags |= Resource::Mapped;

    return true;
}

void FileFree(Resource& resc)
{
    if(!(resc.flags & Resource::FileIO))
        return;

    CFree(resc.data);
    resc.data = nullptr;
    resc.size = 0;

    Profiler::DeepProfile(CFILES_TAG "File buffer free'd");
}

bool FilePull(Resource& resc, bool textmode, bool)
{
    DProfContext         a(CFILES_TAG "File reading");
    FileFun::FileHandle* fp =
        FileFun::Open(resc.m_platform_data->m_url, ResourceAccess::ReadOnly);

    if(!fp)
    {
        cWarning(CFILES_TAG "Failed to read file: {0}", resc.resource());
        Profiler::DeepProfile(CFILES_TAG "File not found");
        return false;
    }

    Bytes data = FileFun::Read(fp, -1, textmode);
    data.disown(); /* Relinquish ownership of data */
    resc.data      = data.data;
    resc.size      = data.size;
    if(!FileFun::Close(fp))
        cWarning(CFILES_TAG "Failed to close file: {0}", resc.resource());

    if(!resc.data)
    {
        Profiler::DeepProfile(CFILES_TAG "File read failure");
        return false;
    }

    resc.flags = resc.flags | Resource::FileIO;

    Profiler::DeepProfile(CFILES_TAG "File read");

    return true;
}

bool FileCommit(Resource& resc, bool append, ResourceAccess acc)
{
    DProfContext a(CFILES_TAG "File write");

    ResourceAccess dflags = ResourceAccess::WriteOnly;

    dflags |= ResourceAccess::NewFile;

    FileFun::FileHandle* fp = FileFun::Open(
        resc.m_platform_data->m_url,
        (append) ? ResourceAccess::Append | dflags | acc : dflags | acc);

    if(!fp)
    {
        Profiler::DeepProfile(CFILES_TAG "File not created");
        return false;
    }

    bool stat = FileFun::Write(fp, resc, false);

    if(!FileFun::Close(fp))
    {
        Profiler::DeepProfile(CFILES_TAG "File failed to close");
        cWarning(CFILES_TAG "Failed to close file: {0}", resc.resource());
    }

    return stat;
}

bool FileMkdir(Url const& dirname, bool recursive)
{
    Profiler::DeepProfile(CFILES_TAG "Directory creation");
    bool status = DirFun::MkDir(dirname, recursive);

    if(!status)
        Profiler::DeepProfile(CFILES_TAG "Directory creation failed");

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
