#pragma once

/* GameCube (GX) implementation of the newer RHI resource types. These mirror
 * the surface of the gleam:: types and satisfy the same concepts from
 * <peripherals/concepts/graphics_api.h>, so GLeamBaseTest_RHI can typedef
 * between gleam:: and gexxo::.
 *
 * For the current milestone (clear screen to a color) only rendertarget_t::clear
 * does real GX work; the other resources are concept-satisfying skeletons that
 * the actual draw/upload paths will fill in later. program_t and api::submit are
 * intentionally absent / out of scope here. */

#include <peripherals/concepts/graphics_api.h>
#include <peripherals/semantic/enum/data_types.h>
#include <peripherals/typing/geometry/size.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <ogc/gx.h>
#include <ogc/tpl.h>

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace gexxo {

/* Bring the shared graphics enums (buffers::, textures::, queries::, drawing::,
 * render_targets::) and the concepts into the gexxo namespace, exactly as
 * gleam does. */
using namespace semantic::concepts::graphics;

using libc_types::f32;
using libc_types::f64;
using libc_types::i32;
using libc_types::u32;
using libc_types::u64;

using typing::vector_types::Matf4;
using typing::vector_types::Vecf4;
using typing::vector_types::Vecui2;

template<typename T>
using size_3d = typing::geometry::size_3d<T>;
template<typename T>
using size_2d = typing::geometry::size_2d<T>;
template<typename T>
using rect = typing::geometry::rect<T>;

enum class api_type_t
{
    none,
    core,
    es,
    webgl,
};

enum class error : u32
{
    none = 0,
};

using extensions_set = std::set<std::string>;

/* --- Buffers -------------------------------------------------------------- */

struct buffer_t;

struct buffer_slice_t
{
    buffer_t* m_buffer{nullptr};
    size_t    m_offset{0};
    size_t    m_size{0};

    inline u32       handle() const { return 0; }
    inline buffer_t* buffer() const { return m_buffer; }
    inline buffer_slice_t slice(size_t offset, size_t size = 0) const
    {
        return {m_buffer, m_offset + offset, size};
    }
};

struct buffer_t : std::enable_shared_from_this<buffer_t>
{
    buffer_t(buffers::type type, semantic::RSCA access)
        : m_type(type)
        , m_access(access)
    {
    }

    inline void alloc() {}
    inline void dealloc() { m_data.clear(); }

    inline void commit(size_t size) { m_data.resize(size); }
    template<typename T>
    inline void commit(gsl::span<T> data)
    {
        auto bytes = reinterpret_cast<libc_types::u8 const*>(data.data());
        m_data.assign(bytes, bytes + data.size_bytes());
    }

    inline libc_types::u8* map(size_t offset, size_t /*size*/)
    {
        return m_data.data() + offset;
    }
    inline void unmap(void* = nullptr) {}
    inline void setState(buffers::property, int) {}

    inline size_t size() const { return m_data.size(); }
    inline u32    handle() const { return 0; }

    inline buffer_slice_t slice(size_t offset, size_t size = 0)
    {
        return {this, offset, size};
    }

    std::vector<libc_types::u8> m_data;
    buffers::type               m_type;
    semantic::RSCA              m_access;
};

/* --- Textures + sampler --------------------------------------------------- */

struct texture_t : std::enable_shared_from_this<texture_t>
{
    using size_type = size_3d<u32>;

    texture_t(textures::type type, typing::pixels::PixDesc fmt, u32 mipmaps)
        : m_type(type)
        , m_format(fmt)
        , m_mipmaps(mipmaps)
    {
    }
    virtual ~texture_t() = default;

    virtual void alloc(size_type const& size, bool /*create_storage*/ = true)
    {
        m_size = size;
    }
    inline void dealloc() {}

    template<typename T>
    inline void upload(
        gsl::span<T> /*data*/,
        Vecui2 /*offset*/,
        size_type const& /*size*/,
        u32 /*level*/ = 0)
    {
    }

    inline size_type size() const { return m_size; }

    template<typename T>
    inline std::shared_ptr<texture_t> view(T, textures::view_params)
    {
        return {};
    }

    inline bool upload(gsl::span<libc_types::u8 const>&& data)
    {
        m_tpl_data.insert(m_tpl_data.begin(), data.begin(), data.end());
        if(TPL_OpenTPLFromMemory(
               &m_tpl,
               m_tpl_data.data(),
               static_cast<libc_types::u32>(m_tpl_data.size()))
           <= 0)
            return false;
        if(TPL_GetTexture(&m_tpl, 0, &m_obj) != 0)
            return false;
        m_loaded = true;
        return true;
    }

    textures::type          m_type;
    typing::pixels::PixDesc m_format;
    u32                     m_mipmaps{1};
    size_type               m_size{};

    /* GX texture state (populated by load_tpl). */
    std::vector<libc_types::u8> m_tpl_data;
    TPLFile                     m_tpl{};
    GXTexObj                    m_obj{};
    bool                        m_loaded{false};
};

struct texture_2d_t : texture_t
{
    using texture_t::texture_t;
};
struct texture_2da_t : texture_t
{
    using texture_t::texture_t;
};
struct texture_3d_t : texture_t
{
    using texture_t::texture_t;
};
struct texture_cube_array_t : texture_t
{
    using texture_t::texture_t;
};

struct sampler_t
{
    sampler_t(std::shared_ptr<texture_t> source = {})
        : m_source(std::move(source))
    {
    }

    inline void alloc() {}
    inline void dealloc() {}

    inline void set(textures::sample_properties, f32) {}
    inline void set(textures::sample_properties, i32, typing::WrapPolicy) {}
    inline void set(textures::sample_properties, textures::compare_mode) {}
    inline void set(
        textures::sample_properties, textures::filter_distance, typing::Filtering)
    {
    }

    inline u32 handle() const { return 0; }

    std::shared_ptr<texture_t> m_source;
};

/* --- Queries -------------------------------------------------------------- */

struct query_t
{
    query_t(queries::type type = queries::type::fragments) : m_type(type) {}

    inline void start() {}
    inline void stop() {}
    inline void resultSync() {}
    inline u64  result() { return 0; }

    queries::type m_type;
};
using null_query_t = query_t;

/* --- Vertex array --------------------------------------------------------- */

struct vertex_attribute
{
    using attribute_flags = buffers::attribute_flags;

    u32 index{0};

    struct
    {
        u64              offset{0};
        u64              stride{0};
        u32              count{4};
        semantic::type_t type{semantic::type_t::f32};
        attribute_flags  flags{attribute_flags::none};
    } value;

    struct
    {
        size_t offset{0};
        u32    id{0};
    } buffer{};
};

struct vertex_array_t
{
    using attribute_type = vertex_attribute;

    inline void alloc() {}
    inline void dealloc() {}

    inline void add(attribute_type attribute)
    {
        m_attributes.push_back(attribute);
    }

    inline void set_buffer(
        buffers::type_vertex, std::shared_ptr<buffer_t> buffer, u32 binding)
    {
        m_vertex_buffers[binding] = std::move(buffer);
    }
    inline void set_buffer(
        buffers::type_element, std::shared_ptr<buffer_t> buffer)
    {
        m_element_buffer = std::move(buffer);
    }

    std::vector<attribute_type>                       m_attributes;
    std::vector<std::shared_ptr<buffer_t>>            m_vertex_buffers{8};
    std::shared_ptr<buffer_t>                         m_element_buffer;
};

/* --- Render target (the clear path) --------------------------------------- */

struct rendertarget_t
{
    inline void alloc() {}
    inline void dealloc() {}

    inline void attach(
        render_targets::attachment,
        texture_t&,
        u32 /*level*/,
        u32 /*i*/     = 0,
        u32 /*layer*/ = 0)
    {
    }

    inline void copy(
        rendertarget_t&,
        rect<u32> /*srcRect*/,
        typing::vector_types::tvec2<u32> /*dstCoord*/,
        render_targets::attachment /*srcAttachment*/,
        render_targets::attachment /*dstAttachment*/,
        u32 /*level*/,
        u32 /*srci*/,
        u32 /*dsti*/)
    {
    }

    void          resize(rect<i32> const& size, u32 = 0);
    size_2d<i32>  size();

    /* Real GX work: stages the EFB copy-clear color (painted at swap time). */
    void clear(Vecf4 const& color, u32 i = 0);
    void clear(f64 depth);
    void clear(i32 stencil);
    void clear(Vecf4 const& color, f64 depth, i32 stencil, u32 i = 0);

    rect<i32> m_size{};
    Vecf4     m_clear_color{};
    f64       m_clear_depth{1.0};
};

/* --- Draw command (concept surface; submit is user-owned) ----------------- */

struct draw_command
{
    struct call_t
    {
        bool              indexed{false};
        bool              instanced{false};
        drawing::primitive mode{drawing::primitive::triangle};
    } call;

    struct data_t
    {
        struct
        {
            u32              count{0};
            u32              offset{0};
            semantic::type_t type{semantic::type_t::u16};
        } elements;
        struct
        {
            u32 count{0};
            u32 offset{0};
        } arrays;
        struct
        {
            u32 count{0};
            u32 offset{0};
        } instances;
    } data;

    std::shared_ptr<query_t>        conditional_query;
    std::shared_ptr<vertex_array_t> vertices;

    std::shared_ptr<texture_t> texture;

    gsl::span<Matf4 const> instance_transforms{};
    int uniforms{};
    int buffers{};
    int samplers{};
};

/* --- Concept conformance checks ------------------------------------------- */

static_assert(BufferSlice<buffer_slice_t>, "gexxo buffer_slice_t");
static_assert(Buffer<buffer_t, buffer_slice_t>, "gexxo buffer_t");
static_assert(Texture<texture_t>, "gexxo texture_t");
static_assert(Sampler<sampler_t>, "gexxo sampler_t");
static_assert(Query<query_t>, "gexxo query_t");
static_assert(VertexArray<vertex_array_t, buffer_t>, "gexxo vertex_array_t");
static_assert(
    RenderTarget<rendertarget_t, texture_t>, "gexxo rendertarget_t");
static_assert(DrawCommand<draw_command>, "gexxo draw_command");

} // namespace gexxo
