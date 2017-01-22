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

#if defined(__EMSCRIPTEN__)
void emscripten_callback_load(void* arg)
{
    int* status = C_FCAST<int*>(arg);
    cDebug("Loaded file");
    *status = 1;
}
void emscripten_callback_store(void* arg)
{
    int* status = C_FCAST<int*>(arg);
    cDebug("Stored file");
    *status = 1;
}
void emscripten_callback_error(void* arg)
{
    int* status = C_FCAST<int*>(arg);
    cDebug("Failed to store file :(");
    *status = -1;
}
#endif

szptr RestoreMemory(c_ptr data_ptr, szptr *data_size, uint16 slot)
{
#if defined(__EMSCRIPTEN__)
    CString save_file = cStringFormat("Coffee_Data_{0}.dat", slot);
    int status = 0;
    emscripten_idb_async_store("Coffee" ,save_file.c_str(), data_ptr, data_size,
                               &status,
                               emscripten_callback_store,
                               emscripten_callback_error);

    while(status == 0);

    return data_size;
#else
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
#endif
}

szptr SaveMemory(c_cptr data_ptr, szptr data_size, uint16 slot)
{
#if defined(__EMSCRIPTEN__)
    CString save_file = cStringFormat("Coffee_Data_{0}.dat", slot);

    int status = 0;

    emscripten_idb_async_store("Coffee" ,save_file.c_str(), data_ptr, data_size,
                               &status,
                               emscripten_callback_store,
                               emscripten_callback_error);

    /* Await data, we're lazy, maybe do some event handling externally? */
    while(status == 0);

    return data_size;
#else
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
#endif
}

}
}
