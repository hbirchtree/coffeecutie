#include <coffee/core/CFiles>

#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/plat/plat_file.h>
#include <coffee/core/CDebug>

#include "../plat/file/file_abstraction.h"

namespace Coffee{
namespace CResources{

static CString _coffee_resource_prefix = "./";

CString DereferencePath(cstring suffix, ResourceAccess storageMask)
{
    //We will NOT try to add any '/' in there.
    if(feval(storageMask&ResourceAccess::SpecifyStorage))
    {
        CString asset_fn = CString(AssetApi::AssetPrefix)+suffix;

        if(feval(storageMask&ResourceAccess::ConfigFile))
        {
            CString cfgDir = Env::GetUserData(nullptr,nullptr);
            return Env::ConcatPath(cfgDir.c_str(),suffix);
        }
#if defined(COFFEE_ANDROID) || defined(COFFEE_APPLE) || defined(COFFEE_LINUX) || defined(COFFEE_WINDOWS)
        else if(feval(storageMask,ResourceAccess::AssetFile)
                && FileFun::VerifyAsset(asset_fn.c_str()))
            return asset_fn.c_str();
#endif
    }
    return _coffee_resource_prefix+suffix;
}

struct Resource::ResourceData
{
    FileFun::FileMapping m_mapping;
    FileFun::FileHandle* m_handle;
};

Resource::Resource(cstring rsrc, bool absolute, ResourceAccess acc):
    m_resource(),
    m_platform_data(new ResourceData),
    data(nullptr),
    size(0),
    flags(Undefined)
{
#ifdef COFFEE_ANDROID
    absolute = false;
#endif

    if(absolute)
        m_resource = rsrc;
    else{
        m_resource = DereferencePath(rsrc,acc&ResourceAccess::StorageMask);
    }
}

Resource::~Resource()
{
    delete m_platform_data;
}

cstring Resource::resource() const
{
    return m_resource.c_str();
}

void FileResourcePrefix(cstring prefix)
{
    _coffee_resource_prefix = prefix;
}

bool FileExists(const Resource &resc)
{
    CString native_fn = FileFun::NativePath(resc.resource());
    return FileFun::Exists(native_fn.c_str());
}

bool FileMap(Resource &resc, ResourceAccess acc)
{
    CString native_fn = FileFun::NativePath(resc.resource());
    resc.size = FileFun::Size(native_fn.c_str());

    if(resc.size == 0)
        return false;

    int err = 0;
    resc.m_platform_data->m_mapping = FileFun::Map(
				native_fn.c_str(),
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
        cWarning("Failed to map file {2}:{0}: {1}",err,error,resc.resource());
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
	CString native_fn = FileFun::NativePath(resc.resource());
    FileFun::FileHandle *fp = FileFun::Open(native_fn.c_str(),ResourceAccess::ReadOnly);

    if(!fp){
        cWarning("Failed to read file: {0}",resc.resource());
        return false;
    }

    CByteData data = FileFun::Read(fp,-1,textmode);
    resc.data = data.data;
    resc.size = data.size;
    if(!FileFun::Close(fp))
        cWarning("Failed to close file: {0}",resc.resource());

    resc.flags = resc.flags|Resource::FileIO;

    return true;
}

bool FileCommit(Resource &resc, bool append, ResourceAccess acc)
{
    CString native_fn = FileFun::NativePath(resc.resource());
    ResourceAccess dflags = ResourceAccess::WriteOnly;

//    if(!FileFun::Exists(native_fn.c_str()))
        dflags |= ResourceAccess::NewFile;

    FileFun::FileHandle *fp = FileFun::Open(
                native_fn.c_str(),
                (append) ?
                    ResourceAccess::Append|dflags|acc
                  : dflags|acc);
	if (!fp)
		return false;
    CByteData d;
    d.data = (byte_t*)resc.data;
    d.size = resc.size;
    bool stat = FileFun::Write(fp,d,false);
    if(!FileFun::Close(fp))
        cWarning("Failed to close file: {0}",resc.resource());
    return stat;
}

bool FileMkdir(cstring dirname, bool recursive)
{
    return DirFun::MkDir(dirname,recursive);
}

}
}
