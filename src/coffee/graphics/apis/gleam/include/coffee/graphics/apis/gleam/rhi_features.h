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
        bool persistence{false};
        bool storage{false};
        bool dsa{false};
        bool mapping{false};
        bool es20{false};

        struct
        {
            bool mapbuffer;
        } oes;
    };
    struct programs
    {
        bool separable_programs{false};
        bool dsa{false};
    };
    struct queries
    {
        bool dsa{false};
        bool disjoint_timer_query{false};
    };
    struct rendertargets
    {
        bool dsa{false};
        bool indexed{false};
        bool es20{false};
    };
    struct textures
    {
        bool storage{false};
        bool dsa{false};
        bool views{false};
        bool bindless_handles{false};
        bool es20{false};

        struct
        {
            struct
            {
                bool astc;
                bool astc_hdr;
            } khr;
            struct
            {
                bool rgtc;
                bool bptc;
            } arb;
            struct
            {
                bool s3tc;
                bool rgtc;
                bool bptc;
            } ext;
            struct
            {
                bool dxt1, dxt3, dxt5;
            } angle;
            struct
            {
                bool pvrtc;
                bool pvrtc2;
            } img;
            struct
            {
                bool etc1;
            } oes;
            struct
            {
                bool etc2;
                bool bptc;
                bool rgtc;
            } gl;
        } tex;
        struct
        {
          bool texture_view{false};
        } arb;
        struct
        {
          bool texture_view{false};
        } ext;
        struct
        {
          bool texture_view{false};
          bool texture_3d{false};
        } oes;

    };
    struct vertices
    {
        bool format{false};
        bool es20{false};
        bool dsa{false};
    };

    buffers       buffer{};
    programs      program{};
    queries       query{};
    rendertargets rendertarget{};
    textures      texture{};
    vertices      vertex{};

    bool          khr_debug{false};
    bool          es20{false};
};

} // namespace gleam
