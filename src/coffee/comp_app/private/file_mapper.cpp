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

void FileMapper::set_download_queue(rq::runtime_queue* queue)
{
    m_download_queue = queue;
}

rq::runtime_queue* FileMapper::download_queue()
{
    return m_download_queue ? m_download_queue : io_queue();
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
    if(platform::file::emscripten::is_offline_url(url.internUrl))
        return url.internUrl;
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

/* Where a download stands, for fetch_set(); all optional */
struct download_state_t
{
    std::atomic<std::uint64_t>* received{nullptr};
    std::atomic<std::uint64_t>* expected{nullptr};
    std::atomic<bool> const*    cancelled{nullptr};

    void sizes(std::uint64_t in, std::uint64_t total) const
    {
        if(expected)
            expected->store(total);
        if(received)
            received->store(in);
    }
    bool stopped() const
    {
        return cancelled && cancelled->load();
    }
};

/* Downloads [0, size) of url by range in chunks: each lands in
 * buffer_for(offset, size) and is handed to commit() */
static bool download_chunks(
    platform::url::Url const&                                               url,
    std::uint64_t                                                           size,
    download_state_t const&                                                 state,
    std::function<gsl::span<char>(std::uint64_t, std::uint64_t)> const&    buffer_for,
    std::function<bool(std::uint64_t, gsl::span<char>)> const&             commit)
{
    using namespace platform::file::emscripten;
    constexpr std::uint64_t chunk    = 16u << 20;
    std::uint64_t           done     = 0;
    int                     last_pct = 0;
    while(done < size)
    {
        if(state.stopped())
            return false;
        auto take   = std::min<std::uint64_t>(chunk, size - done);
        auto buffer = buffer_for(done, take);
        if(!fetch_range(url.internUrl, done, buffer) || !commit(done, buffer))
            return false;
        done += take;
        state.sizes(done, size);
        int pct = static_cast<int>(done * 100 / size);
        if(pct / 25 != last_pct / 25)
            fmt::print(stderr, "FileMapper: {} {}%\n", url.internUrl, pct);
        last_pct = pct;
    }
    return true;
}

/* Brings url into the store unless the copy there has the server's size.
 * Downloads by range in chunks so the heap only ever holds one; a server
 * without Range support handed the whole file back with the probe. Returns
 * the file's size, 0 if it is not to be had */
static std::uint64_t store_sync(platform::url::Url const& url, download_state_t const& state = {})
{
    using namespace platform::file::emscripten;
    if(!store_ready())
        return 0;
    auto path  = store_path(url);
    auto local = store_size(path);
    if(is_offline_url(url.internUrl))
    {
        state.sizes(local, local);
        return local;
    }
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
        state.sizes(local, local);
        return local;
    }
    if(local == probe.size && probe.size)
    {
        if(probe.whole)
            platform::file::emscripten::unmap(std::move(*probe.whole));
        state.sizes(local, local);
        return local;
    }
    state.sizes(0, probe.size);
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
        state.sizes(done, probe.size);
    } else
    {
        std::vector<char> buffer;
        auto              buffer_for = [&buffer](std::uint64_t, std::uint64_t size) {
            buffer.resize(size);
            return gsl::span<char>(buffer);
        };
        auto commit = [fd, &done](std::uint64_t offset, gsl::span<char> chunk) {
            if(::pwrite(fd, chunk.data(), chunk.size(), offset) != static_cast<ssize_t>(chunk.size()))
                return false;
            done = offset + chunk.size();
            return true;
        };
        download_chunks(url, probe.size, state, buffer_for, commit);
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
    /* Store files are read by range too: they can be larger than the heap */
    const bool is_remote = !platform::file::exists(src).has_value() &&
                           enum_helpers::feval(src.flags, semantic::RSCA::AssetFile);
    if(is_remote || platform::file::emscripten::is_offline_url(src.internUrl))
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
    if(auto live = live_resource(src))
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
                        remember(res);
                    prom->set_value(std::move(res));
                }))
            .assume_value();
        return prom->get_future();
    } else
#endif
    {
        std::promise<std::shared_ptr<Resource>> out;
        auto                                    fut = out.get_future();
        if(auto mapped = map_local(src))
            remember(mapped), out.set_value(std::move(mapped));
        else
            out.set_value({});
        return fut;
    }
}

/* A local file mapped, outside fetch()'s cache so any thread may do it */
std::shared_ptr<FileMapper::Resource> FileMapper::map_local(platform::url::Url const& src)
{
    auto size = platform::file::size(src);
    if(!size.has_value())
        return nullptr;
    auto mapping = platform::file::map(src, {.size = size.value()});
    if(!mapping.has_value())
        return nullptr;
    auto res       = std::make_shared<FileMapper::Resource>();
    res->m_mapping = std::move(mapping.value());
    res->m_url     = src;
    return res;
}

std::shared_ptr<FileMapper::Resource> FileMapper::live_resource(platform::url::Url const& src)
{
    std::lock_guard _(m_lock);
    auto            it = m_resources.find(src.internUrl);
    return it != m_resources.end() ? it->second.lock() : nullptr;
}

std::shared_ptr<FileMapper::RangedFile> FileMapper::live_ranged(platform::url::Url const& src)
{
    std::lock_guard _(m_lock);
    auto            it = m_ranged.find(src.internUrl);
    return it != m_ranged.end() ? it->second.lock() : nullptr;
}

void FileMapper::remember(std::shared_ptr<Resource> const& res)
{
    std::lock_guard _(m_lock);
    m_resources[res->m_url.internUrl] = res;
}

void FileMapper::remember(std::shared_ptr<RangedFile> const& file)
{
    std::lock_guard _(m_lock);
    m_ranged[file->m_url.internUrl] = file;
}

float FileMapper::FetchSet::progress() const
{
    std::uint64_t in = 0, total = 0;
    for(std::size_t i = 0; i < m_entries.size(); i++)
    {
        in += m_slots[i].received.load();
        total += m_slots[i].expected.load();
    }
    if(total)
        return static_cast<float>(static_cast<double>(in) / static_cast<double>(total));
    return done() ? 1.f : 0.f;
}

bool FileMapper::FetchSet::done() const
{
    for(std::size_t i = 0; i < m_entries.size(); i++)
        if(!m_slots[i].settled.load(std::memory_order_acquire))
            return false;
    return true;
}

bool FileMapper::FetchSet::failed() const
{
    return m_failed.load();
}

void FileMapper::FetchSet::cancel()
{
    m_cancelled.store(true);
}

bool FileMapper::FetchSet::cancelled() const
{
    return m_cancelled.load();
}

std::shared_ptr<FileMapper::Resource> FileMapper::FetchSet::resource(std::size_t i) const
{
    return m_slots[i].settled.load(std::memory_order_acquire) ? m_slots[i].resource : nullptr;
}

std::shared_ptr<FileMapper::RangedFile> FileMapper::FetchSet::ranged(std::size_t i) const
{
    return m_slots[i].settled.load(std::memory_order_acquire) ? m_slots[i].ranged : nullptr;
}

#if defined(COFFEE_EMSCRIPTEN)
static bool is_remote_url(platform::url::Url const& src)
{
    return platform::file::emscripten::is_offline_url(src.internUrl) ||
           (!platform::file::exists(src).has_value() &&
            enum_helpers::feval(src.flags, semantic::RSCA::AssetFile));
}

#endif

void FileMapper::fetch_one(FetchSet& set, std::size_t i)
{
    auto const&            entry = set.m_entries[i];
    auto&                  slot  = set.m_slots[i];
    if(entry.mode == fetch_mode::memory)
        slot.resource = live_resource(entry.url);
    else
        slot.ranged = live_ranged(entry.url);
    if(slot.resource || slot.ranged)
    {
        auto size = slot.resource ? static_cast<semantic::BytesConst>(*slot.resource).size : slot.ranged->size();
        fmt::print(stderr, "FileMapper: {} still held, reused\n", entry.url.internUrl);
        slot.expected.store(size);
        slot.received.store(size);
    } else if(!set.cancelled())
    {
#if defined(COFFEE_EMSCRIPTEN)
        if(is_remote_url(entry.url))
        {
            download_state_t const state{&slot.received, &slot.expected, &set.m_cancelled};
            if(entry.mode == fetch_mode::memory && !entry.cache)
            {
                /* The whole file into the heap by range, without the local store */
                using namespace platform::file::emscripten;
                auto probe = fetch_probe(entry.url.internUrl);
                auto res   = std::make_shared<Resource>();
                res->m_url = entry.url;
                if(probe.whole)
                {
                    auto view = probe.whole->view;
                    res->m_alloc.assign(view.begin(), view.end());
                    platform::file::emscripten::unmap(std::move(*probe.whole));
                    state.sizes(res->m_alloc.size(), res->m_alloc.size());
                    slot.resource = std::move(res);
                } else if(probe.status == 206)
                {
                    state.sizes(0, probe.size);
                    res->m_alloc.resize(probe.size);
                    auto buffer_for = [&res](std::uint64_t offset, std::uint64_t size) {
                        return gsl::span<char>(res->m_alloc.data() + offset, size);
                    };
                    auto commit = [](std::uint64_t, gsl::span<char>) { return true; };
                    if(download_chunks(entry.url, probe.size, state, buffer_for, commit))
                        slot.resource = std::move(res);
                }
            } else if(auto size = store_sync(entry.url, state); size && !set.cancelled())
            {
                int fd = ::open(store_path(entry.url).c_str(), O_RDONLY);
                if(fd >= 0 && entry.mode == fetch_mode::ranged)
                {
                    auto file     = std::make_shared<RangedFile>();
                    file->m_url   = entry.url;
                    file->m_owner = this;
                    file->m_size  = size;
                    file->m_fd    = fd;
                    slot.ranged   = std::move(file);
                } else if(fd >= 0)
                {
                    auto res   = std::make_shared<Resource>();
                    res->m_url = entry.url;
                    res->m_alloc.resize(size);
                    if(::pread(fd, res->m_alloc.data(), size, 0) == static_cast<ssize_t>(size))
                        slot.resource = std::move(res);
                    ::close(fd);
                }
            }
        } else
#endif
            if(auto mapping = map_local(entry.url))
        {
            auto size = static_cast<semantic::BytesConst>(*mapping).size;
            slot.expected.store(size);
            slot.received.store(size);
            if(entry.mode == fetch_mode::ranged)
            {
                auto file       = std::make_shared<RangedFile>();
                file->m_url     = entry.url;
                file->m_owner   = this;
                file->m_mapping = std::move(mapping);
                file->m_size    = size;
                slot.ranged     = std::move(file);
            } else
                slot.resource = std::move(mapping);
        }
    }
    if(slot.resource)
        remember(slot.resource);
    if(slot.ranged)
        remember(slot.ranged);
    if(!slot.resource && !slot.ranged && !entry.optional)
        set.m_failed.store(true);
    /* A file that never came counts as done, so progress still reaches the end */
    if(!slot.resource && !slot.ranged)
        slot.received.store(slot.expected.load());
    slot.settled.store(true, std::memory_order_release);
}

std::shared_ptr<FileMapper::FetchSet> FileMapper::fetch_set(
    std::vector<fetch_entry> entries, rq::runtime_queue* deliver_on, ready_fn on_ready)
{
    auto set       = std::make_shared<FetchSet>();
    set->m_entries = std::move(entries);
    set->m_slots   = std::make_unique<FetchSet::slot_t[]>(set->m_entries.size());
    auto* queue    = download_queue();

    /* Sizes first, so progress is over the whole set from the start */
    auto sizes = [this, set] {
        for(std::size_t i = 0; i < set->m_entries.size(); i++)
        {
            auto const&   url  = set->m_entries[i].url;
            std::uint64_t size = 0;
            bool const    memory = set->m_entries[i].mode == fetch_mode::memory;
            if(auto live = memory ? live_resource(url) : nullptr)
                size = static_cast<semantic::BytesConst>(*live).size;
            else if(auto file = memory ? nullptr : live_ranged(url))
                size = file->size();
            if(size)
            {
                set->m_slots[i].expected.store(size);
                continue;
            }
#if defined(COFFEE_EMSCRIPTEN)
            if(is_remote_url(url))
            {
                using namespace platform::file::emscripten;
                auto local = store_size(store_path(url));
                if(is_offline_url(url.internUrl))
                    size = local;
                else
                {
                    auto probe = fetch_probe(url.internUrl);
                    if(probe.whole)
                        platform::file::emscripten::unmap(std::move(*probe.whole));
                    size = probe.size ? probe.size : local;
                }
            } else
#endif
                size = platform::file::size(url).value_or(0);
            set->m_slots[i].expected.store(size);
        }
    };
    auto each = [this, set, deliver_on, on_ready](std::size_t i) {
        fetch_one(*set, i);
        auto deliver = [set, i, on_ready] {
            if(!set->cancelled() && on_ready)
                on_ready(*set, i);
        };
        if(deliver_on)
            rq::runtime_queue::QueueImmediate(deliver_on, {}, std::move(deliver)).assume_value();
        else
            deliver();
    };
    /* One task for the set keeps its files in order */
    auto all = [set, sizes, each] {
        sizes();
        for(std::size_t i = 0; i < set->m_entries.size(); i++)
            each(i);
    };
    if(!queue)
        all();
    else
        rq::runtime_queue::QueueImmediate(queue, {}, std::move(all)).assume_value();
    return set;
}

std::future<std::vector<char>> FileMapper::read_head(platform::url::Url const& src, std::size_t bytes)
{
    using head_t = std::vector<char>;
#if defined(COFFEE_EMSCRIPTEN)
    if(is_remote_url(src))
        return run_on<head_t>(download_queue(), [src, bytes]() -> head_t {
            using namespace platform::file::emscripten;
            auto probe = is_offline_url(src.internUrl) ? range_probe_t{} : fetch_probe(src.internUrl);
            auto path  = store_path(src);
            auto local = store_size(path);
            /* A complete copy in the store is read from disk */
            if(local && (is_offline_url(src.internUrl) || probe.size == local))
            {
                if(probe.whole)
                    platform::file::emscripten::unmap(std::move(*probe.whole));
                head_t out(std::min<std::uint64_t>(bytes, local));
                int    fd = ::open(path.c_str(), O_RDONLY);
                bool   ok = fd >= 0 && ::pread(fd, out.data(), out.size(), 0) == static_cast<ssize_t>(out.size());
                if(fd >= 0)
                    ::close(fd);
                return ok ? out : head_t{};
            }
            if(probe.whole)
            {
                auto   view = probe.whole->view;
                head_t out(view.begin(), view.begin() + std::min<std::size_t>(bytes, view.size()));
                platform::file::emscripten::unmap(std::move(*probe.whole));
                return out;
            }
            if(probe.status != 206)
                return {};
            head_t out(std::min<std::uint64_t>(bytes, probe.size));
            return fetch_range(src.internUrl, 0, gsl::span<char>(out)) ? out : head_t{};
        });
#endif
    std::promise<head_t> out;
    auto                 fut = out.get_future();
    head_t               head;
    if(auto mapping = map_local(src))
    {
        auto view = static_cast<semantic::BytesConst>(*mapping);
        head.assign(view.data, view.data + std::min<std::size_t>(bytes, view.size));
    }
    out.set_value(std::move(head));
    return fut;
}

#endif

} // namespace comp_app
