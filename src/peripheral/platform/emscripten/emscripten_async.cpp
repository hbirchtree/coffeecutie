#include <fmt/core.h>
#include <platforms/emscripten/fsio.h>
#include <platforms/emscripten/mmio.h>
#include <platforms/emscripten/rdwrio.h>

#include <emscripten/emscripten.h>

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
    auto mapping
        = posix::map(tmpfile, posix::mapping_params_t{.size = size.value()});
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
    auto fut = data.promise.get_future();
    emscripten_async_wget(
        file.internUrl.c_str(),
        data.hash.c_str(),
        [](const char* hash) {
            auto it = mmap_async_data.find(hash + 1);
            if(it == mmap_async_data.end())
            {
                fprintf(stderr, "Failed to lookup data for %s\n", hash + 1);
                return;
            }
            auto* pdata = &it->second;
            fprintf(
                stdout, "Received %s from %s\n", hash + 1, pdata->url.c_str());
            if(auto mapping = mmap_download(hash); mapping.has_value())
                pdata->promise.set_value(std::move(mapping.value()));
            else
                pdata->promise.set_value(posix::mem_mapping_t{});
            mmap_async_data.erase(hash);
        },
        [](const char* hash) {
            auto* pdata = &mmap_async_data[hash];
            fprintf(stderr, "Failed to download: %s\n", pdata->url.c_str());
            mmap_async_data.erase(hash);
        });
    return fut;
}

} // namespace platform::file::emscripten
