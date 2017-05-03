#pragma once

#include "gleam_types_rhi.h"
#include "gleam_framebuffer_rhi.h"
#include "gleam_api_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_DataStore
{
    ~GLEAM_DataStore()
    {
        fprintf(stderr, "I'M DYING BLARGH\n");
    }

    GLEAM_Instance_Data* inst_data;
    GLEAM_API::FB_T DefaultFramebuffer;

    APILevel CURR_API = GL_Nothing;

    #ifndef NDEBUG
    bool DEBUG_MODE = false;
    #else
    static const constexpr bool DEBUG_MODE = false;
    #endif
};

}
}
}
