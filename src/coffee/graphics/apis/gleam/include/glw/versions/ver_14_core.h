#ifdef GL_VERSION_1_4
/* Introduced in GL core 1.4 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>) STATICINLINE
    void blend_func_separate(
        group::blending_factor sfactorRGB,
        group::blending_factor dfactorRGB,
        group::blending_factor sfactorAlpha,
        group::blending_factor dfactorAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparate)
    }
    glBlendFuncSeparate(
        static_cast<GLenum>(sfactorRGB),
        static_cast<GLenum>(dfactorRGB),
        static_cast<GLenum>(sfactorAlpha),
        static_cast<GLenum>(dfactorAlpha));
    detail::error_check("BlendFuncSeparate"sv);
}

/* Introduced in GL core 1.4 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<1, 4>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void multi_draw_arrays(
        group::primitive_type mode, span_const_i32 first, span_const_i32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArrays)
    }
    GLsizei drawcount = first.size();
    detail::assert_equal(first.size(), drawcount);
    detail::assert_equal(count.size(), drawcount);
    glMultiDrawArrays(static_cast<GLenum>(mode), first.data(), count.data());
    detail::error_check("MultiDrawArrays"sv);
}

/* Introduced in GL core 1.4 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<1, 4>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void multi_draw_elements(
        group::primitive_type     mode,
        span_const_i32            count,
        group::draw_elements_type type,
        ptroff                    indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElements)
    }
    GLsizei drawcount = count.size();
    detail::assert_equal(count.size(), drawcount);
    glMultiDrawElements(
        static_cast<GLenum>(mode),
        count.data(),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices));
    detail::error_check("MultiDrawElements"sv);
}

/* Introduced in GL core 1.4 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>) STATICINLINE
    void point_parameter(group::point_parameter_name_arb pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterf)
    }
    glPointParameterf(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameterf"sv);
}

/* Introduced in GL core 1.4 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<1, 4>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void point_parameter(
        group::point_parameter_name_arb pname, span_const_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameterfv)
    }
    glPointParameterfv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("PointParameterfv"sv);
}

/* Introduced in GL core 1.4 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>) STATICINLINE
    void point_parameter(group::point_parameter_name_arb pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameteri)
    }
    glPointParameteri(static_cast<GLenum>(pname), param);
    detail::error_check("PointParameteri"sv);
}

/* Introduced in GL core 1.4 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<1, 4>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void point_parameter(
        group::point_parameter_name_arb pname, span_const_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointParameteriv)
    }
    glPointParameteriv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("PointParameteriv"sv);
}

/* Introduced in GL core 1.4 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<1, 4>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void blend_color(vec_4_f32 const& red)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendColor)
    }
    glBlendColor(red.x(), red.y(), red.z(), red.w());
    detail::error_check("BlendColor"sv);
}

/* Introduced in GL core 1.4 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<1, 4>>) STATICINLINE
    void blend_equation(group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquation)
    }
    glBlendEquation(static_cast<GLenum>(mode));
    detail::error_check("BlendEquation"sv);
}

#endif // GL_VERSION_1_4
