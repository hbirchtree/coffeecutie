#include <coffee/core/CFiles>

#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/plat/plat_file.h>
#include <coffee/core/CDebug>

#include <coffee/core/base/files/url.h>

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
//    CString native_fn = FileFun::NativePath(resc.resource());
//    cVerbose(6,"Native file path: {0}->{1}",resc.resource(),native_fn);

    resc.size = FileFun::Size(resc.m_platform_data->m_url);
    resc.size = CMath::max(resc.size, size);

    if(resc.size == 0)
        return false;

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
        return false;
    }

    resc.data = resc.m_platform_data->m_mapping.ptr;
    resc.flags = resc.flags|Resource::Mapped;

    return true;
}

bool FileUnmap(Resource &resc)
{
    if(!(resc.flags&Resource::Mapped))
        return false;

    bool s = FileFun::Unmap(&resc.m_platform_data->m_mapping);

    if(!s)
        return false;

    resc.data = nullptr;
    resc.size = 0;

    resc.flags ^= Resource::Mapped;

    return s;
}

void FileFree(Resource &resc)
{
    if(!(resc.flags&Resource::FileIO))
        return;

    CFree(resc.data);
    resc.data = nullptr;
    resc.size = 0;
}

bool FilePull(Resource &resc, bool textmode, bool)
{
    FileFun::FileHandle *fp =
            FileFun::Open(resc.m_platform_data->m_url,
                          ResourceAccess::ReadOnly);

    if(!fp){
        cWarning("Failed to read file: {0}",resc.resource());
        return false;
    }

    CByteData data = FileFun::Read(fp,-1,textmode);
    resc.data = data.data;
    resc.size = data.size;
    if(!FileFun::Close(fp))
        cWarning("Failed to close file: {0}",resc.resource());

    if(!resc.data)
        return false;

    resc.flags = resc.flags|Resource::FileIO;

    return true;
}

bool FileCommit(Resource &resc, bool append, ResourceAccess acc)
{
    cVerbose(7,"Entered FileCommit");
//    CString native_fn = FileFun::NativePath(resc.resource());
//    cVerbose(7,"Got native path: {0}",native_fn);
    ResourceAccess dflags = ResourceAccess::WriteOnly;

//    if(!FileFun::Exists(native_fn.c_str()))
        dflags |= ResourceAccess::NewFile;

    FileFun::FileHandle *fp = FileFun::Open(
                resc.m_platform_data->m_url,
                (append) ?
                    ResourceAccess::Append|dflags|acc
                  : dflags|acc);
    cVerbose(7,"Got FH pointer: {0}",(c_cptr const&)fp);
	if (!fp)
        return false;
    CByteData d;
    d.data = (byte_t*)resc.data;
    d.size = resc.size;
    bool stat = FileFun::Write(fp,d,false);
    cVerbose(7,"Write operation result: {0}",stat);
    if(!FileFun::Close(fp))
        cWarning("Failed to close file: {0}",resc.resource());
    return stat;
}

bool FileMkdir(Url const& dirname, bool recursive)
{
    return DirFun::MkDir(dirname,recursive);
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
