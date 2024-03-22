#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/enum/pixels/format.h>

namespace gleam {

enum class error
{
    none,

    no_program,
    no_data,

    async_shader_compile_failed,
    program_error_state,

    refuse_bad_es20_impl,
    refuse_version_too_low,
    no_emulation_native_version_low,
    no_emulation_compiled_version_low,

    sampler_state_invalid,

    draw_no_element_buffer,
    draw_no_elements,
    draw_no_arrays,
    draw_too_many_elements,
    draw_too_many_vertices,
    draw_unsupported_call,
    no_implementation_for_draw_call,
};

struct features
{
    struct buffers
    {
        bool barrier{false};

        bool dsa{false};
        bool invalidate{false};
        bool mapping{true};
        bool pbo{true};
        bool persistence{false};
        bool storage{false};
        bool ssbo{false};
        bool ubo{false};

        struct
        {
            bool mapbuffer{false};
        } oes;

        struct
        {
            bool invalidate{false};
        } arb;
    };

    struct debugging
    {
        bool debug{false};

        struct
        {
            bool debug{false};
        } khr;

        struct
        {
            bool unmasked_vendors{false};
            bool debug_shaders{false};
        } webgl;
    };

    struct drawing
    {
        bool instancing{false};
        bool vertex_offset{false};
        bool indirect{false};
        bool multi_indirect{false};
        bool base_instance{false};
        bool shader_base_instance{false};

        struct
        {
            bool shader_draw_parameters{false};
        } arb;
    };

    struct programs
    {
        bool dsa{false};
        bool separable_programs{false};
        bool uniform_location{false};
        bool compute{false};
        bool spirv{false};

        bool buffer_binding{true};

        struct
        {
            bool parallel_shader_compile{false};
        } khr;
    };

    struct queries
    {
        bool disjoint_timer_query{false};
        bool dsa{false};
    };

    struct rendertargets
    {
        bool clearbuffer{true};
        bool dsa{false};
        bool framebuffer_parameter{false};
        bool framebuffer_texture{false};
        bool indexed{false};
        bool readdraw_buffers{true};

        /* Format support */
        bool color_buffer_float{false};
        bool color_buffer_10bit{false}; // GL_RGB10_A2
        bool color_buffer_half_float{false};
        bool color_buffer_11f_11f_10f{false}; // GL_R11F_G11F_B10F
        bool depth24{false};
        bool depth24_stencil8{false};
        bool depth32{false};
        bool depth32f{false};

        /* Specify these for simple selection of rendertarget formats */
        typing::pixels::PixFmt high_precision_color_format{};
        typing::pixels::PixFmt med_precision_color_format{};
        typing::pixels::PixFmt high_precision_single_format{};
        typing::pixels::PixFmt high_precision_depth_format{};
        typing::pixels::PixFmt low_precision_color_format{};
        typing::pixels::PixFmt low_precision_single_format{};
        typing::pixels::PixFmt low_precision_depth_format{};

        struct
        {
            bool discard_framebuffer{false};
        } ext;

        struct
        {
            bool shading_rate_image{false};
        } nv;
    };

    struct textures
    {
        bool anisotropy{false};
        bool bindless_handles{false};
        bool cube_array{false};
        bool dsa{false};
        bool image_copy{false};
        bool image_texture{false};
        bool internal_format_query{false};
        bool max_level{false};
        bool multibind{false};
        bool samplers{true};
        bool sampler_binding{false};
        bool storage{false};
        bool swizzle{true};
        bool texture_3d{true};
        bool tex_layer_query{true};
        bool views{false};

        struct texture_support_t
        {
            struct
            {
                bool astc{false};
                bool astc_hdr{false};
            } khr;

            struct
            {
                bool rgtc{false};
                bool bptc{false};
            } arb;

            struct
            {
                bool s3tc{false};
                bool rgtc{false};
                bool bptc{false};
            } ext;

            struct
            {
                bool s3tc{false};
            } angle;

            struct
            {
                bool pvrtc{false};
                bool pvrtc2{false};
            } img;

            struct
            {
                bool etc1{false};
                bool rgba8{false};
            } oes;

            struct
            {
                bool etc2{false};
                bool bptc{false};
                bool rgtc{false};
                bool astc{false};
            } gl;
        } tex;

        texture_support_t software_decoded{};

        struct
        {
            bool texture_view{false};
        } arb;

        struct
        {
            bool texture_view{false};
            bool texture_anisotropic{false};
        } ext;

        struct
        {
            bool texture_view{false};
            bool texture_3d{false};
        } oes;
    };

    struct vertices
    {
        bool attribute_binding{true};
        bool dsa{false};
        bool format{false};
        bool layout_binding{false};
        bool vertex_arrays{true};
        bool vertex_offset{true};

        struct
        {
            bool vertex_arrays{false};
        } oes;
    };

    buffers       buffer{};
    debugging     debug{};
    drawing       draw{};
    programs      program{};
    queries       query{};
    rendertargets rendertarget{};
    textures      texture{};
    vertices      vertex{};
};

struct workarounds
{
    struct
    {
        bool emulated_instance_id{false};
        bool emulated_base_instance{false};
        bool emulated_vertex_offset{false};
        bool force_vertex_attrib_names{false};
        bool advance_ubos_by_baseinstance{false};
    } draw;

    struct
    {
        bool emulated_mapbuffer{false};
        bool slow_mapbuffer{false};
        bool disable_immutable_buffers{false};
    } buffer;

    struct
    {
        bool adreno_3xx; /*!< Adreno 3xx drivers seemingly
                          *   can't handle complex shaders */
    } bugs;

    bool ignore_all_warnings{false};
};

using libc_types::u32;
using libc_types::u64;

struct usage
{
    struct
    {
        u32 draws{0};
        u32 instances{0};

        u64 triangles{0};
        u64 triangle_strips{0};
        u64 other_prims{0};

        u32 failed_draws{0};
    } draw;

    struct
    {
        u32 uploads{0};
        u32 mappings{0};
        u64 upload_data{0};
        u64 mapped_data{0};
    } buffers;

    struct
    {
        u32 texture_uploads{0};
    } texture;

    struct
    {
        u32 buffers{0};
        u64 buffer_size{0};
        u32 texture_binds{0};
        u32 values{0};
        u64 value_size{0};
    } uniforms;
};

} // namespace gleam
