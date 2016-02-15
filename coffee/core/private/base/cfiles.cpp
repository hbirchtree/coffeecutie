#include <coffee/core/base/cfiles.h>

#include <cstring>

#include <coffee/core/plat/plat_file.h>
#include <coffee/core/plat/plat_core.h>
#include <coffee/core/base/cdebug.h>

#include <SDL2/SDL_system.h>

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

CResourceUrl::CResourceUrl():
    url(nullptr),
    flags(0)
{
}

namespace CFiles{

}

bool FileExists(const CResource &resc)
{
#if defined(COFFEE_C_FILE_API)
    FILE *f = fopen(resc.resource(),"r");
    if(f)
        fclose(f);
    return f;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

bool FileMap(CResource &resc, ResourceAccess acc)
{
#if defined(COFFEE_C_FILE_API)
    FileFun::FileHandle* h = FileFun::Open(resc.resource(),ResourceAccess::ReadOnly);
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
        cWarning("Failed to map file: {0}",strerror(err));
        resc.size = 0;
        return false;
    }
    return true;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

bool FileUnmap(CResource &resc)
{
#if defined(COFFEE_C_FILE_API)
    bool s = FileFun::Unmap(resc.data,resc.size);
    resc.data = nullptr;
    resc.size = 0;
    return s;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

void FileFree(CResource &resc)
{
    CFree(resc.data);
    resc.data = nullptr;
    resc.size = 0;
}

bool FilePull(CResource &resc, bool textmode, bool)
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

    return true;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

bool FileCommit(CResource &resc, bool append)
{
#if defined(COFFEE_C_FILE_API)
    FileFun::FileHandle *fp = FileFun::Open(resc.resource(),
                                            (append) ? ResourceAccess::Append|ResourceAccess::WriteOnly : ResourceAccess::WriteOnly);
    CByteData d;
    d.data = (byte_t*)resc.data;
    d.size = resc.size;
    if(!append)
        FileFun::Seek(fp,0);
    bool stat = FileFun::Write(fp,d,false);
    if(!FileFun::Close(fp))
        cWarning("Failed to close file: {0}",resc.resource());
    return stat;
#elif defined(COFFEE_ANDROID_FILE_ASSET_API)
    return false;
#endif
}

CResource::CResource(cstring rsrc, bool absolute):
    m_resource(),
    data(nullptr),
    size(0)
{
    if(absolute)
        m_resource = rsrc;
    else
        m_resource = coffee_file_get_dereferenced_path(rsrc);

}

cstring CResource::resource() const
{
    return m_resource.c_str();
}

}
}
