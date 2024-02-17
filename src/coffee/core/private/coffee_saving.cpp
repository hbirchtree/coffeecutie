#include <coffee/core/coffee_saving.h>

#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/internal_state.h>

#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>

#include <coffee/core/CDebug>

namespace Coffee::Store {

SaveApi::~SaveApi()
{
}

FilesystemApi::FilesystemApi()
    : m_app(GetCurrentApp())
{
}

FilesystemApi::FilesystemApi(const AppData& app)
    : m_app(app)
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

struct DataStatus
{
    enum Status
    {
        Uninitialized,
        Waiting,
        Success,
        Failure,
    };

    semantic::Bytes     data;
    std::promise<szptr> output;
    Status              status{Uninitialized};
};

#if defined(COFFEE_EMSCRIPTEN)
// static void emscripten_callback_load(void* arg, void* data, int size)
//{
//     auto* status = C_RCAST<DataStatus*>(arg);
//     cDebug("Loaded file");
//     MemCpy(semantic::Bytes::ofsemantic::Bytes(data, size), status->data);
//     status->output.set_value(size);
//     status->status = DataStatus::Success;
// }
// static void emscripten_callback_store(void* arg)
//{
//     auto* status = C_RCAST<DataStatus*>(arg);
//     cDebug("Stored file");
//     status->output.set_value(status->data.size);
//     status->status = DataStatus::Success;
// }
// static void emscripten_callback_error(void* arg)
//{
//     auto* status = C_RCAST<DataStatus*>(arg);
//     cDebug("Failed to do something with file :(");
//     status->output.set_value(0);
//     status->status = DataStatus::Failure;
// }
#endif

// static std::string CreateSaveString(u16 slot)
//{
//     return Strings::cStringFormat(
//         "{0}-{1}.data", GetCurrentApp().application_name, slot);
// }

static Url CreateSaveUrl(u16 slot)
{
    return Path{"CoffeeData"}
        .addExtension(stl_types::cast_pod(slot))
        .addExtension("bin")
        .url(RSCA::ConfigFile);
}

std::future<szptr> FilesystemApi::restore(
    semantic::Bytes&& data, slot_count_t slot)
{
    std::promise<szptr> out;

    if(!data.data)
        return out.get_future();

#if defined(COFFEE_EMSCRIPTEN) && 0
    //    std::string save_file = CreateSaveString(slot);

    auto out_fut = out.get_future();
    //    DataStatus data_status
    //        = {std::move(data), std::move(out), DataStatus::Waiting};
    //    //    emscripten_idb_async_load(
    //    //        m_app.organization_name.c_str(),
    //    //        save_file.c_str(),
    //    //        &data_status,
    //    //        emscripten_callback_load,
    //    //        emscripten_callback_error);

    return out_fut;
#else
    Resource rsc(CreateSaveUrl(slot));

    if(!FileExists(rsc))
    {
        out.set_value(0);
        return out.get_future();
    }

    if(!FilePull(rsc) && !rsc.data_ro.empty())
    {
        out.set_value(0);
        return out.get_future();
    }

    if(rsc.data_ro.empty() || rsc.data_ro.size() > data.size)
    {
        out.set_value(0);
        return out.get_future();
    }

    //    semantic::chunk_ops::MemCpy(C_OCAST<semantic::Bytes>(rsc), data);

    out.set_value(rsc.data_ro.size());
    return out.get_future();
#endif
}

std::future<szptr> FilesystemApi::save(
    semantic::Bytes const& data, slot_count_t slot)
{
    std::promise<szptr> out;

#if defined(COFFEE_EMSCRIPTEN) && 0
    //    std::string    save_file   = CreateSaveString(slot);
    auto out_fut = out.get_future();
    //    DataStatus data_status = {*data.at(0), std::move(out),
    //    DataStatus::Waiting};

    //    //    emscripten_idb_async_store(
    //    //        m_app.organization_name.c_str(),
    //    //        save_file.c_str(),
    //    //        reinterpret_cast<void*>(data.data),
    //    //        data.size,
    //    //        &data_status,
    //    //        emscripten_callback_store,
    //    //        emscripten_callback_error);

    return out_fut;
#else
    Resource rsc(CreateSaveUrl(slot));

    /* I promise not to overwrite your data... */
    rsc = data;

    if(!FileCommit(rsc, RSCA::WriteOnly | RSCA::Discard | RSCA::NewFile))
        out.set_value(0);
    else
        out.set_value(rsc.data_ro.size());

    return out.get_future();
#endif
}

} // namespace Coffee::Store
