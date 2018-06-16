#pragma once

#include "gleam_types_rhi.h"
#include "gleam_framebuffer_rhi.h"
#include "gleam_api_rhi.h"
#include "gleam_quad_draw.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct InstanceDataDeleter
{
    void operator()(GLEAM_Instance_Data* p);
};

struct GLEAM_DataStore
{
    ~GLEAM_DataStore()
    {
    }

    UqPtr<GLEAM_Instance_Data, InstanceDataDeleter> inst_data;
    GLEAM_API::FB_T DefaultFramebuffer;

    GLEAM_Quad_Drawer debug_drawer;

    APILevel CURR_API = GL_Nothing;

    RHI::GraphicsAPI_Threading::GraphicsQueue GpuThread;

    GLEAM_Options options;

    struct {
        bool gles20 = false;
        bool is_gles = false;

        bool rasterizer_discard = false;
        bool depth_clamp = false;
        bool viewport_indexed = false;

        bool separable_programs = false;

        bool buffer_storage = false;
        bool buffer_persistent = false;

        bool texture_storage = false;

        bool vertex_format = false;
        bool element_buffer_bind = false;

        bool draw_buffers_blend = false;
        bool draw_color_mask = false;
        bool draw_base_instance =  false;
        bool draw_multi_indirect = false;
        bool draw_indirect = false;

#if defined(COFFEE_GLEAM_DESKTOP)
        bool base_instance = false;
        bool direct_state = false;
#else
        bool qcom_tiling = false;
        const bool base_instance = false;
        const bool direct_state = false;
#endif
    } features;

    #ifndef NDEBUG
    bool DEBUG_MODE = false;
    #else
    static const constexpr bool DEBUG_MODE = false;
    #endif
};

}
}
}
