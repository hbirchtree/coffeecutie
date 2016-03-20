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

CString coffee_file_get_dereferenced_path(cstring suffix)
{
#if defined(COFFEE_C_FILE_API)
    //We will NOT try to add any '/' in there.
    return _coffee_resource_prefix + suffix;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return "";
#endif
}

bool FileExists(const Resource &resc)
{
#if defined(COFFEE_C_FILE_API)
    return FileFun::Exists(resc.resource());
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

bool FileMap(Resource &resc, ResourceAccess acc)
{
#if defined(COFFEE_C_FILE_API)
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

    if(!resc.m_mapping)
    {
        cWarning("Failed to map file {1}: {0}",strerror(err),resc.resource());
        resc.size = 0;
        return false;
    }

    resc.data = resc.m_mapping->ptr;
    resc.flags = resc.flags|Resource::Mapped;

    return true;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

bool FileUnmap(Resource &resc)
{
#if defined(COFFEE_C_FILE_API)
    if(!(resc.flags&Resource::Mapped))
        return false;

    bool s = FileFun::Unmap(resc.m_mapping);
    resc.data = nullptr;
    resc.size = 0;

    resc.flags ^= Resource::Mapped;

    delete resc.m_mapping;

    return s;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
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
#if defined(COFFEE_C_FILE_API)
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
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

bool FileCommit(Resource &resc, bool append, ResourceAccess acc)
{
#if defined(COFFEE_C_FILE_API)
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
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

Resource::Resource(cstring rsrc, bool absolute):
    m_resource(),
    data(nullptr),
    size(0),
    flags(Undefined)
{
    if(absolute)
        m_resource = rsrc;
    else
        m_resource = coffee_file_get_dereferenced_path(rsrc);

}

cstring Resource::resource() const
{
    return m_resource.c_str();
}

}
}
