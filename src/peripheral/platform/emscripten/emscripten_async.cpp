#include <fmt/core.h>
#include <fmt/format.h>
#include <platforms/emscripten/fsio.h>
#include <platforms/emscripten/mmio.h>
#include <platforms/emscripten/rdwrio.h>

#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/wasmfs.h>
#include <sys/stat.h>
#include <thread>

#include <cctype>
#include <cstring>
#include <mutex>
#include <vector>

namespace platform::file::emscripten {

/* /config is on OPFS under WasmFS: a closed file is already durable */
static void sync_storage(semantic::RSCA)
{
}

bool persistent_mkdir(const char* path)
{
    static backend_t opfs = [] {
        backend_t out = nullptr;
        /* Creating the backend blocks until the main thread is back in its
         * event loop, so not from the main thread itself */
        std::thread([&out] { out = wasmfs_create_opfs_backend(); }).join();
        return out;
    }();
    if(!opfs)
        return false;
    struct stat st{};
    if(::stat(path, &st) == 0)
        return S_ISDIR(st.st_mode);
    return wasmfs_create_directory(path, 0777, opfs) == 0;
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

static std::mutex               offline_prefixes_lock;
static std::vector<std::string> offline_prefixes;

void add_offline_prefix(std::string prefix)
{
    std::lock_guard _(offline_prefixes_lock);
    offline_prefixes.push_back(std::move(prefix));
}

bool is_offline_url(std::string_view url)
{
    std::lock_guard _(offline_prefixes_lock);
    for(auto const& prefix : offline_prefixes)
        if(url.starts_with(prefix))
            return true;
    return false;
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
    fprintf(stdout, "Getting file from VFS: %s\n", hash);
    auto tmpfile = url::constructors::MkUrl(hash);
    auto size    = posix::size(tmpfile);
    auto mapping =
        posix::map(tmpfile, posix::mapping_params_t{.size = size.value()});
    if(mapping.has_value())
        return mapping.value();
    else
        return std::nullopt;
}

std::optional<posix::posix_error> unmap(posix::mem_mapping_t&& mapping)
{
    if(!mapping.fetch_fd)
        return posix::unmap(std::move(mapping));
    emscripten_fetch_close(static_cast<emscripten_fetch_t*>(mapping.fetch_fd));
    return std::nullopt;
}

/* One synchronous GET, with a Range when last >= first; the response stays
 * open for the caller, whatever its status */
static emscripten_fetch_t* fetch_sync(
    std::string const& url, std::uint64_t first = 1, std::uint64_t last = 0)
{
    auto        range     = fmt::format("bytes={}-{}", first, last);
    const char* headers[] = {"Range", range.c_str(), nullptr};
    emscripten_fetch_attr_t attrs;
    emscripten_fetch_attr_init(&attrs);
    strncpy(attrs.requestMethod, "GET", 3);
    attrs.attributes =
        EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
    if(last >= first)
        attrs.requestHeaders = headers;
    return emscripten_fetch(&attrs, url.c_str());
}

static emscripten_fetch_t* fetch_sync_range(
    std::string const& url, std::uint64_t first, std::uint64_t last)
{
    auto* fetch = fetch_sync(url, first, last);
    if(!fetch)
        return nullptr;
    if(fetch->status != 206)
    {
        fmt::print(
            stderr,
            "fetch_range: {} bytes={}-{}: status {}, no partial content\n",
            url,
            first,
            last,
            fetch->status);
        emscripten_fetch_close(fetch);
        return nullptr;
    }
    return fetch;
}

range_probe_t fetch_probe(std::string const& url)
{
    auto* fetch = fetch_sync(url, 0, 0);
    if(!fetch)
        return {};
    range_probe_t out{.status = fetch->status};
    if(fetch->status == 200)
    {
        /* Range ignored: the file is already here, keep it */
        out.size  = fetch->numBytes;
        out.whole = posix::mem_mapping_t{
            .view = posix::mem_mapping_t::span_type(
                const_cast<char*>(fetch->data), fetch->numBytes),
            .access   = RSCA::ReadOnly,
            .fetch_fd = fetch,
        };
        return out;
    }
    if(fetch->status != 206)
    {
        emscripten_fetch_close(fetch);
        return out;
    }
    auto len = emscripten_fetch_get_response_headers_length(fetch);
    std::string headers(len + 1, '\0');
    emscripten_fetch_get_response_headers(fetch, headers.data(), headers.size());
    emscripten_fetch_close(fetch);
    /* content-range: bytes 0-0/429017088 */
    for(auto& c : headers)
        c = static_cast<char>(tolower(c));
    auto at = headers.find("content-range:");
    auto slash = at == std::string::npos ? at : headers.find('/', at);
    if(slash == std::string::npos)
        return out;
    for(auto p = slash + 1; p < headers.size() && isdigit(headers[p]); p++)
        out.size = out.size * 10 + static_cast<std::uint64_t>(headers[p] - '0');
    return out;
}

bool fetch_range(
    std::string const& url, std::uint64_t offset, gsl::span<char> dst)
{
    if(dst.empty())
        return true;
    auto* fetch = fetch_sync_range(url, offset, offset + dst.size() - 1);
    if(!fetch)
        return false;
    bool ok = fetch->numBytes == dst.size();
    if(ok)
        memcpy(dst.data(), fetch->data, dst.size());
    else
        fmt::print(
            stderr,
            "fetch_range: {} +{}: got {} of {} bytes\n",
            url,
            offset,
            fetch->numBytes,
            dst.size());
    emscripten_fetch_close(fetch);
    return ok;
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
        out.set_value(mmap_download(file.internUrl.c_str())
                          .value_or(posix::mem_mapping_t{}));
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
    /* Without EMSCRIPTEN_FETCH_REPLACE, Fetch looks in IndexedDB first.
     * NO_DOWNLOAD turns a cache miss into an error rather than an XHR. */
    attrs.attributes =
        EMSCRIPTEN_FETCH_LOAD_TO_MEMORY |
        (is_offline_url(file.internUrl) ? EMSCRIPTEN_FETCH_NO_DOWNLOAD
                                        : EMSCRIPTEN_FETCH_PERSIST_FILE);
    attrs.userData  = data.hash.data();
    attrs.onsuccess = [](emscripten_fetch_t* fetch) {
        std::string key(reinterpret_cast<const char*>(fetch->userData));
        auto&       data = mmap_async_data[key];
        data.promise.set_value(
            posix::mem_mapping_t{
                .view = posix::mem_mapping_t::span_type(
                    const_cast<char*>(fetch->data), fetch->numBytes),
                .access   = RSCA::ReadOnly,
                .fetch_fd = fetch,
            });
        mmap_async_data.erase(key);
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
