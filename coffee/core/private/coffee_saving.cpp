#include <coffee/core/coffee_saving.h>
#include <coffee/core/coffee.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>

namespace Coffee{
namespace Store{

int64 AvailableSaveMemory()
{
    return -1;
}

uint16 AvailableSaveSlots()
{
    return 1;
}

szptr RestoreMemory(c_ptr data_ptr, szptr *data_size, uint16 slot)
{
    CString file_str = cStringFormat("CoffeeData.{0}.bin", slot);
    CResources::Resource rsc(file_str.c_str(),
                             ResourceAccess::SpecifyStorage
                             | ResourceAccess::ConfigFile);

    if(!CResources::FilePull(rsc, false, false) && rsc.size <= *data_size)
    {
        *data_size = 0;
        CResources::FileFree(rsc);
        return *data_size;
    }

    if(!data_ptr)
    {
        CResources::FileFree(rsc);
        return rsc.size;
    }

    MemCpy(data_ptr, rsc.data, rsc.size);
    CResources::FileFree(rsc);
    return rsc.size;
}

szptr SaveMemory(c_cptr data_ptr, szptr data_size, uint16 slot)
{
    CString file_str = cStringFormat("CoffeeData.{0}.bin", slot);
    CResources::Resource rsc(file_str.c_str(),
                             ResourceAccess::SpecifyStorage
                             | ResourceAccess::ConfigFile);

    /* I promise not to overwrite your data... */
    rsc.data = c_ptr(data_ptr);
    rsc.size = data_size;

    if(!CResources::FileCommit(rsc, false,
                           ResourceAccess::WriteOnly
                           | ResourceAccess::Discard))
        return 0;

    return rsc.size;
}

}
}
