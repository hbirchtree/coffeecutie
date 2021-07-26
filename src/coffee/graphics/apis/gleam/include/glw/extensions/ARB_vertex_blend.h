#pragma once

#ifdef GL_ARB_vertex_blend
namespace gl::arb::vertex_blend {
namespace values {
constexpr libc_types::u32 modelview0           = 0x1700;
constexpr libc_types::u32 modelview1           = 0x850A;
constexpr libc_types::u32 max_vertex_units     = 0x86A4;
constexpr libc_types::u32 active_vertex_units  = 0x86A5;
constexpr libc_types::u32 weight_sum_unity     = 0x86A6;
constexpr libc_types::u32 vertex_blend         = 0x86A7;
constexpr libc_types::u32 current_weight       = 0x86A8;
constexpr libc_types::u32 weight_array_type    = 0x86A9;
constexpr libc_types::u32 weight_array_stride  = 0x86AA;
constexpr libc_types::u32 weight_array_size    = 0x86AB;
constexpr libc_types::u32 weight_array_pointer = 0x86AC;
constexpr libc_types::u32 weight_array         = 0x86AD;
constexpr libc_types::u32 modelview2           = 0x8722;
constexpr libc_types::u32 modelview3           = 0x8723;
constexpr libc_types::u32 modelview4           = 0x8724;
constexpr libc_types::u32 modelview5           = 0x8725;
constexpr libc_types::u32 modelview6           = 0x8726;
constexpr libc_types::u32 modelview7           = 0x8727;
constexpr libc_types::u32 modelview8           = 0x8728;
constexpr libc_types::u32 modelview9           = 0x8729;
constexpr libc_types::u32 modelview10          = 0x872A;
constexpr libc_types::u32 modelview11          = 0x872B;
constexpr libc_types::u32 modelview12          = 0x872C;
constexpr libc_types::u32 modelview13          = 0x872D;
constexpr libc_types::u32 modelview14          = 0x872E;
constexpr libc_types::u32 modelview15          = 0x872F;
constexpr libc_types::u32 modelview16          = 0x8730;
constexpr libc_types::u32 modelview17          = 0x8731;
constexpr libc_types::u32 modelview18          = 0x8732;
constexpr libc_types::u32 modelview19          = 0x8733;
constexpr libc_types::u32 modelview20          = 0x8734;
constexpr libc_types::u32 modelview21          = 0x8735;
constexpr libc_types::u32 modelview22          = 0x8736;
constexpr libc_types::u32 modelview23          = 0x8737;
constexpr libc_types::u32 modelview24          = 0x8738;
constexpr libc_types::u32 modelview25          = 0x8739;
constexpr libc_types::u32 modelview26          = 0x873A;
constexpr libc_types::u32 modelview27          = 0x873B;
constexpr libc_types::u32 modelview28          = 0x873C;
constexpr libc_types::u32 modelview29          = 0x873D;
constexpr libc_types::u32 modelview30          = 0x873E;
constexpr libc_types::u32 modelview31          = 0x873F;
} // namespace values
STATICINLINE void vertex_blend(i32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexBlendARB)
    }
    glVertexBlendARB(count);
    detail::error_check("VertexBlendARB"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void weight_pointer(
        i32                            size,
        group::weight_pointer_type_arb type,
        i32                            stride,
        span_const_void const&         pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightPointerARB)
    }
    glWeightPointerARB(
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("WeightPointerARB"sv);
}

template<class span_const_i8>
requires(semantic::concepts::Span<span_const_i8>&& std::is_same_v<
         std::decay_t<typename span_const_i8::value_type>,
         std::decay_t<i8>>) STATICINLINE
    void weightbv(span_const_i8 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightbvARB)
    }
    glWeightbvARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLbyte*>(weights.data())
                       : nullptr);
    detail::error_check("WeightbvARB"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void weightdv(span_const_f64 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightdvARB)
    }
    glWeightdvARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLdouble*>(weights.data())
                       : nullptr);
    detail::error_check("WeightdvARB"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void weightfv(span_const_f32 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightfvARB)
    }
    glWeightfvARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLfloat*>(weights.data())
                       : nullptr);
    detail::error_check("WeightfvARB"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void weightiv(span_const_i32 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightivARB)
    }
    glWeightivARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLint*>(weights.data())
                       : nullptr);
    detail::error_check("WeightivARB"sv);
}

template<class span_const_i16>
requires(semantic::concepts::Span<span_const_i16>&& std::is_same_v<
         std::decay_t<typename span_const_i16::value_type>,
         std::decay_t<i16>>) STATICINLINE
    void weightsv(span_const_i16 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightsvARB)
    }
    glWeightsvARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLshort*>(weights.data())
                       : nullptr);
    detail::error_check("WeightsvARB"sv);
}

template<class span_const_u8>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>) STATICINLINE
    void weightubv(span_const_u8 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightubvARB)
    }
    glWeightubvARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLubyte*>(weights.data())
                       : nullptr);
    detail::error_check("WeightubvARB"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void weightuiv(span_const_u32 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightuivARB)
    }
    glWeightuivARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLuint*>(weights.data())
                       : nullptr);
    detail::error_check("WeightuivARB"sv);
}

template<class span_const_u16>
requires(semantic::concepts::Span<span_const_u16>&& std::is_same_v<
         std::decay_t<typename span_const_u16::value_type>,
         std::decay_t<u16>>) STATICINLINE
    void weightusv(span_const_u16 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightusvARB)
    }
    glWeightusvARB(
        weights.size(),
        weights.size() ? reinterpret_cast<const GLushort*>(weights.data())
                       : nullptr);
    detail::error_check("WeightusvARB"sv);
}

} // namespace gl::arb::vertex_blend
#endif // GL_ARB_vertex_blend
namespace gl::arb::vertex_blend {
constexpr auto name = "GL_ARB_vertex_blend";
}
