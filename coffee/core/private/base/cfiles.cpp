#include <coffee/core/CFiles>

#include <coffee/core/plat/environment/environment_details.h>
#include <coffee/core/plat/plat_file.h>
#include <coffee/core/CDebug>

#include "../plat/file/file_abstraction.h"

namespace Coffee{
namespace CResources{

static CString _coffee_resource_prefix = "./";

void FileResourcePrefix(cstring prefix)
{
    _coffee_resource_prefix = prefix;
}

CString DereferencePath(cstring suffix, ResourceAccess storageMask)
{
    //We will NOT try to add any '/' in there.
    if(feval(storageMask&ResourceAccess::StorageSpecifier))
    {
        if(feval(storageMask&ResourceAccess::ConfigFile))
        {
            CString cfgDir = Env::GetUserData(nullptr,nullptr);
            return Env::ConcatPath(cfgDir.c_str(),suffix);
        }
#ifdef COFFEE_ANDROID
        else if(feval(storageMask&ResourceAccess::AssetFile))
            return CString(AssetApi::AssetPrefix)+suffix;
#endif
    }
    return _coffee_resource_prefix+suffix;
}

bool FileExists(const Resource &resc)
{
    return FileFun::Exists(resc.resource());
}

bool FileMap(Resource &resc, ResourceAccess acc)
{
    CString native_fn = FileFun::NativePath(resc.resource());
    resc.size = FileFun::Size(native_fn.c_str());

    if(resc.size == 0)
        return false;

    int err = 0;
    resc.m_mapping = FileFun::Map(
				native_fn.c_str(),
                acc,
                0,resc.size,
                &err);

    if(!resc.m_mapping.ptr)
    {
        /* Externalize error checkers */
#ifndef COFFEE_WINDOWS
		CString error = strerror(err);
#else
		LPSTR msgBuf = nullptr;
		size_t size = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&msgBuf,0,nullptr);
		CString error(msgBuf, size);
		LocalFree(msgBuf);
#endif
        cWarning("Failed to map file {2}:{0}: {1}",err,error,resc.resource());
        resc.size = 0;
        return false;
    }

    resc.data = resc.m_mapping.ptr;
    resc.flags = resc.flags|Resource::Mapped;

    return true;
}

bool FileUnmap(Resource &resc)
{
    if(!(resc.flags&Resource::Mapped))
        return false;

    bool s = FileFun::Unmap(&resc.m_mapping);

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
    FileFun::FileHandle *fp = FileFun::Open(
                native_fn.c_str(),
                (append) ?
                    ResourceAccess::Append|ResourceAccess::WriteOnly|acc
                  : ResourceAccess::WriteOnly|acc);
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

Resource::Resource(cstring rsrc, bool absolute, ResourceAccess acc):
    m_resource(),
    data(nullptr),
    size(0),
    flags(Undefined)
{
#ifdef COFFEE_ANDROID
    absolute = false;
#endif

    if(absolute)
        m_resource = rsrc;
    else
        m_resource = DereferencePath(rsrc,acc&ResourceAccess::StorageMask);
}

cstring Resource::resource() const
{
    return m_resource.c_str();
}

}
}
