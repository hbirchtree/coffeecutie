#include <base/cfiles.h>

#include <cstring>
#include <byteswap.h>

#include <plat/plat_file.h>
#include <plat/plat_core.h>
#include <base/cdebug.h>

namespace Coffee{
namespace CResources{

static CString _coffee_resource_prefix = "./";

void FileResourcePrefix(cstring prefix)
{
    _coffee_resource_prefix = prefix;
}

CString coffee_file_get_dereferenced_path(cstring suffix)
{
    //We will NOT try to add any '/' in there.
    return _coffee_resource_prefix + suffix;
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
    FILE *f = fopen(resc.resource(),"r");
    if(f)
        fclose(f);
    return f;
}

bool FileMap(CResource &resc)
{
    resc.size = coffee_file_get_size(resc.resource());
    int err = 0;
    resc.data = CMemoryManagement::coffee_memory_map_file(
                resc.resource(),
                0,resc.size,
                &err);
    if(!resc.data)
    {
        cWarning("Failed to map file: {0}",strerror(err));
        resc.size = 0;
        return false;
    }
    return true;
}

bool FileUnmap(CResource &resc)
{
    bool s = CMemoryManagement::coffee_memory_unmap_file(resc.data,resc.size);
    resc.data = nullptr;
    resc.size = 0;
    return s;
}

void FileFree(CResource &resc)
{
    CFree(resc.data);
    resc.data = nullptr;
    resc.size = 0;
}

bool FilePull(CResource &resc, bool textmode, bool)
{
    CFiles::CFile *fp = CFiles::coffee_file_open(resc.resource(),"rb");

    if(!fp){
        cWarning("Failed to read file: %s",resc.resource());
        return false;
    }

    resc.data = CFiles::coffee_file_read(fp,resc.data,&resc.size,textmode);
    if(CFiles::coffee_file_close(fp))
        cWarning("Failed to close file: %s",resc.resource());

    return true;
}

bool FileCommit(CResource &resc, bool append)
{
    CFiles::CFile *fp = CFiles::coffee_file_open(resc.resource(),(append) ? "ab+" : "wb");
    bool stat = CFiles::coffee_file_write(fp,resc.data,resc.size);
    if(CFiles::coffee_file_close(fp))
        cWarning("Failed to close file: %s",resc.resource());
    return stat;
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

bool FileMkdir(cstring dirname, bool recursive)
{
    return CFiles::coffee_file_mkdir(dirname,recursive);
}

}
}
