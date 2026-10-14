#include <coffee/comp_app/file_mapper.h>

#include <coffee/core/task_queue/task.h>

#if defined(COFFEE_EMSCRIPTEN)
#include <fcntl.h>
#include <platforms/emscripten/fsio.h>
#include <platforms/emscripten/mmio.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <cstring>
#include <fmt/core.h>

namespace comp_app {

#if !defined(COFFEE_GEKKO)

void FileMapper::set_io_queue(rq::runtime_queue* queue)
{
    m_io_queue = queue;
}

rq::runtime_queue* FileMapper::io_queue()
{
    if(!m_io_queue)
        fmt::print(stderr, "FileMapper: no IO queue set, range reads fail\n");
    return m_io_queue;
}

/* Runs fn on the IO queue and hands back its result */
template<typename T>
static std::future<T> run_on(rq::runtime_queue* queue, std::function<T()>&& fn)
{
    auto task = rq::dependent_task<void, T>::CreateSource(std::move(fn));
    auto fut  = task->get_future();
    if(!queue || rq::runtime_queue::Queue(queue, std::move(task)).has_error())
        return {};
    return fut;
}

#if defined(COFFEE_EMSCRIPTEN)
/* The local store: /maps on the persistent backend */
static bool store_ready()
{
    static bool ready = platform::file::emscripten::persistent_mkdir("/maps");
    if(!ready)
        fmt::print(stderr, "FileMapper: no persistent store, /maps unavailable\n");
    return ready;
}

static std::string store_path(platform::url::Url const& url)
{
    std::string name = url.internUrl;
    for(auto& c : name)
        if(c == '/' || c == '\\' || c == ':')
            c = '_';
    return "/maps/" + name;
}

static std::uint64_t store_size(std::string const& path)
{
    struct stat st{};
    return ::stat(path.c_str(), &st) == 0 ? static_cast<std::uint64_t>(st.st_size)
                                          : 0;
}

/* Brings url into the store unless the copy there has the server's size.
 * Downloads by range in chunks so the heap only ever holds one; a server
 * without Range support handed the whole file back with the probe. Returns
 * the file's size, 0 if it is not to be had */
static std::uint64_t store_sync(platform::url::Url const& url)
{
    using namespace platform::file::emscripten;
    if(!store_ready())
        return 0;
    auto path  = store_path(url);
    auto local = store_size(path);
    if(is_offline_url(url.internUrl))
        return local;
    auto probe = fetch_probe(url.internUrl);
    if(probe.status != 200 && probe.status != 206)
    {
        /* Offline, or gone: whatever is cached */
        if(local)
            fmt::print(
                stderr,
                "FileMapper: {} not reachable (status {}), using the cached copy\n",
                url.internUrl,
                probe.status);
        return local;
    }
    if(local == probe.size && probe.size)
    {
        if(probe.whole)
            platform::file::emscripten::unmap(std::move(*probe.whole));
        return local;
    }
    fmt::print(
        stderr, "FileMapper: caching {} ({} MiB)\n", url.internUrl, probe.size >> 20);
    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0)
        return 0;
    std::uint64_t done = 0;
    if(probe.whole)
    {
        auto view = probe.whole->view;
        if(::pwrite(fd, view.data(), view.size(), 0) == static_cast<ssize_t>(view.size()))
            done = view.size();
        platform::file::emscripten::unmap(std::move(*probe.whole));
    } else
    {
        constexpr std::uint64_t chunk = 16u << 20;
        std::vector<char>       buffer;
        int                     last_pct = 0;
        while(done < probe.size)
        {
            auto take = std::min<std::uint64_t>(chunk, probe.size - done);
            buffer.resize(take);
            if(!fetch_range(url.internUrl, done, gsl::span<char>(buffer)))
                break;
            if(::pwrite(fd, buffer.data(), take, done) != static_cast<ssize_t>(take))
                break;
            done += take;
            int pct = static_cast<int>(done * 100 / probe.size);
            if(pct / 25 != last_pct / 25)
                fmt::print(stderr, "FileMapper: {} {}%\n", url.internUrl, pct);
            last_pct = pct;
        }
    }
    ::close(fd);
    if(done != probe.size)
    {
        fmt::print(
            stderr,
            "FileMapper: caching {} failed at {} of {} bytes\n",
            url.internUrl,
            done,
            probe.size);
        ::unlink(path.c_str());
        return 0;
    }
    return done;
}
#endif

FileMapper::RangedFile::~RangedFile()
{
#if defined(COFFEE_EMSCRIPTEN)
    if(m_fd >= 0)
        ::close(m_fd);
#endif
}

bool FileMapper::RangedFile::read(std::uint64_t offset, gsl::span<char> dst)
{
    if(offset > m_size || dst.size() > m_size - offset)
        return false;
    if(m_mapping)
    {
        auto bytes = static_cast<semantic::BytesConst>(*m_mapping);
        std::memcpy(dst.data(), bytes.data + offset, dst.size());
        return true;
    }
#if defined(COFFEE_EMSCRIPTEN)
    if(m_fd < 0)
        return false;
    auto fut = run_on<bool>(m_owner->io_queue(), [this, offset, dst] {
        return ::pread(m_fd, dst.data(), dst.size(), offset) ==
               static_cast<ssize_t>(dst.size());
    });
    return fut.valid() && fut.get();
#else
    return false;
#endif
}

std::future<std::shared_ptr<FileMapper::RangedFile>> FileMapper::open_ranged(
    platform::url::Url const& src)
{
    using file_ptr = std::shared_ptr<RangedFile>;
#if defined(COFFEE_EMSCRIPTEN)
    const bool is_remote = !platform::file::exists(src).has_value() &&
                           enum_helpers::feval(src.flags, semantic::RSCA::AssetFile);
    if(is_remote)
    {
        auto file     = std::make_shared<RangedFile>();
        file->m_url   = src;
        file->m_owner = this;
        return run_on<file_ptr>(io_queue(), [file]() -> file_ptr {
            file->m_size = store_sync(file->m_url);
            if(!file->m_size)
                return nullptr;
            file->m_fd = ::open(store_path(file->m_url).c_str(), O_RDONLY);
            return file->m_fd >= 0 ? file : nullptr;
        });
    }
#endif
    /* Local files are mapped; the range reads are copies out of the view */
    std::promise<file_ptr> out;
    auto                   fut     = out.get_future();
    auto                   mapping = fetch(src).get();
    if(!mapping)
    {
        out.set_value({});
        return fut;
    }
    auto file       = std::make_shared<RangedFile>();
    file->m_url     = src;
    file->m_owner   = this;
    file->m_mapping = mapping;
    file->m_size    = static_cast<semantic::BytesConst>(*mapping).size;
    out.set_value(std::move(file));
    return fut;
}

std::future<std::shared_ptr<FileMapper::Resource>> FileMapper::fetch_cached(
    platform::url::Url const& src)
{
#if defined(COFFEE_EMSCRIPTEN)
    const bool is_remote = !platform::file::exists(src).has_value() &&
                           enum_helpers::feval(src.flags, semantic::RSCA::AssetFile);
    if(is_remote)
    {
        using res_ptr = std::shared_ptr<Resource>;
        return run_on<res_ptr>(io_queue(), [src]() -> res_ptr {
            auto size = store_sync(src);
            if(!size)
                return nullptr;
            int fd = ::open(store_path(src).c_str(), O_RDONLY);
            if(fd < 0)
                return nullptr;
            auto res   = std::make_shared<Resource>();
            res->m_url = src;
            res->m_alloc.resize(size);
            bool ok = ::pread(fd, res->m_alloc.data(), size, 0) ==
                      static_cast<ssize_t>(size);
            ::close(fd);
            return ok ? res : nullptr;
        });
    }
#endif
    return fetch(src);
}

FileMapper::Resource::~Resource()
{
    if(static_cast<gsl::span<char>>(m_mapping).data())
        platform::file::unmap(std::move(m_mapping));
}

FileMapper::Resource::operator semantic::BytesConst() const
{
#if defined(COFFEE_EMSCRIPTEN)
    if(!m_alloc.empty())
        return semantic::mem_chunk<const char>::ofContainer(m_alloc);
#endif
    auto span = static_cast<gsl::span<char>>(m_mapping);
    return semantic::mem_chunk<const char>::ofContainer(span);
}

std::future<std::shared_ptr<FileMapper::Resource>> FileMapper::fetch(
    const platform::url::Url& src)
{
    if(auto it = m_resources.find(src.internUrl); it != m_resources.end())
        if(auto live = it->second.lock())
        {
            std::promise<std::shared_ptr<Resource>> out;
            out.set_value(std::move(live));
            return out.get_future();
        }

    auto res = std::make_shared<Resource>();

    res->m_url = src;

#if defined(COFFEE_EMSCRIPTEN)
    /* Only use wget if the file is not local */
    const bool is_remote =
        compile_info::platform::is_emscripten &&
        !platform::file::exists(src).has_value() &&
        enum_helpers::feval(src.flags, semantic::RSCA::AssetFile);

    if(is_remote && platform::file::emscripten::is_offline_url(src.internUrl))
        return fetch_cached(src);
    if(is_remote)
    {
        using mapping_t = platform::file::posix::mem_mapping_t;
        auto prom = std::make_shared<std::promise<std::shared_ptr<Resource>>>();
        rq::runtime_queue::Queue(
            rq::dependent_task<mapping_t, void>::CreateSink(
                platform::file::emscripten::mmap_async(src),
                [this, res, prom](mapping_t* data) mutable {
                    // res->m_alloc   = std::move(data->alloc.allocation);
                    res->m_mapping = platform::file::map_handle{
                        .view     = data->view,
                        .access   = semantic::RSCA::ReadOnly,
                        .fetch_fd = data->fetch_fd,
                    };
                    if(data->view.data())
                        m_resources[res->m_url.internUrl] = res;
                    prom->set_value(std::move(res));
                }))
            .assume_value();
        return prom->get_future();
    } else
#endif
    {
        std::promise<std::shared_ptr<Resource>> out;
        auto                                    fut = out.get_future();
        if(auto size = platform::file::size(src); size.has_value())
        {
            if(auto mapping = platform::file::map(src, {.size = size.value()});
               mapping.has_value())
            {
                res->m_mapping = std::move(mapping.value());
                res->m_url     = src;
                m_resources[src.internUrl] = res;
                out.set_value(std::move(res));
            } else
                /* mapping failed despite a valid size: return empty rather
                 * than letting the promise break */
                out.set_value({});
        } else
            out.set_value({});
        return fut;
    }
}

#endif

} // namespace comp_app
