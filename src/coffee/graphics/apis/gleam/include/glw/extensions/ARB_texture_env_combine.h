#pragma once

#ifdef GL_ARB_texture_env_combine
namespace gl::arb::texture_env_combine {
namespace values {
constexpr libc_types::u32 subtract       = 0x84E7;
constexpr libc_types::u32 combine        = 0x8570;
constexpr libc_types::u32 combine_rgb    = 0x8571;
constexpr libc_types::u32 combine_alpha  = 0x8572;
constexpr libc_types::u32 rgb_scale      = 0x8573;
constexpr libc_types::u32 add_signed     = 0x8574;
constexpr libc_types::u32 interpolate    = 0x8575;
constexpr libc_types::u32 constant       = 0x8576;
constexpr libc_types::u32 primary_color  = 0x8577;
constexpr libc_types::u32 previous       = 0x8578;
constexpr libc_types::u32 source0_rgb    = 0x8580;
constexpr libc_types::u32 source1_rgb    = 0x8581;
constexpr libc_types::u32 source2_rgb    = 0x8582;
constexpr libc_types::u32 source0_alpha  = 0x8588;
constexpr libc_types::u32 source1_alpha  = 0x8589;
constexpr libc_types::u32 source2_alpha  = 0x858A;
constexpr libc_types::u32 operand0_rgb   = 0x8590;
constexpr libc_types::u32 operand1_rgb   = 0x8591;
constexpr libc_types::u32 operand2_rgb   = 0x8592;
constexpr libc_types::u32 operand0_alpha = 0x8598;
constexpr libc_types::u32 operand1_alpha = 0x8599;
constexpr libc_types::u32 operand2_alpha = 0x859A;
} // namespace values
} // namespace gl::arb::texture_env_combine
#endif // GL_ARB_texture_env_combine
namespace gl::arb::texture_env_combine {
constexpr auto name = "GL_ARB_texture_env_combine";
}
