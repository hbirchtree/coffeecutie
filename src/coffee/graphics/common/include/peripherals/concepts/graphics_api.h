#pragma once

#include <concepts>
#include <peripherals/concepts/span.h>
#include <peripherals/concepts/vector.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/typing/enum/graphics/buffer_types.h>
#include <peripherals/typing/enum/graphics/shader_stage.h>
#include <peripherals/typing/enum/pixels/filtering.h>
#include <peripherals/typing/enum/pixels/format_transform.h>
#include <peripherals/typing/enum/pixels/wrapping.h>
#include <peripherals/typing/geometry/rect.h>
#include <peripherals/typing/geometry/size.h>
#include <peripherals/typing/vectors/vector_types.h>
#include <type_traits>

namespace semantic::concepts::graphics {

using ::libc_types::f32;
using ::libc_types::f64;
using ::libc_types::i32;
using ::libc_types::i64;
using ::libc_types::u32;
using ::libc_types::u64;

namespace buffers {

enum class type
{
    /* mesh data */
    vertex  = 0x10,
    element = 0x11,

    /* housekeeping */
    pixels = 0x20,
    query  = 0x21,

    /* shader data */
    atomic          = 0x40,
    constants       = 0x41,
    shader_writable = 0x42,

    /* dispatch data */
    compute = 0x80,
    draw    = 0x81,

    vertex_type       = 0x10,
    housekeeping_type = 0x20,
    shader_type       = 0x40,
    dispatch_type     = 0x80,

    pixel_unpack = pixels,
    pixel_pack   = pixels | 0x100,
};

C_FLAGS(type, u32);

enum class property
{
    none,
    pixel_pack = 0x1,
    immutable  = 0x2,
};

C_FLAGS(property, u32);

template<type v>
struct buffer_type
{
    static constexpr auto value = v;
};

using type_vertex          = buffer_type<type::vertex>;
using type_element         = buffer_type<type::element>;
using type_pixel_unpack    = buffer_type<type::pixels>;
using type_pixel_pack      = buffer_type<type::pixels | type::pixel_pack>;
using type_query           = buffer_type<type::query>;
using type_atomic          = buffer_type<type::atomic>;
using type_constants       = buffer_type<type::constants>;
using type_shader_writable = buffer_type<type::shader_writable>;
using type_compute         = buffer_type<type::compute>;
using type_draw            = buffer_type<type::draw>;

constexpr type_vertex  vertex;
constexpr type_element element;

constexpr type_pixel_unpack pixel_unpack;
constexpr type_pixel_pack   pixel_pack;
constexpr type_query        query;

constexpr type_atomic          atomic;
constexpr type_constants       constants;
constexpr type_shader_writable shader_writable;

constexpr type_compute compute;
constexpr type_draw    draw;

enum attribute_flags
{
    none       = 0x0,
    packed     = 0x1,
    normalized = 0x2,
    instanced  = 0x4,
};

C_FLAGS(attribute_flags, u32);

} // namespace buffers

namespace textures {

enum class property
{
    none,
    immutable       = 0x1,
    view            = 0x2,
    generate_mipmap = 0x4,
};
C_FLAGS(property, u32);

enum class type
{
    d2   = 0x1,
    d3   = 0x2,
    cube = 0x3,

    array       = 0x4,
    multisample = 0x8,

    d2_array   = d2 | array,
    cube_array = cube | array,

    type_mask     = 0b0111,
    modifier_mask = 0b1000,

};
C_FLAGS(type, u32);

enum class swizzle_t
{
    red,
    green,
    blue,
    alpha,
    one,
    zero,
};

template<type v>
struct texture_type
{
    static constexpr auto value = v;
};

using type_d2         = texture_type<type::d2>;
using type_d2_array   = texture_type<type::d2_array>;
using type_d3         = texture_type<type::d3>;
using type_cube_array = texture_type<type::cube_array>;

constexpr texture_type<type::d2>         d2;
constexpr texture_type<type::d2_array>   d2_array;
constexpr texture_type<type::d3>         d3;
constexpr texture_type<type::cube_array> cube_array;

static_assert(
    (type::d2_array & type::type_mask) == type::d2_array,
    "d2_array not matched");

enum class sample_properties
{
    edge_policy,
    lod_range,
    lod_bias,
    filtering,
    compare_mode,
    anisotropic,
};

enum class filter_distance
{
    min_,
    mag,
};

enum class compare_mode
{
    none,
    texture,
};

struct view_params
{
    struct
    {
        i32 min_ = 0, count = -1;
    } layer;

    struct
    {
        i32 min_ = 0, count = -1;
    } mip;

    typing::pixels::PixDesc format;
};

} // namespace textures

namespace render_targets {

enum class attachment
{
    color   = 0x1,
    depth   = 0x2,
    stencil = 0x4,

    /* Framebuffer attachments */
    front = 0x8,
    back  = 0x10,

    depth_stencil = depth | stencil,
};
C_FLAGS(attachment, u32);

} // namespace render_targets

namespace queries {

enum class type
{
    time,
    fragments,
    timestamp,
};

template<type v>
struct query_type
{
    static constexpr auto value = v;
};

using type_time      = query_type<type::time>;
using type_fragments = query_type<type::fragments>;

constexpr type_time      time;
constexpr type_fragments fragments;

} // namespace queries

namespace drawing {

enum class primitive
{
    point    = 0x1,
    line     = 0x2,
    triangle = 0x4,
    strip    = 0x10,
    fan      = 0x20,

    line_strip     = line | strip,
    triangle_strip = triangle | strip,
    triangle_fan   = triangle | fan,

    primitive_mask    = point | line | triangle,
    construction_mask = strip | fan,
};

C_FLAGS(primitive, u32);

} // namespace drawing

namespace programs {

enum class shader_format_t
{
    source,
    spv,
};

}

template<class T>
concept Texture =
    requires(T v) {
        {
            v.alloc(std::declval<typing::geometry::size_3d<u32>>())
        };
        {
            v.dealloc()
        };

        {
            v.upload(
                std::declval<gsl::span<int, gsl::dynamic_extent>>(),
                std::declval<typing::vector_types::Vecui2>(),
                std::declval<typing::geometry::size_3d<u32>>(),
                0)
        };
        {
            v.size()
        };

        {
            v.view(textures::d2, std::declval<textures::view_params>())
        };
    } &&
    //    (Size2D<stl_types::mem_function_traits(&T::size)::result_type, u32>/*
    //    || Size3D<declmemtype2(T, size), u32>*/);
    true;

template<class T>
concept Sampler = requires(T v) {
    {
        v.alloc()
    };
    {
        v.dealloc()
    };

    /* Changing properties */
    {
        v.set(textures::sample_properties::anisotropic, f32())
    };
    {
        v.set(
            textures::sample_properties::edge_policy,
            0,
            typing::WrapPolicy::Clamp)
    };
    {
        v.set(
            textures::sample_properties::compare_mode,
            textures::compare_mode::texture)
    };
    {
        v.set(
            textures::sample_properties::filtering,
            textures::filter_distance::min_,
            typing::Filtering::Linear)
    };

    /* For shader pipeline */
    {
        v.handle()
    };
};

template<class T>
concept BufferSlice = requires(T v) {
    {
        v.handle()
    };
    {
        v.buffer()
    };
    std::is_same_v<decltype(v.slice(0)), T>;
};

template<class T, class BufferSliceT>
concept Buffer = BufferSlice<BufferSliceT> && requires(T v) {
    {
        v.alloc()
    };
    {
        v.dealloc()
    };

    /* Population */
    {
        v.commit(100ULL)
    };
    {
        v.commit(
            std::declval<gsl::span<libc_types::i32, gsl::dynamic_extent>>())
    };
    {
        v.commit(
            std::declval<gsl::span<libc_types::f32, gsl::dynamic_extent>>())
    };

    /* Memory ops */
    {
        v.map(0ULL, 10ULL)
    };
    {
        v.unmap(nullptr)
    };
    {
        v.setState(buffers::property::pixel_pack, 1)
    };
    {
        v.size()
    };

    /* For shader pipeline */
    {
        v.handle()
    }; /* Full buffer */
    std::is_same_v<decltype(v.slice(0)), BufferSliceT>;
};

template<class T>
concept Query = requires(T v) {
    {
        v.start()
    };
    {
        v.stop()
    };

    {
        v.resultSync()
    };
    {
        v.result()
    };
};

template<class T, class ShaderT>
concept Pipeline = requires(T v) {
    {
        v.alloc()
    };
    {
        v.dealloc()
    };

    {
        v.attach(
            std::declval<std::shared_ptr<ShaderT>>(),
            typing::graphics::ShaderStage::Vertex)
    };
    {
        v.compile()
    };
};

template<class T>
concept Shader = requires(T v) {
    {
        v.compile(
            typing::graphics::ShaderStage::Vertex,
            std::declval<semantic::mem_chunk<char>>())
    };
};

template<class T, class TextureT>
concept RenderTarget = requires(T v) {
    {
        v.alloc()
    };
    {
        v.dealloc()
    };

    {
        v.attach(
            render_targets::attachment::color, *std::declval<TextureT*>(), 0, 0)
    };
    {
        v.copy(
            *std::declval<T*>(),
            std::declval<typing::geometry::rect<u32>>(),
            std::declval<typing::vector_types::tvec2<u32>>(),
            render_targets::attachment::color,
            render_targets::attachment::color,
            0,
            0,
            0)
    };
    {
        v.resize(std::declval<typing::geometry::rect<i32>>(), 0)
    };
    {
        v.size()
    };

    {
        v.clear(std::declval<typing::vector_types::Vecf4>(), 0)
    };
    {
        v.clear(f64())
    };
    {
        v.clear(i32())
    };
    {
        v.clear(std::declval<typing::vector_types::Vecf4>(), f64(), i32(), 0)
    };
};

template<class T>
concept VertexAttribute = requires(T v) {
    {
        v.index
    };

    /* Value properties */
    {
        v.value.type
    };
    {
        v.value.count
    };
    {
        v.value.size
    };
    {
        v.value.flags
    };
    {
        v.value.stride
    };
    {
        v.value.offset
    };

    /* Buffer properties */
    {
        v.buffer.id
    };
    {
        v.buffer.offset
    };
};

template<class T, class BufferT>
concept VertexArray = requires(T v) {
    {
        v.alloc()
    };
    {
        v.dealloc()
    };

    {
        v.add(std::declval<typename T::attribute_type>())
    };
    {
        v.set_buffer(buffers::vertex, std::shared_ptr<BufferT>(), 0)
    };
    {
        v.set_buffer(buffers::element, std::shared_ptr<BufferT>())
    };
};

template<class T>
concept DrawCommand = requires(T v) {
    {
        v.call.indexed
    };
    {
        v.call.instanced
    };
    {
        v.call.mode
    };
    {
        v.data.elements.count
    };
    {
        v.data.elements.offset
    };
    {
        v.data.elements.type
    };
    {
        v.data.arrays.count
    };
    {
        v.data.arrays.offset
    };
    {
        v.data.instances.count
    };
    {
        v.data.instances.offset
    };

    {
        v.conditional_query
    };
    {
        v.vertices
    };
    {
        v.uniforms
    };
    {
        v.buffers
    };
    {
        v.samplers
    };
};

template<class T>
concept RenderPass = DrawCommand<typename T::command_type> && requires(T v) {
    {
        v.commands
    };
    {
        v.pipeline
    };

    {
        v.framebuffer
    };

    {
        v.state.blend
    };
    {
        v.state.depth
    };
    {
        v.state.pixel
    };
    {
        v.state.raster
    };
    {
        v.state.stencil
    };
    {
        v.state.view
    };
};

template<class T>
concept PipelineInfo = requires(T v) {
    {
        v.uniforms
    };
    {
        v.samplers
    };
    {
        v.buffers
    };
    {
        v.inputs
    };
    {
        v.outputs
    };
};

template<class T>
concept PipelineDump = true;

template<class T, class TextureT, class SamplerT>
concept FramebufferDump = true;

template<class T>
concept DebugScope = true;

template<class T>
concept PerfScope = true;

template<class T>
concept DebugDrawer = true;

template<class T, class APIT>
concept DebugTools = PipelineDump<typename T::pipeline_dump_type> &&
                     FramebufferDump<
                         typename T::render_dump_type,
                         typename APIT::texture_type,
                         typename APIT::sampler_type> &&
                     DebugScope<typename T::debug_scope_type> &&
                     PerfScope<typename T::perf_scope_type> &&
                     DebugDrawer<typename T::debug_draw_type> && requires(T v) {
                         {
                             v.enable()
                         };
                         {
                             v.scope()
                         };
                         {
                             v.perf_scope()
                         };
                     };

template<class T>
concept InfoTools = requires(T v) {
    {
        v.max_of()
    };
};

template<class T>
concept Queue = requires(T v) {
    {
        v.queue(std::declval<std::function<void()>>())
    };
};

template<class T>
concept FixedPipeline =
    Texture<typename T::texture_type> &&
    Buffer<typename T::buffer_type, typename T::buffer_slice_type> &&
    Shader<typename T::shader_type> &&
    Pipeline<typename T::program_type, typename T::shader_type> &&
    RenderTarget<typename T::render_target_type, typename T::texture_type> &&
    requires(T v) {
        {
            v.alloc_buffer(buffers::vertex, semantic::RSCA::None)
        };
        {
            v.alloc_program()
        };
        {
            v.alloc_rendertarget()
        };
        {
            v.alloc_shader()
        };
        {
            v.alloc_texture(
                textures::type::d2_array,
                typing::pixels::PixDesc(),
                1,
                textures::property::none)
        };
        {
            v.alloc_vertex_array()
        };

        {
            v.api_name()
        };
        {
            v.api_version()
        };
    };

template<class T>
concept API = FixedPipeline<T> && Sampler<typename T::sampler_type> &&
              Query<typename T::query_type> &&
              DrawCommand<typename T::draw_command_type> &&
              DebugTools<typename T::debug_type, T> &&
              Queue<typename T::queue_type> && requires(T v) {
                  {
                      v.alloc_query()
                  };

                  {
                      v.debug()
                  };

                  {
                      v.submit(std::declval<typename T::optimized_pass_type>())
                  };
                  {
                      v.submit(std::declval<typename T::render_pass_type>())
                  };

                  {
                      v.shaderlang_name()
                  };
                  {
                      v.shaderlang_version()
                  };
                  {
                      v.device()
                  };
                  {
                      v.device_driver()
                  };

                  {
                      v.default_rendertarget()
                  };
              };

namespace detail {

template<typename T>
concept is_uniform_pair = true
    /*std::is_same_v<decltype(typename T::first_type().name), std::string_view>
    && std::is_same_v<decltype(typename T::first_type().location),
    libc_types::i32> && semantic::concepts::Span<typename T::second_type>*/
    ;

template<typename S, typename... U>
concept is_uniform_stage =
    (std::is_same_v<S, typing::graphics::ShaderStage> &&
     (is_uniform_pair<U> && ...));

} // namespace detail

namespace util {
namespace detail {

template<typename T>
inline void dealloc_single(T& r)
{
    r->dealloc();
}

} // namespace detail

template<typename... T>
inline void dealloc_all(T&... resources)
{
    (..., detail::dealloc_single(resources));
}

} // namespace util
} // namespace semantic::concepts::graphics
