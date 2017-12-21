#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/CDebug>

#define CFILES_TAG "File::"

namespace Coffee{
namespace CResources{

struct Resource::ResourceData
{
    FileFun::FileMapping m_mapping;
    FileFun::FileHandle* m_handle;
    Url m_url;
};

Resource::Resource(cstring rsrc, bool absolute, ResourceAccess acc):
    Resource(MkUrl(rsrc, acc & ResourceAccess::StorageMask))
{
}

Resource::Resource(cstring rsrc, ResourceAccess acc):
    Resource(rsrc,false,acc)
{
}

Resource::Resource(const Url &url):
    m_resource(*url),
    m_platform_data(new ResourceData),
    data(nullptr),
    size(0),
    flags(Undefined)
{
    m_platform_data->m_url = url;

    Profiler::DeepProfile(CFILES_TAG "Resource created");
}

Resource::Resource(Resource &&rsc)
{
    this->data = rsc.data;
    this->size = rsc.size;
    this->flags = rsc.flags;
    this->m_platform_data = rsc.m_platform_data;
    this->m_resource = std::move(rsc.m_resource);

    rsc.data = nullptr;
    rsc.flags = FileFlags::Undefined;
    rsc.m_platform_data = nullptr;
    rsc.size = 0;
}

Resource::~Resource()
{
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

bool FileExists(const Resource &resc)
{
    return FileFun::Exists(resc.m_platform_data->m_url);
}

bool FileMap(Resource &resc, ResourceAccess acc, szptr size)
{
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
    resc.m_platform_data->m_mapping = FileFun::Map(
                resc.m_platform_data->m_url,
                acc,
                0,resc.size,
                &err);

    if(!resc.m_platform_data->m_mapping.ptr)
    {
        /* Externalize error checkers */
#ifndef COFFEE_WINDOWS
		CString error = strerror(err);
#else
		CString error = win_strerror(err);
#endif
        cWarning("Failed to map file {2}:{0}: {1}",
                 err,error,resc.resource());
        resc.size = 0;
        Profiler::DeepProfile(CFILES_TAG "Mapping failed");
        Profiler::DeepPopContext();
        return false;
    }

    resc.data = resc.m_platform_data->m_mapping.ptr;
    resc.flags = resc.flags|Resource::Mapped;

    Profiler::DeepProfile("File mapped");

    Profiler::DeepPopContext();

    return true;
}

bool FileUnmap(Resource &resc)
{
    Profiler::DeepPushContext(CFILES_TAG "File mapping");
    if(!(resc.flags&Resource::Mapped))
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

void FileFree(Resource &resc)
{
    if(!(resc.flags&Resource::FileIO))
        return;

    CFree(resc.data);
    resc.data = nullptr;
    resc.size = 0;

    Profiler::DeepProfile(CFILES_TAG "File buffer free'd");
}

bool FilePull(Resource &resc, bool textmode, bool)
{
    Profiler::DeepPushContext(CFILES_TAG "File reading");
    FileFun::FileHandle *fp =
            FileFun::Open(resc.m_platform_data->m_url,
                          ResourceAccess::ReadOnly);

    if(!fp){
        cWarning("Failed to read file: {0}",resc.resource());
        Profiler::DeepProfile(CFILES_TAG "File not found");
        Profiler::DeepPopContext();
        return false;
    }

    CByteData data = FileFun::Read(fp,-1,textmode);
    resc.data = data.data;
    resc.size = data.size;
    if(!FileFun::Close(fp))
        cWarning("Failed to close file: {0}",resc.resource());

    if(!resc.data)
    {
        Profiler::DeepProfile(CFILES_TAG "File read failure");
        Profiler::DeepPopContext();
        return false;
    }

    resc.flags = resc.flags|Resource::FileIO;

    Profiler::DeepProfile(CFILES_TAG "File read");

    Profiler::DeepPopContext();

    return true;
}

bool FileCommit(Resource &resc, bool append, ResourceAccess acc)
{
    Profiler::DeepPushContext("File write");

    ResourceAccess dflags = ResourceAccess::WriteOnly;

    dflags |= ResourceAccess::NewFile;

    FileFun::FileHandle *fp = FileFun::Open(
                resc.m_platform_data->m_url,
                (append) ?
                    ResourceAccess::Append|dflags|acc
                  : dflags|acc);

	if (!fp)
    {
        Profiler::DeepProfile(CFILES_TAG "File not created");
        Profiler::DeepPopContext();
        return false;
    }

    CByteData d;
    d.data = (byte_t*)resc.data;
    d.size = resc.size;
    bool stat = FileFun::Write(fp,d,false);

    if(!FileFun::Close(fp))
    {
        Profiler::DeepProfile(CFILES_TAG "File failed to close");
        cWarning("Failed to close file: {0}",resc.resource());
    }

    Profiler::DeepPopContext();

    return stat;
}

bool FileMkdir(Url const& dirname, bool recursive)
{
    Profiler::DeepProfile(CFILES_TAG "Directory creation");
    bool status = DirFun::MkDir(dirname,recursive);

    if(!status)
        Profiler::DeepProfile(CFILES_TAG "Directory creation failed");

    return status;
}

}

namespace Strings{
CString to_string(const CResources::Resource &r)
{
    return CString("rsc(") +
            StrUtil::pointerify(r.data) +
            "+" +
            cast_pod(r.size) +
            ")";
}
}
}
