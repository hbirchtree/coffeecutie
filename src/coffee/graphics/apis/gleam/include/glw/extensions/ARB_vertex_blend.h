#pragma once

#ifdef GL_ARB_vertex_blend
namespace gl::arb::vertex_blend {
namespace values {
constexpr u32 modelview0           = 0x1700;
constexpr u32 modelview1           = 0x850A;
constexpr u32 max_vertex_units     = 0x86A4;
constexpr u32 active_vertex_units  = 0x86A5;
constexpr u32 weight_sum_unity     = 0x86A6;
constexpr u32 vertex_blend         = 0x86A7;
constexpr u32 current_weight       = 0x86A8;
constexpr u32 weight_array_type    = 0x86A9;
constexpr u32 weight_array_stride  = 0x86AA;
constexpr u32 weight_array_size    = 0x86AB;
constexpr u32 weight_array_pointer = 0x86AC;
constexpr u32 weight_array         = 0x86AD;
constexpr u32 modelview2           = 0x8722;
constexpr u32 modelview3           = 0x8723;
constexpr u32 modelview4           = 0x8724;
constexpr u32 modelview5           = 0x8725;
constexpr u32 modelview6           = 0x8726;
constexpr u32 modelview7           = 0x8727;
constexpr u32 modelview8           = 0x8728;
constexpr u32 modelview9           = 0x8729;
constexpr u32 modelview10          = 0x872A;
constexpr u32 modelview11          = 0x872B;
constexpr u32 modelview12          = 0x872C;
constexpr u32 modelview13          = 0x872D;
constexpr u32 modelview14          = 0x872E;
constexpr u32 modelview15          = 0x872F;
constexpr u32 modelview16          = 0x8730;
constexpr u32 modelview17          = 0x8731;
constexpr u32 modelview18          = 0x8732;
constexpr u32 modelview19          = 0x8733;
constexpr u32 modelview20          = 0x8734;
constexpr u32 modelview21          = 0x8735;
constexpr u32 modelview22          = 0x8736;
constexpr u32 modelview23          = 0x8737;
constexpr u32 modelview24          = 0x8738;
constexpr u32 modelview25          = 0x8739;
constexpr u32 modelview26          = 0x873A;
constexpr u32 modelview27          = 0x873B;
constexpr u32 modelview28          = 0x873C;
constexpr u32 modelview29          = 0x873D;
constexpr u32 modelview30          = 0x873E;
constexpr u32 modelview31          = 0x873F;
} // namespace values

/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param count GLint
 * \return void
 */
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
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void weight_pointer(
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
requires(
    concepts::span<span_const_i8> &&
    std::is_same_v<
        std::decay_t<typename span_const_i8::value_type>,
        std::decay_t<i8>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLbyte *
 * \return void
 */
STATICINLINE void weightbv(span_const_i8 const& weights)
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
requires(
    concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLdouble *
 * \return void
 */
STATICINLINE void weightdv(span_const_f64 const& weights)
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
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLfloat *
 * \return void
 */
STATICINLINE void weightfv(span_const_f32 const& weights)
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
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLint *
 * \return void
 */
STATICINLINE void weightiv(span_const_i32 const& weights)
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
requires(
    concepts::span<span_const_i16> &&
    std::is_same_v<
        std::decay_t<typename span_const_i16::value_type>,
        std::decay_t<i16>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLshort *
 * \return void
 */
STATICINLINE void weightsv(span_const_i16 const& weights)
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
requires(
    concepts::span<span_const_u8> &&
    std::is_same_v<
        std::decay_t<typename span_const_u8::value_type>,
        std::decay_t<u8>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLubyte *
 * \return void
 */
STATICINLINE void weightubv(span_const_u8 const& weights)
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
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLuint *
 * \return void
 */
STATICINLINE void weightuiv(span_const_u32 const& weights)
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
requires(
    concepts::span<span_const_u16> &&
    std::is_same_v<
        std::decay_t<typename span_const_u16::value_type>,
        std::decay_t<u16>>)
/*!
 * \brief Part of GL_ARB_vertex_blend
 * \param size GLint
 * \param weights const GLushort *
 * \return void
 */
STATICINLINE void weightusv(span_const_u16 const& weights)
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
