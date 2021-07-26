#pragma once

#ifdef GL_ARB_compute_variable_group_size
namespace gl::arb::compute_variable_group_size {
namespace values {
constexpr libc_types::u32 max_compute_fixed_group_invocations    = 0x90EB;
constexpr libc_types::u32 max_compute_fixed_group_size           = 0x91BF;
constexpr libc_types::u32 max_compute_variable_group_invocations = 0x9344;
constexpr libc_types::u32 max_compute_variable_group_size        = 0x9345;
} // namespace values
STATICINLINE void dispatch_compute_group_size(
    u32 num_groups_x,
    u32 num_groups_y,
    u32 num_groups_z,
    u32 group_size_x,
    u32 group_size_y,
    u32 group_size_z)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchComputeGroupSizeARB)
    }
    glDispatchComputeGroupSizeARB(
        num_groups_x,
        num_groups_y,
        num_groups_z,
        group_size_x,
        group_size_y,
        group_size_z);
    detail::error_check("DispatchComputeGroupSizeARB"sv);
}

} // namespace gl::arb::compute_variable_group_size
#endif // GL_ARB_compute_variable_group_size
namespace gl::arb::compute_variable_group_size {
constexpr auto name = "GL_ARB_compute_variable_group_size";
}
