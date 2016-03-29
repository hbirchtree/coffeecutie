#include <coffee/core/CFiles>

#include <coffee/core/plat/plat_file.h>
#include <coffee/core/CDebug>

#if defined(COFFEE_ANDROID_FILE_ASSET_API)
#include <SDL2/SDL_system.h>
#endif

namespace Coffee{
namespace CResources{

static CString _coffee_resource_prefix = "./";

void FileResourcePrefix(cstring prefix)
{
    _coffee_resource_prefix = prefix;
}

CString DereferencePath(cstring suffix)
{
#if defined(COFFEE_C_FILE_API)
    //We will NOT try to add any '/' in there.
    return _coffee_resource_prefix + suffix;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return suffix;
#endif
}

bool FileExists(const Resource &resc)
{
    return FileFun::Exists(resc.resource());
}

bool FileMap(Resource &resc, ResourceAccess acc)
{
	CString native_fn = FileFun::NativePath(resc.resource());
    resc.size = FileFun::Size(native_fn.c_str());

    cDebug("File size: {0}",resc.size);

    if(resc.size == 0)
        return false;

    cDebug("Size test passed");

    int err = 0;
    resc.m_mapping = FileFun::Map(
				native_fn.c_str(),
                acc,
                0,resc.size,
                &err);

    cDebug("File mapping successful");

    if(!resc.m_mapping.ptr)
    {
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

    cDebug("Acquired file mapping");

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

Resource::Resource(cstring rsrc, bool absolute):
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
        m_resource = DereferencePath(rsrc);
}

cstring Resource::resource() const
{
    return m_resource.c_str();
}

}
}
