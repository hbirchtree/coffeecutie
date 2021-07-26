#pragma once

#ifdef GL_ARB_enhanced_layouts
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/ProgramInterface.h"
#include "../enums/ProgramResourceProperty.h"
namespace gl::arb::enhanced_layouts {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::program_interface;
using gl::group::program_resource_property;
namespace values {
} // namespace values
} // namespace gl::arb::enhanced_layouts
#endif // GL_ARB_enhanced_layouts
namespace gl::arb::enhanced_layouts {
constexpr auto name = "GL_ARB_enhanced_layouts";
}
