#include <coffee/comp_app/file_mapper.h>

#include <coffee/core/task_queue/task.h>

#if defined(COFFEE_EMSCRIPTEN)
#include <platforms/emscripten/mmio.h>
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
    auto fut = run_on<bool>(m_owner->io_queue(), [this, offset, dst] {
        return platform::file::emscripten::fetch_range(
            m_url.internUrl, offset, dst);
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
            using namespace platform::file::emscripten;
            auto probe = fetch_probe(file->m_url.internUrl);
            if(probe.status == 200)
            {
                /* No Range support: the whole file, as a mapping would be */
                fmt::print(
                    stderr,
                    "FileMapper: {} is served without Range support, "
                    "fetching it whole ({} bytes)\n",
                    file->m_url.internUrl,
                    probe.size);
                if(auto& whole = probe.whole)
                {
                    auto res       = std::make_shared<Resource>();
                    res->m_url     = file->m_url;
                    res->m_mapping = platform::file::map_handle{
                        .view     = whole->view,
                        .access   = semantic::RSCA::ReadOnly,
                        .fetch_fd = whole->fetch_fd,
                    };
                    file->m_mapping = res;
                    file->m_size    = whole->view.size();
                }
            } else
                file->m_size = probe.size;
            return file->m_size ? file : nullptr;
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

FileMapper::Resource::~Resource()
{
    platform::file::unmap(std::move(m_mapping));
}

FileMapper::Resource::operator semantic::BytesConst() const
{
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
