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
    FILE *f = fopen(resc.resource(),"r");
    bool ev = f;
    if(f)
        fclose(f);
    return ev;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

bool FileMap(Resource &resc, ResourceAccess acc)
{
#if defined(COFFEE_C_FILE_API)
    FileFun::FileHandle* h = FileFun::Open(resc.resource(),ResourceAccess::ReadOnly);

    if(!h)
        return false;

    resc.size = FileFun::Size(h);
    FileFun::Close(h);
    int err = 0;
    resc.data = FileFun::Map(
                resc.resource(),
                acc,
                0,resc.size,
                &err);
    if(!resc.data)
    {
        cWarning("Failed to map file {1}: {0}",strerror(err),resc.resource());
        resc.size = 0;
        return false;
    }

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

    bool s = FileFun::Unmap(resc.data,resc.size);
    resc.data = nullptr;
    resc.size = 0;

    resc.flags ^= Resource::Mapped;

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
    FileFun::FileHandle *fp = FileFun::Open(resc.resource(),ResourceAccess::ReadOnly);

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
    FileFun::FileHandle *fp = FileFun::Open(
                resc.resource(),
                (append) ?
                    ResourceAccess::Append|ResourceAccess::WriteOnly|acc
                  : ResourceAccess::WriteOnly|acc);
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
