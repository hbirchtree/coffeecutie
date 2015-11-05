#include "cfiles.h"

#include "coffee/core/plat/plat_file.h"

namespace Coffee{
namespace CResources{

CResourceUrl::CResourceUrl():
    flags(0),
    url(nullptr)
{
}

namespace CFiles{

}

bool coffee_file_exists(const CResource *resc)
{
    FILE *f = fopen(resc->resource(),"r");
    if(f)
        fclose(f);
    return f;
}

void coffee_file_open(CResource *resc)
{
}

void coffee_file_close(CResource *resc)
{

}

bool coffee_file_memmap(CResource *resc)
{
    resc->size = coffee_file_get_size(resc->resource());
    resc->data = CMemoryManagement::coffee_memory_map_file(
                resc->resource(),
                0,resc->size);
    if(!resc->data)
    {
        resc->size = 0;
        return false;
    }
    return true;
}

bool coffee_file_memunmap(CResource *resc)
{
    bool s = CMemoryManagement::coffee_memory_unmap_file(resc->data,resc->size);
    resc->data = nullptr;
    resc->size = 0;
    return s;
}

void coffee_file_free(CResource *resc)
{
    free(resc->data);
    resc->data = nullptr;
    resc->size = 0;
}

bool coffee_file_pull(CResource *resc, bool textmode)
{
    FILE *fp = CFiles::coffee_file_open(resc->resource(),"rb");

    if(!fp){
        cWarning("Failed to read file: %s",resc->resource());
        return false;
    }

    resc->data = CFiles::coffee_file_read(fp,resc->data,&resc->size,textmode);
    if(CFiles::coffee_file_close(fp))
        cWarning("Failed to close file: %s",resc->resource());
    return true;
}

bool coffee_file_commit(const CResource *resc, bool append)
{
    FILE *fp = CFiles::coffee_file_open(resc->resource(),(append) ? "ab+" : "wb");
    bool stat = CFiles::coffee_file_write(fp,resc->data,resc->size);
    if(CFiles::coffee_file_close(fp))
        cWarning("Failed to close file: %s",resc->resource());
    return stat;
}

CResource::CResource(cstring rsrc):
    flags(0),
    size(0),
    data(nullptr),
    m_resource(rsrc)
{
}

cstring CResource::resource() const
{
    return m_resource.c_str();
}

}
}
