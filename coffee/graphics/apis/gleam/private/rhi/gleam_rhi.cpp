#include <coffee/graphics/apis/gleam/rhi/gleam_rhi.h>

#include <coffee/graphics/apis/gleam/rhi/gleam_framebuffer_rhi.h>

#include "gleam_internal_types.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

thread_local GLEAM_Instance_Data* GLEAM_API::instance_data = nullptr;

thread_local GLEAM_API::FB_T GLEAM_API::DefaultFramebuffer = {};
APILevel GL_CURR_API = GL_Nothing;
#ifndef NDEBUG
bool GL_DEBUG_MODE = false;
#endif

}
}
}
