#pragma once

#include <malloc.h>
#include <memory>
#include <ogc/gx.h>
#include <ogc/tpl.h>
#include <optional>
#include <peripherals/concepts/graphics_api.h>
#include <peripherals/identify/compiler/unreachable.h>
#include <peripherals/semantic/enum/data_types.h>
#include <peripherals/stl/standard_exceptions.h>
#include <peripherals/typing/geometry/size.h>
#include <peripherals/typing/vectors/vector_types.h>
#include <platforms/gekko/paged_mmap.h>
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
    none   = 0,
    failed = 1, /* draw submission failed; see the log for the real cause */
};

using extensions_set = std::set<std::string>;

/* --- Buffers -------------------------------------------------------------- */

struct buffer_t;

struct buffer_slice_t
{
    buffer_t* m_buffer{nullptr};
    size_t    m_offset{0};
    size_t    m_size{0};

    inline u32 handle() const
    {
        return 0;
    }

    inline buffer_t* buffer() const
    {
        return m_buffer;
    }

    template<typename T = libc_types::u8>
    inline gsl::span<T> map(size_t offset = 0, size_t size = 0);
    buffer_slice_t      slice(size_t offset, size_t size = 0) const;
};

struct buffer_t : std::enable_shared_from_this<buffer_t>
{
    buffer_t(buffers::type type, semantic::RSCA access)
        : m_type(type)
        , m_access(access)
    {
    }

    inline void alloc()
    {
    }

    inline void dealloc()
    {
        m_data.clear();
    }

    inline void commit(size_t size)
    {
        m_data.resize(size);
    }

    template<typename T>
    inline void commit(gsl::span<T> data)
    {
        auto bytes = reinterpret_cast<libc_types::u8 const*>(data.data());
        m_data.assign(bytes, bytes + data.size_bytes());
    }

    /* offset is in BYTES; size is the element COUNT of the returned span (0 =
     * the rest of the buffer). */
    template<typename T = libc_types::u8>
    inline gsl::span<T> map(size_t offset = 0, size_t size = 0)
    {
        if(size == 0)
            size = m_data.size() - offset;
        return gsl::span<T>(
            reinterpret_cast<T*>(m_data.data() + offset), size / sizeof(T));
    }

    inline void unmap(void* = nullptr)
    {
    }

    inline void setState(buffers::property, int)
    {
    }

    inline size_t size() const
    {
        return m_data.size();
    }

    inline u32 handle() const
    {
        return 0;
    }

    inline buffer_slice_t slice(size_t offset, size_t size = 0)
    {
        if(size == 0)
            size = m_data.size() - offset;
        return {this, offset, size};
    }

    std::vector<libc_types::u8> m_data;
    buffers::type               m_type;
    semantic::RSCA              m_access;
};

template<typename T>
inline gsl::span<T> buffer_slice_t::map(size_t offset, size_t size)
{
    if(!m_buffer)
        return {};
    if(size == 0)
        size = m_size;
    return gsl::span<T>(
        m_buffer->m_data.data() + m_offset + offset, size / sizeof(T));
}

inline buffer_slice_t buffer_slice_t::slice(size_t offset, size_t size) const
{
    if(!m_buffer)
        return {};
    if(size == 0)
        size = m_buffer->m_data.size() - m_offset - offset;
    if(size == 0)
        Throw(std::out_of_range("gexxo::buffer_slice_t bounds invalid"));
    return {m_buffer, m_offset + offset, size};
}

/* --- Textures + sampler --------------------------------------------------- */

struct texture_t : std::enable_shared_from_this<texture_t>
{
    using size_type = size_3d<u32>;
    using filtering = typing::Filtering;

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

    inline void dealloc()
    {
    }

    template<typename T>
    inline void upload(
        gsl::span<T> /*data*/,
        Vecui2 /*offset*/,
        size_type const& /*size*/,
        u32 /*level*/ = 0)
    {
    }

    inline size_type size() const
    {
        return m_size;
    }

    template<typename T>
    inline std::shared_ptr<texture_t> view(T, textures::view_params)
    {
        return {};
    }

    /*! Upload to managed buffer
     */
    inline bool upload(gsl::span<libc_types::u8 const>&& data)
    {
        m_tpl_data.insert(m_tpl_data.begin(), data.begin(), data.end());
        if(TPL_OpenTPLFromMemory(
               &m_tpl,
               m_tpl_data.data(),
               static_cast<libc_types::u32>(m_tpl_data.size())) <= 0)
            return false;
        if(TPL_GetTexture(&m_tpl, 0, &m_obj) != 0)
            return false;
        m_loaded = true;
        return true;
    }

    /* Build the GX texture in place from a TPL that already lives in resident,
     * GX-readable memory (e.g. a pinned vmem::mlock region) -- no copy. The
     * GXTexObj points into `mem`, so the caller must keep it alive (and
     * writable: TPL_OpenTPLFromMemory rewrites descriptor offsets in place) for
     * the texture's lifetime. `mem` must be a cached address (GX
     * virtual->physical relies on that), not a page-table window VA. */
    inline bool upload_pinned(platform::file::gekko::vmem::mapping&& mem)
    {
        m_mapped     = std::move(mem);
        auto& mem_   = *m_mapped;
        m_locked     = platform::file::gekko::vmem::pin(mem_.base, mem_.size);
        auto& locked = *m_locked;
        if(TPL_OpenTPLFromMemory(&m_tpl, locked.ptr, locked.size) <= 0)
            return false;
        if(TPL_GetTexture(&m_tpl, 0, &m_obj) != 0)
            return false;
        m_loaded = true;
        return true;
    }

    /* Initialise the GX texture object directly from already-tiled texel data
     * (e.g. a hand-tiled RGB565 lightmap), bypassing TPL. `tiled` must be
     * 32-byte aligned, in the GX tiled layout for `gxfmt`, and kept alive +
     * cache-flushed by the caller for the texture's lifetime. */
    inline void init_raw(
        void* tiled,
        u16   w,
        u16   h,
        u8    gxfmt,
        u8    wrap_s = GX_CLAMP,
        u8    wrap_t = GX_CLAMP,
        u8    maxlod = 0)
    {
        bool const mip = maxlod > 0;
        GX_InitTexObj(
            &m_obj,
            tiled,
            w,
            h,
            gxfmt,
            wrap_s,
            wrap_t,
            mip ? GX_TRUE : GX_FALSE);
        if(mip)
            // tiled buffer holds levels 0..maxlod packed consecutively.
            GX_InitTexObjLOD(
                &m_obj,
                GX_LIN_MIP_LIN,
                GX_LINEAR,
                0.0f,
                static_cast<float>(maxlod),
                0.0f,
                GX_FALSE,
                GX_FALSE,
                GX_ANISO_1);
        else
            GX_InitTexObjFilterMode(&m_obj, GX_LINEAR, GX_LINEAR);
        m_loaded = true;
    }

    textures::type          m_type;
    typing::pixels::PixDesc m_format;
    u32                     m_mipmaps{1};
    size_type               m_size{};

    std::optional<platform::file::gekko::vmem::mapping>       m_mapped{};
    std::optional<platform::file::gekko::vmem::locked_region> m_locked{};

    /* GX texture state (populated by load_tpl). */
    std::vector<libc_types::u8> m_tpl_data;
    TPLFile                     m_tpl{};
    GXTexObj                    m_obj{};
    bool                        m_loaded{false};

    filtering m_filter{filtering::Linear};
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

    inline void alloc()
    {
    }

    inline void dealloc()
    {
    }

    inline void set(textures::sample_properties, f32)
    {
    }

    inline void set(textures::sample_properties, i32, typing::WrapPolicy)
    {
    }

    inline void set(textures::sample_properties, textures::compare_mode)
    {
    }

    inline void set(
        textures::sample_properties,
        textures::filter_distance,
        typing::Filtering)
    {
    }

    inline u32 handle() const
    {
        return 0;
    }

    std::shared_ptr<texture_t> m_source;
};

/* --- Queries -------------------------------------------------------------- */

struct query_t
{
    query_t(queries::type type = queries::type::fragments)
        : m_type(type)
    {
    }

    inline void start()
    {
    }

    inline void stop()
    {
    }

    inline void resultSync()
    {
    }

    inline u64 result()
    {
        return 0;
    }

    queries::type m_type;
};

using null_query_t = query_t;

/* --- Vertex array --------------------------------------------------------- */

struct vertex_attribute
{
    using attribute_flags = buffers::attribute_flags;

    u32 index{0};

    enum role_t
    {
        position,
        texcoord0,
        texmtx0 = texcoord0 + 8,
        normal  = texmtx0 + 8,
        color0,
        color1,
        nbt,
    } role{position};

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

namespace detail {

inline u8 role_to_attr(vertex_attribute const& attr)
{
    if(attr.role >= vertex_attribute::texcoord0 &&
       attr.role < (vertex_attribute::texcoord0 + 8))
        return GX_VA_TEX0 + (attr.role - vertex_attribute::texcoord0);
    if(attr.role >= vertex_attribute::texmtx0 &&
       attr.role < (vertex_attribute::texmtx0 + 8))
        return GX_VA_TEX0MTXIDX + (attr.role - vertex_attribute::texmtx0);
    switch(attr.role)
    {
    case vertex_attribute::position:
        return GX_VA_POS;
    case vertex_attribute::normal:
        return GX_VA_NRM;
    case vertex_attribute::color0:
        return GX_VA_CLR0;
    case vertex_attribute::color1:
        return GX_VA_CLR1;
    case vertex_attribute::nbt:
        return GX_VA_NBT;
    }
    unreachable();
}

inline u8 attr_to_type(vertex_attribute const& attr)
{
    switch(u8(attr.role))
    {
    case vertex_attribute::position:
        return attr.value.count == 2 ? GX_POS_XY : GX_POS_XYZ;
    case vertex_attribute::normal:
        return GX_NRM_XYZ;
    case vertex_attribute::color0:
    case vertex_attribute::color1:
        return attr.value.count == 3 ? GX_CLR_RGB : GX_CLR_RGBA;
    case vertex_attribute::texcoord0:
    case vertex_attribute::texcoord0 + 1:
    case vertex_attribute::texcoord0 + 2:
    case vertex_attribute::texcoord0 + 3:
    case vertex_attribute::texcoord0 + 4:
    case vertex_attribute::texcoord0 + 5:
    case vertex_attribute::texcoord0 + 6:
    case vertex_attribute::texcoord0 + 7:
        return attr.value.count == 2 ? GX_TEX_ST : GX_TEX_S;
    }
    unreachable();
}

inline u32 attr_to_fmt(vertex_attribute const& attr)
{
    switch(attr.role)
    {
    case vertex_attribute::role_t::color0:
    case vertex_attribute::role_t::color1:
        switch(attr.value.type)
        {
        case semantic::type_t::u16:
            return attr.value.count == 4 ? GX_RGBA4 : GX_RGB565;
        case semantic::type_t::u32:
            return GX_RGBA8;
        default:
            return attr.value.count == 3 ? GX_RGB8 : GX_RGBA8;
        }
        break;
    default:
        switch(attr.value.type)
        {
        case semantic::type_t::u8:
            return GX_U8;
        case semantic::type_t::i8:
            return GX_S8;
        case semantic::type_t::u16:
            return GX_U16;
        case semantic::type_t::i16:
            return GX_S16;
        case semantic::type_t::f32:
            return GX_F32;
        default:
            Throw(unimplemented_path("vertex format not supported"));
        }
        break;
    }
    unreachable();
}

} // namespace detail

struct vertex_array_t
{
    using attribute_type = vertex_attribute;

    inline void alloc()
    {
    }

    inline void dealloc()
    {
    }

    inline void add(attribute_type attribute)
    {
        m_attributes.push_back(attribute);
        m_attr_desc[attribute.index] = {
            .attr = detail::role_to_attr(attribute),
            .type = GX_DIRECT, // not used
        };
        m_attr_fmt[attribute.index] = {
            .vtxattr  = detail::role_to_attr(attribute),
            .comptype = detail::attr_to_type(attribute),
            .compsize = detail::attr_to_fmt(attribute),
            .frac     = 0,
        };
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

    std::vector<attribute_type>              m_attributes;
    std::map<u32, std::shared_ptr<buffer_t>> m_vertex_buffers{};
    std::shared_ptr<buffer_t>                m_element_buffer;

    std::array<GXVtxDesc, GX_MAX_VTXDESC_LISTSIZE>       m_attr_desc{};
    std::array<GXVtxAttrFmt, GX_MAX_VTXATTRFMT_LISTSIZE> m_attr_fmt{};
};

/* --- Render target (the clear path) --------------------------------------- */

struct rendertarget_t
{
    inline void alloc()
    {
    }

    inline void dealloc()
    {
    }

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

    void         resize(rect<i32> const& size, u32 = 0);
    size_2d<i32> size();

    /* Real GX work: stages the EFB copy-clear color (painted at swap time). */
    void clear(Vecf4 const& color, u32 i = 0);
    void clear(f64 depth);
    void clear(i32 stencil);
    void clear(Vecf4 const& color, f64 depth, i32 stencil, u32 i = 0);

    rect<i32> m_size{};
    Vecf4     m_clear_color{};
    f64       m_clear_depth{1.0};
};

/* --- Program definition --- */

struct program_t
{
    inline void alloc()
    {
    }

    inline void dealloc()
    {
    }

    struct channel_t
    {
        enum output_t
        {
            color0      = GX_COLOR0,
            color1      = GX_COLOR1,
            alpha0      = GX_ALPHA0,
            alpha1      = GX_ALPHA1,
            color0a0    = GX_COLOR0A0,
            color1a1    = GX_COLOR1A1,
            color_zero  = GX_COLORZERO,
            alpha_bump  = GX_ALPHA_BUMP,
            alpha_bumpn = GX_ALPHA_BUMPN,
            color_null  = GX_COLORNULL,
        };

        enum source_t
        {
            src_reg    = GX_SRC_REG,
            src_vertex = GX_SRC_VTX,
        };

        enum diffuse_func_t
        {
            diffuse_none   = GX_DF_NONE,
            diffuse_signed = GX_DF_SIGNED,
            diffuse_clamp  = GX_DF_CLAMP,
        };

        enum attenuation_func_t
        {
            attenuate_specular  = GX_AF_SPEC,
            attenuate_spotlight = GX_AF_SPOT,
            attenuate_none      = GX_AF_NONE,
        };

        output_t channel{color0a0};
        bool     lighting{false}; // GX_ENABLE/GX_DISABLE
        source_t ambient_src{src_reg};
        source_t diffuse_src{src_reg};
        u8       light_mask{0x0};

        diffuse_func_t     diffuse_function{diffuse_none};
        attenuation_func_t attenuation_function{attenuate_none};
    };

    std::vector<channel_t> channels;

    /* One TEV stage: GX_SetTevOrder + GX_SetTevOp. */
    struct stage_t
    {
        enum id_t
        {
            stage0 = GX_TEVSTAGE0,
            stage1,
            stage2,
            stage3,
            stage4,
            stage5,
            stage6,
            stage7,
            stage8,
            stage9,
            stage10,
            stage11,
            stage12,
            stage13,
            stage14,
            stage15,
        };

        enum op_t
        {
            modulate = GX_MODULATE,
            decal    = GX_DECAL,
            blend    = GX_BLEND,
            replace  = GX_REPLACE,
            pass     = GX_PASSCLR,
            /* Not a GX preset: multiply this stage's texture by the previous
             * stage's output (cprev * texc), for multi-texture passes such as
             * diffuse * lightmap. Configured via explicit TEV combiner inputs.
             */
            modulate_prev = 0xFF,
            /* As modulate_prev but with a 2x output scale -- Halo's
             * double-biased-multiply detail blend (detail grey 0.5 = neutral).
             */
            modulate_prev_2x = 0xFE,
        };

        enum texcoord_t
        {
            texcoord0 = GX_TEXCOORD0,
            texcoord1,
            texcoord2,
            texcoord3,
            texcoord4,
            texcoord5,
            texcoord6,
            texcoord7,
            texcoord_null = GX_TEXCOORDNULL,
        };

        enum texmap_t
        {
            texmap0 = GX_TEXMAP0,
            texmap1,
            texmap2,
            texmap3,
            texmap4,
            texmap5,
            texmap6,
            texmap7,
            texmap_null = GX_TEXMAP_NULL,
        };

        id_t                stage{stage0};
        op_t                op{modulate};
        texcoord_t          texcoord{texcoord_null};
        texmap_t            texmap{texmap_null};
        channel_t::output_t color{
            channel_t::color0a0}; // rasterised colour input
    };

    std::vector<stage_t> stages;
};

/* Binds a texture to a GX texture map and describes how its coordinates are
 * generated. The generation source/type may vary per texture. */
struct texture_binding_t
{
    // texmap/texcoord reuse the TEV stage enums (same GX slots).
    using texmap_t   = program_t::stage_t::texmap_t;
    using texcoord_t = program_t::stage_t::texcoord_t;

    enum gen_type_t // GXTexGenType
    {
        mtx3x4 = GX_TG_MTX3x4,
        mtx2x4 = GX_TG_MTX2x4,
        bump0  = GX_TG_BUMP0,
        srtg   = GX_TG_SRTG,
    };

    enum gen_src_t // GXTexGenSrc (the input -- varies per texture)
    {
        src_position = GX_TG_POS,
        src_normal   = GX_TG_NRM,
        src_color0   = GX_TG_COLOR0,
        src_color1   = GX_TG_COLOR1,
        src_tex0     = GX_TG_TEX0,
        src_tex1,
        src_tex2,
        src_tex3,
        src_tex4,
        src_tex5,
        src_tex6,
        src_tex7,
    };

    std::shared_ptr<texture_t> texture;
    texmap_t                   texmap{program_t::stage_t::texmap0};
    texcoord_t                 texcoord{program_t::stage_t::texcoord0};
    gen_type_t                 gen_type{mtx2x4};
    gen_src_t                  gen_src{src_tex0};
    u32 gen_mtx{GX_IDENTITY}; // texcoord matrix (raw GX matrix id)
};

/* --- Draw command --- */

struct draw_command
{
    struct call_t
    {
        bool               indexed{false};
        bool               instanced{false};
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
    std::shared_ptr<program_t>      program;
    std::shared_ptr<vertex_array_t> vertices;

    std::vector<texture_binding_t> textures;

    gsl::span<Matf4 const> instance_transforms{};
    int                    uniforms{};
    int                    buffers{};
    int                    samplers{};
};

/* A recorded GX display list: the GX_Begin..vertices..GX_End command stream for
 * a draw whose geometry doesn't change frame-to-frame. Built lazily by submit()
 * and keyed by a hash of the draw state, so repeated identical draws replay the
 * cached FIFO via GX_CallDisplayList instead of re-feeding every vertex. The
 * buffer is 32-byte aligned (GX requirement). Move-only RAII. */
struct display_list_t
{
    void* buffer{nullptr};
    u32   size{0}; /* bytes used (multiple of 32) */

    display_list_t() = default;

    display_list_t(void* b, u32 s)
        : buffer(b)
        , size(s)
    {
    }

    display_list_t(display_list_t&& o) noexcept
        : buffer(o.buffer)
        , size(o.size)
    {
        o.buffer = nullptr;
        o.size   = 0;
    }

    display_list_t& operator=(display_list_t&& o) noexcept
    {
        if(this != &o)
        {
            if(buffer)
                free(buffer);
            buffer   = o.buffer;
            size     = o.size;
            o.buffer = nullptr;
            o.size   = 0;
        }
        return *this;
    }

    display_list_t(display_list_t const&)            = delete;
    display_list_t& operator=(display_list_t const&) = delete;

    ~display_list_t()
    {
        if(buffer)
            free(buffer);
    }
};

using typing::vector_types::Vecd2;
using typing::vector_types::Veci4;

struct cull_state
{
    bool front_face{true};
};

struct blend_state
{
    bool additive{false};
    bool multiply{false};
    bool maximum{false};
};

struct depth_state
{
    using depth_range = std::optional<Vecd2>;

    depth_range range{};
    bool        reversed{false};
    bool        strict_greater{false};
};

struct view_state
{
    using rect = std::optional<Veci4>;

    bool indexed{false};
    rect view{};
    rect scissor{};

    depth_state depth;
};

/* --- Concept conformance checks ------------------------------------------- */

static_assert(BufferSlice<buffer_slice_t>, "gexxo::buffer_slice_t");
static_assert(Buffer<buffer_t, buffer_slice_t>, "gexxo::buffer_t");
static_assert(Texture<texture_t>, "gexxo::texture_t");
static_assert(Sampler<sampler_t>, "gexxo::sampler_t");
static_assert(Query<query_t>, "gexxo::query_t");
static_assert(VertexArray<vertex_array_t, buffer_t>, "gexxo::vertex_array_t");
static_assert(RenderTarget<rendertarget_t, texture_t>, "gexxo::rendertarget_t");
static_assert(DrawCommand<draw_command>, "gexxo::draw_command");

} // namespace gexxo
