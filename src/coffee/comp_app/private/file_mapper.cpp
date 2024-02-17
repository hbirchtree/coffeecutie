#include <coffee/comp_app/file_mapper.h>

#include <coffee/core/task_queue/task.h>

namespace comp_app {

FileMapper::Resource::~Resource()
{
    platform::file::unmap(std::move(m_mapping));
}

FileMapper::Resource::operator semantic::BytesConst() const
{
    auto span = static_cast<gsl::span<char>>(m_mapping);
    return semantic::mem_chunk<const char>::ofContainer(span);
}

void FileMapper::start_frame(compo::ContainerProxy&, const compo::time_point&)
{
}

std::future<std::shared_ptr<FileMapper::Resource>> FileMapper::fetch(
    const platform::url::Url& src)
{
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
                        // .view = gsl::span<char>(
                        //     res->m_alloc.data(), res->m_alloc.size()),
                        .view   = data->view,
                        .access = semantic::RSCA::ReadOnly,
                    };
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
            if(auto mapping = platform::file::map(src, {.size = size.value()});
               mapping.has_value())
            {
                res->m_mapping = std::move(mapping.value());
                res->m_url     = src;
                out.set_value(std::move(res));
            }
        /* Promise breaks here if we failed to get the file */
        return fut;
    }
}

} // namespace comp_app
