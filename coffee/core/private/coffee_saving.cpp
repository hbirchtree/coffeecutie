#include <coffee/core/coffee_saving.h>
#include <coffee/core/coffee.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>

namespace Coffee{
namespace Store{

i64 AvailableSaveMemory()
{
    return -1;
}

u16 AvailableSaveSlots()
{
    return 1;
}

#if defined(COFFEE_EMSCRIPTEN)
struct DataStatus
{
    int32 status;
    int32 __padding;
    c_ptr ptr;
    szptr* size;
};

void emscripten_callback_load(void* arg, void* data, int size)
{
    DataStatus* status = C_FCAST<DataStatus*>(arg);
    cDebug("Loaded file");
    if(size <= C_CAST<int>(*status->size))
        MemCpy(status->ptr, data, C_CAST<szptr>(size));
    status->status = 1;
}
void emscripten_callback_store(void* arg)
{
    int32* status = C_FCAST<int32*>(arg);
    cDebug("Stored file");
    *status = 1;
}
void emscripten_callback_error(void* arg)
{
    int32* status = C_FCAST<int32*>(arg);
    cDebug("Failed to do something with file :(");
    *status = -1;
}
#endif

szptr RestoreMemory(Bytes&& data, u16 slot)
{
    cVerbose(8, "Loading memory: {0}+{1} <- slot {2} ::",
             StrUtil::pointerify(data.data),
             data.size, slot);

#if defined(COFFEE_EMSCRIPTEN)
    CString save_file = cStringFormat("{0}_Slot-{1}.dat",
                                      ApplicationData().application_name,
                                      slot);

    static DataStatus data_status = {0, 0, data.data, data.size};
    emscripten_idb_async_load(ApplicationData().organization_name.c_str(),
                              save_file.c_str(),
                              &data_status,
                              emscripten_callback_load,
                              emscripten_callback_error);

    return data.size;
#else
    CString file_str = cStringFormat("CoffeeData.{0}.bin", slot);
    cVerbose(8, "Pre-creating resource");
    CResources::Resource rsc(file_str.c_str(),
                             ResourceAccess::SpecifyStorage
                             | ResourceAccess::ConfigFile);

    cVerbose(8, "Created resource");

    if(!CResources::FilePull(rsc, false, false) && rsc.size <= data.size)
    {
        data.size = 0;
        CResources::FileFree(rsc);
        return data.size;
    }

    cVerbose(8, "Pulled resource");

    if(!data.data || !rsc.data || rsc.size < 1 || rsc.size >= data.size)
    {
        CResources::FileFree(rsc);
        return rsc.size;
    }

    cVerbose(8, "Validated resource, got {0}+{1}",
             StrUtil::pointerify(rsc.data), rsc.size);

    MemCpy(C_OCAST<Bytes>(rsc), data);
    cVerbose(8, "Copied data from mapping to memory");
    CResources::FileFree(rsc);

    cVerbose(8, "Memory load succeeded, got {0} bytes", data.size);

    return rsc.size;
#endif
}

szptr SaveMemory(Bytes const& data, uint16 slot)
{
#if defined(COFFEE_EMSCRIPTEN)
    CString save_file = cStringFormat("{0}_Slot-{1}.dat",
                                      ApplicationData().application_name,
                                      slot);

    static int32 status = 0;

    emscripten_idb_async_store(ApplicationData().organization_name.c_str(),
                               save_file.c_str(),
                               C_FCAST<void*>(data.data), data.size,
                               &status,
                               emscripten_callback_store,
                               emscripten_callback_error);

    return data_size;
#else
    CString file_str = cStringFormat("CoffeeData.{0}.bin", slot);
    CResources::Resource rsc(file_str.c_str(),
                             ResourceAccess::SpecifyStorage
                             | ResourceAccess::ConfigFile
                             | ResourceAccess::NewFile);

    /* I promise not to overwrite your data... */
    rsc = data;

    if(!CResources::FileCommit(rsc, false,
                           ResourceAccess::WriteOnly
                           | ResourceAccess::Discard))
        return 0;

    return rsc.size;
#endif
}

}
}
