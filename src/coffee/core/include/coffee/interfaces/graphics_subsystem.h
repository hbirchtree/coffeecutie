#pragma once

#include <coffee/components/types.h>
#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/interfaces/cgraphics_util.h>
#include <peripherals/stl/functional_types.h>

#include <type_traits>

namespace Coffee {
namespace RHI {
namespace Components {

template<typename API>
struct AllocatorData
{
    /* Context data for graphics RHI */
    typename API::API_CONTEXT context;

    template<typename T>
    using ApiStore = Vector<ShPtr<T>>;

    using PipelineParams = RHI::shader_param_view<API>;

    struct PipelineData
    {
        PipelineData() : params(pipeline)
        {
        }

        ~PipelineData()
        {
            pipeline.dealloc();
        }

        typename API::PIP pipeline;
        PipelineParams    params;
    };

    ~AllocatorData()
    {
#define DEALLOC_BLOCK(TYPE, VAR) \
    {                            \
        for(auto& buf : VAR)     \
            buf->dealloc();      \
    }

        DEALLOC_BLOCK(typename API::V_DESC, vertex_desc);

        DEALLOC_BLOCK(typename API::BUF_A, array_bufs);
        DEALLOC_BLOCK(typename API::BUF_E, element_bufs);
        DEALLOC_BLOCK(typename API::BUF_U, uniform_bufs);
        DEALLOC_BLOCK(typename API::BUF_S, shader_bufs);
        DEALLOC_BLOCK(typename API::BUF_P, pixel_bufs);
        DEALLOC_BLOCK(typename API::BUF_DRAW, indirect_bufs);

#undef DEALLOC_BLOCK
    }

    ApiStore<typename API::V_DESC> vertex_desc;
    ApiStore<PipelineData>         pipelines;

    ApiStore<typename API::BUF_A>    array_bufs;
    ApiStore<typename API::BUF_E>    element_bufs;
    ApiStore<typename API::BUF_U>    uniform_bufs;
    ApiStore<typename API::BUF_S>    shader_bufs;
    ApiStore<typename API::BUF_P>    pixel_bufs;
    ApiStore<typename API::BUF_DRAW> indirect_bufs;

    ApiStore<RHI::GraphicsAPI::SurfaceBase> surfaces;
    ApiStore<RHI::GraphicsAPI::Sampler>     samplers;
};

namespace detail {

template<
    typename BufT,
    typename API,
    typename... Args,
    typename std::enable_if<
        std::is_same<BufT, typename API::BUF_A>::value>::type* = nullptr>
ShPtr<BufT> allocate_buffer(AllocatorData<API>& data, Args... args)
{
    data.array_bufs.push_back(MkShared<BufT>(std::forward<Args>(args)...));
    return data.array_bufs.back();
}

template<
    typename BufT,
    typename API,
    typename... Args,
    typename std::enable_if<
        std::is_same<BufT, typename API::BUF_E>::value>::type* = nullptr>
ShPtr<BufT> allocate_buffer(AllocatorData<API>& data, Args... args)
{
    data.element_bufs.push_back(MkShared<BufT>(std::forward<Args>(args)...));
    return data.element_bufs.back();
}

template<
    typename BufT,
    typename API,
    typename... Args,
    typename std::enable_if<
        std::is_same<BufT, typename API::BUF_U>::value>::type* = nullptr>
ShPtr<BufT> allocate_buffer(AllocatorData<API>& data, Args... args)
{
    data.uniform_bufs.push_back(MkShared<BufT>(std::forward<Args>(args)...));
    return data.uniform_bufs.back();
}

template<
    typename BufT,
    typename API,
    typename... Args,
    typename std::enable_if<
        std::is_same<BufT, typename API::BUF_S>::value>::type* = nullptr>
ShPtr<BufT> allocate_buffer(AllocatorData<API>& data, Args... args)
{
    data.shader_bufs.push_back(MkShared<BufT>(std::forward<Args>(args)...));
    return data.shader_bufs.back();
}

template<
    typename BufT,
    typename API,
    typename... Args,
    typename std::enable_if<
        std::is_same<BufT, typename API::BUF_P>::value>::type* = nullptr>
ShPtr<BufT> allocate_buffer(AllocatorData<API>& data, Args... args)
{
    data.pixel_bufs.push_back(MkShared<BufT>(std::forward<Args>(args)...));
    return data.pixel_bufs.back();
}

template<
    typename BufT,
    typename API,
    typename... Args,
    typename std::enable_if<
        std::is_same<BufT, typename API::BUF_DRAW>::value>::type* = nullptr>
ShPtr<BufT> allocate_buffer(AllocatorData<API>& data, Args... args)
{
    data.indirect_bufs.push_back(MkShared<BufT>(std::forward<Args>(args)...));
    return data.indirect_bufs.back();
}

} // namespace detail

template<typename API>
using AllocatorTag = Coffee::Components::TagType<AllocatorData<API>>;

template<typename API>
struct GraphicsAllocator
    : Coffee::Components::Globals::ValueSubsystem<AllocatorTag<API>>
{
    using AllocData = AllocatorData<API>;

    template<typename... Args>
    GraphicsAllocator(typename API::OPTS const& opts, Args... args)
    {
        auto& context = this->get().context;

        context = API::GetLoadAPI(opts);

        if(!context(std::forward<Args>(args)...))
        {
            Throw(undefined_behavior("failed to initialize RHI"));
        }
    }

    template<size_t NumAttribs>
    ShPtr<typename API::V_DESC> alloc_desc(
        Array<typename API::V_ATTR, NumAttribs>&& attribs)
    {
        auto& store = this->get().vertex_desc;
        store.push_back(MkShared<typename API::V_DESC>());
        auto& vao = store.back();

        vao->alloc();

        for(auto const& attrib : attribs)
        {
            vao->addAttribute(attrib);
        }

        return vao;
    }

    template<size_t NumShaders = 2>
    typename AllocData::PipelineParams& alloc_standard_pipeline(
        Array<Bytes, NumShaders>&& shaders)
    {
        auto& store = this->get().pipelines;
        store.push_back(MkShared<typename AllocData::PipelineData>());

        auto& pipeline = *store.back();

        if(!RHI::LoadPipeline<API>(
               pipeline.pipeline, std::move(shaders[0]), std::move(shaders[1])))
            Throw(
                undefined_behavior("shader compile/pipeline assembly failed"));

        pipeline.params.get_pipeline_params();
        pipeline.params.get_state();

        return pipeline.params;
    }

    template<typename BUF_Type, typename... Args>
    ShPtr<BUF_Type> alloc_buffer(Args... args)
    {
        auto buf_hnd = detail::allocate_buffer<BUF_Type, API, Args...>(
            this->get(), std::forward<Args>(args)...);

        buf_hnd->alloc();

        return buf_hnd;
    }

    template<typename SUF_Type, typename... Args>
    Tup<ShPtr<typename SUF_Type::sampler_type>, ShPtr<SUF_Type>>
    alloc_surface_sampler(Args... args)
    {
        auto surf_hnd = MkShared<SUF_Type>(std::forward<Args>(args)...);
        auto samp_hnd = MkShared<typename SUF_Type::sampler_type>();

        auto& store         = this->get().surfaces;
        auto& sampler_store = this->get().samplers;

        store.push_back(surf_hnd);
        sampler_store.push_back(samp_hnd);

        samp_hnd->alloc();
        samp_hnd->attach(surf_hnd.get());

        return std::make_tuple(samp_hnd, surf_hnd);
    }
};

} // namespace Components
} // namespace RHI
} // namespace Coffee
