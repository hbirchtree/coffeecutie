#include <fmt/core.h>
#include <platforms/emscripten/fsio.h>
#include <platforms/emscripten/mmio.h>
#include <platforms/emscripten/rdwrio.h>

#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>

namespace platform::file::emscripten {

static void sync_storage(semantic::RSCA storage)
{
    if(enum_helpers::feval(storage, RSCA::ConfigFile))
        // clang-format off
        EM_ASM(
            FS.syncfs((err) => {
                if(err)
                    console.err("Error during syncfs: ", err);
            });
        );
    // clang-format on
}

static std::map<int, semantic::RSCA> fd_tracking{};

void track_file_open(int fd, semantic::RSCA access)
{
    fd_tracking[fd] = access;
}

void notify_file_close(int fd)
{
    if(auto it = fd_tracking.find(fd); it == fd_tracking.end())
        return;
    else
    {
        sync_storage(it->second);
        fd_tracking.erase(it);
    }
}

struct promise_data_t
{
    std::promise<posix::mem_mapping_t> promise{};

    std::string url;
    std::string hash;
};

static std::map<std::string, promise_data_t> mmap_async_data{};

static std::optional<posix::mem_mapping_t> mmap_download(const char* hash)
{
    auto tmpfile = url::constructors::MkUrl(hash);
    auto size    = posix::size(tmpfile);
    auto mapping =
        posix::map(tmpfile, posix::mapping_params_t{.size = size.value()});
    if(mapping.has_value())
        return mapping.value();
    else
        return std::nullopt;
}

std::future<posix::mem_mapping_t> mmap_async(Url const& file)
{
    auto hash = std::to_string(std::hash<std::string>()(file.internUrl));

    fprintf(
        stdout,
        "Starting async mmap of %s into %s\n",
        file.internUrl.c_str(),
        hash.c_str());

    if(posix::path::exists(url::constructors::MkUrl(hash)))
    {
        std::promise<posix::mem_mapping_t> out;
        auto                               fut = out.get_future();
        out.set_value(
            mmap_download(hash.c_str()).value_or(posix::mem_mapping_t{}));
        return fut;
    }

    auto& data = mmap_async_data[hash];
    data       = promise_data_t{
              .url  = file.internUrl,
              .hash = std::move(hash),
    };
    auto                    fut = data.promise.get_future();
    emscripten_fetch_attr_t attrs;
    emscripten_fetch_attr_init(&attrs);
    strncpy(attrs.requestMethod, "GET", 3);
    attrs.attributes =
        EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_PERSIST_FILE;
    attrs.userData  = data.hash.data();
    attrs.onsuccess = [](emscripten_fetch_t* fetch) {
        auto& data =
            mmap_async_data[reinterpret_cast<const char*>(fetch->userData)];
        data.promise.set_value(posix::mem_mapping_t{
            .view = posix::mem_mapping_t::span_type(
                const_cast<char*>(fetch->data), fetch->numBytes),
            .access   = RSCA::ReadOnly,
            .fetch_fd = fetch,
        });
    };
    attrs.onerror = [](emscripten_fetch_t* fetch) {
        auto  key  = reinterpret_cast<const char*>(fetch->userData);
        auto& data = mmap_async_data[key];
        data.promise.set_value(posix::mem_mapping_t{.access = RSCA::None});
        emscripten_fetch_close(fetch);
        mmap_async_data.erase(key);
    };
    attrs.onreadystatechange = [](emscripten_fetch_t* fetch) {
        const char* status = nullptr;
        switch(fetch->readyState)
        {
        case 0:
            status = "unsent";
            break;
        case 1:
            status = "opened";
            break;
        case 2:
            status = "headers_received";
            break;
        case 3:
            status = "loading";
            break;
        case 4:
            status = "done";
            break;
        default:
            return;
        }
        fprintf(
            stderr, "emscripten_fetch: url=%s, state=%s", fetch->url, status);
    };
    emscripten_fetch(&attrs, file.internUrl.c_str());

    // emscripten_async_wget(
    //     file.internUrl.c_str(),
    //     data.hash.c_str(),
    //     [](const char* hash) {
    //         auto it = mmap_async_data.find(hash + 1);
    //         if(it == mmap_async_data.end())
    //         {
    //             fprintf(stderr, "Failed to lookup data for %s\n", hash + 1);
    //             return;
    //         }
    //         auto* pdata = &it->second;
    //         fprintf(
    //             stdout, "Received %s from %s\n", hash + 1,
    //             pdata->url.c_str());
    //         if(auto mapping = mmap_download(hash); mapping.has_value())
    //             pdata->promise.set_value(std::move(mapping.value()));
    //         else
    //             pdata->promise.set_value(posix::mem_mapping_t{});
    //         mmap_async_data.erase(hash);
    //     },
    //     [](const char* hash) {
    //         auto* pdata = &mmap_async_data[hash];
    //         fprintf(stderr, "Failed to download: %s\n", pdata->url.c_str());
    //         mmap_async_data.erase(hash);
    //     });
    return fut;
}

} // namespace platform::file::emscripten
