#pragma once

namespace gleam {

enum class error
{
    none,

    no_program,

    refuse_bad_es20_impl,
    refuse_version_too_low,

    sampler_state_invalid,

    draw_no_element_buffer,
    draw_no_elements,
    draw_no_arrays,
    draw_unsupported_call,
    no_implementation_for_draw_call,
};

struct features
{
    struct buffers
    {
        bool dsa{false};
        bool mapping{true};
        bool persistence{false};
        bool storage{false};

        struct
        {
            bool mapbuffer;
        } oes;
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
            bool shader_draw_parameters;
        } arb;
    };
    struct programs
    {
        bool dsa{false};
        bool separable_programs{false};
        bool uniform_location{false};
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
        bool framebuffer_texture{true};
        bool indexed{false};
        bool readdraw_buffers{true};

        struct
        {
            bool discard_framebuffer{false};
        } ext;
    };
    struct textures
    {
        bool anisotropy{false};
        bool bindless_handles{false};
        bool dsa{false};
        bool multibind{false};
        bool samplers{true};
        bool storage{false};
        bool texture_3d{true};
        bool tex_layer_query{true};
        bool views{false};

        struct
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
                bool dxt1{false};
                bool dxt3{false};
                bool dxt5{false};
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
        bool vertex_arrays{true};
        struct
        {
            bool vertex_arrays{false};
        } oes;
    };

    buffers       buffer{};
    drawing       draw{};
    programs      program{};
    queries       query{};
    rendertargets rendertarget{};
    textures      texture{};
    vertices      vertex{};

    bool khr_debug{false};
};

struct workarounds
{
    struct
    {
        bool emulated_instance_id{false};
        bool emulated_base_instance{false};
        bool emulated_vertex_offset{false};
    } draw;

    bool ignore_all_warnings{false};
};

} // namespace gleam
