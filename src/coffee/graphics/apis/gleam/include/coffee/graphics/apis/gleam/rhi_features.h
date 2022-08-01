#pragma once

namespace gleam {

enum class error
{
    none,

    refuse_bad_es20_impl,
    refuse_version_too_low,

    draw_no_element_buffer,
    draw_no_elements,
    draw_no_arrays,
    draw_unsupported,
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
            } oes;
            struct
            {
                bool etc2{false};
                bool bptc{false};
                bool rgtc{true};
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
    };

    buffers       buffer{};
    programs      program{};
    queries       query{};
    rendertargets rendertarget{};
    textures      texture{};
    vertices      vertex{};

    bool khr_debug{false};
};

} // namespace gleam
