#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/coffee_saving.h>
#include <coffee/core/internal_state.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>

namespace Coffee {
namespace Store {

SaveApi::~SaveApi()
{
}

FilesystemApi::FilesystemApi() : m_app(GetCurrentApp())
{
}

FilesystemApi::FilesystemApi(const AppData& app) : m_app(app)
{
}

FilesystemApi::~FilesystemApi()
{
}

u64 FilesystemApi::availableMemory()
{
    return static_cast<u64>(-1);
}

FilesystemApi::slot_count_t FilesystemApi::availableSlots()
{
    return 1;
}

#if defined(COFFEE_EMSCRIPTEN)
struct DataStatus
{
    int32  status;
    int32  __padding;
    c_ptr  ptr;
    szptr* size;
};

void emscripten_callback_load(void* arg, void* data, int size)
{
    DataStatus* status = C_FCAST<DataStatus*>(arg);
    cDebug("Loaded file");
    if(size <= C_CAST<int>(*status->size))
        MemCpy(Bytes::From(data, size), Bytes::From(status->ptr, size));
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

#if defined(COFFEE_EMSCRIPTEN)
static CString CreateSaveString(u16 slot)
{
    return cStringFormat(
        "{0}-{1}.data", GetCurrentApp().application_name, slot);
}
#else
static Url CreateSaveUrl(u16 slot)
{
    return platform::url::constructors::MkUrl(
        Path("CoffeeData").addExtension(cast_pod(slot)).addExtension("bin"),
        RSCA::ConfigFile);
}
#endif

szptr FilesystemApi::restore(Bytes&& data, slot_count_t slot)
{
    if(!data.data)
        return 0;

#if defined(COFFEE_EMSCRIPTEN)
    CString save_file = CreateSaveString(slot);

    static DataStatus data_status = {0, 0, data.data, &data.size};
    emscripten_idb_async_load(
        m_app.organization_name.c_str(),
        save_file.c_str(),
        &data_status,
        emscripten_callback_load,
        emscripten_callback_error);

    return data.size;
#else
    Resource rsc(CreateSaveUrl(slot));

    if(!FileExists(rsc))
        return 0;

    if(!FilePull(rsc) && rsc.size <= data.size)
    {
        data.size = 0;
        return data.size;
    }

    if(!rsc.data || rsc.size < 1 || rsc.size > data.size)
    {
        return rsc.size;
    }

    MemCpy(C_OCAST<Bytes>(rsc), data);

    return rsc.size;
#endif
}

szptr FilesystemApi::save(Bytes const& data, slot_count_t slot)
{
#if defined(COFFEE_EMSCRIPTEN)
    CString save_file = CreateSaveString(slot);

    static int32 status = 0;

    emscripten_idb_async_store(
        m_app.organization_name.c_str(),
        save_file.c_str(),
        C_FCAST<void*>(data.data),
        data.size,
        &status,
        emscripten_callback_store,
        emscripten_callback_error);

    return data.size;
#else
    Resource rsc(CreateSaveUrl(slot));

    /* I promise not to overwrite your data... */
    rsc = data;

    if(!FileCommit(rsc, RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile))
        return 0;

    return rsc.size;
#endif
}

} // namespace Store
} // namespace Coffee
