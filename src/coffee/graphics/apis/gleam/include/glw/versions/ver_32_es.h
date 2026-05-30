#ifdef GL_ES_VERSION_3_2
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glBlendBarrier. Introduced in GL es 3.2

 * \return void
 */
STATICINLINE void blend_barrier(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendBarrier)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendBarrier);
#endif
    glBlendBarrier();
    detail::error_check("BlendBarrier"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glCopyImageSubData. Introduced in GL es 3.2
 * \param srcName GLuint
 * \param srcTarget GLenum
 * \param srcLevel GLint
 * \param srcX GLint
 * \param srcY GLint
 * \param srcZ GLint
 * \param dstName GLuint
 * \param dstTarget GLenum
 * \param dstLevel GLint
 * \param dstX GLint
 * \param dstY GLint
 * \param dstZ GLint
 * \param srcWidth GLsizei
 * \param srcHeight GLsizei
 * \param srcDepth GLsizei
 * \return void
 */
STATICINLINE void copy_image_sub_data(
    u32                               srcName,
    group::copy_image_sub_data_target srcTarget,
    i32                               srcLevel,
    i32                               srcX,
    i32                               srcY,
    i32                               srcZ,
    u32                               dstName,
    group::copy_image_sub_data_target dstTarget,
    i32                               dstLevel,
    i32                               dstX,
    i32                               dstY,
    i32                               dstZ,
    i32                               srcWidth,
    i32                               srcHeight,
    i32                               srcDepth,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyImageSubData)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glCopyImageSubData,
        srcName,
        srcTarget,
        srcLevel,
        srcX,
        srcY,
        srcZ,
        dstName,
        dstTarget,
        dstLevel,
        dstX,
        dstY,
        dstZ,
        srcWidth,
        srcHeight,
        srcDepth);
#endif
    glCopyImageSubData(
        srcName,
        static_cast<GLenum>(srcTarget),
        srcLevel,
        srcX,
        srcY,
        srcZ,
        dstName,
        static_cast<GLenum>(dstTarget),
        dstLevel,
        dstX,
        dstY,
        dstZ,
        srcWidth,
        srcHeight,
        srcDepth);
    detail::error_check("CopyImageSubData"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glDebugMessageCallback. Introduced in GL es 3.2
 * \param callback GLDEBUGPROC
 * \param userParam const void *
 * \return void
 */
STATICINLINE void debug_message_callback(
    GLDEBUGPROC            callback,
    span_const_void const& userParam,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageCallback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glDebugMessageCallback,
        gsl::span<const char>(
            reinterpret_cast<const char*>(userParam.data()),
            userParam.size_bytes()),
        callback);
#endif
    glDebugMessageCallback(
        callback,
        userParam.size() ? reinterpret_cast<const void*>(userParam.data())
                         : nullptr);
    detail::error_check("DebugMessageCallback"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDebugMessageControl. Introduced in GL es 3.2
 * \param source GLenum
 * \param type GLenum
 * \param severity GLenum
 * \param count GLsizei
 * \param ids const GLuint *
 * \param enabled GLboolean
 * \return void
 */
STATICINLINE void debug_message_control(
    group::debug_source   source,
    group::debug_type     type,
    group::debug_severity severity,
    span_const_u32 const& ids,
    bool                  enabled,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageControl)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glDebugMessageControl,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()),
        source,
        type,
        severity,
        enabled);
#endif
    glDebugMessageControl(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        static_cast<GLenum>(severity),
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageControl"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glDebugMessageInsert. Introduced in GL es 3.2
 * \param source GLenum
 * \param type GLenum
 * \param id GLuint
 * \param severity GLenum
 * \param length GLsizei
 * \param buf const GLchar *
 * \return void
 */
STATICINLINE void debug_message_insert(
    group::debug_source     source,
    group::debug_type       type,
    u32                     id,
    group::debug_severity   severity,
    i32                     length,
    std::string_view const& buf,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageInsert)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDebugMessageInsert, source, type, id, severity, length, buf);
#endif
    glDebugMessageInsert(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        id,
        static_cast<GLenum>(severity),
        length,
        buf.data());
    detail::error_check("DebugMessageInsert"sv, check_errors);
}

template<
    class span_GLchar,
    class span_debug_severity,
    class span_debug_source,
    class span_debug_type,
    class span_i32,
    class span_u32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    concepts::span<span_debug_source> &&
    std::is_same_v<
        std::decay_t<typename span_debug_source::value_type>,
        std::decay_t<group::debug_source>> &&
    concepts::span<span_debug_type> &&
    std::is_same_v<
        std::decay_t<typename span_debug_type::value_type>,
        std::decay_t<group::debug_type>> &&
    concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_debug_severity> &&
    std::is_same_v<
        std::decay_t<typename span_debug_severity::value_type>,
        std::decay_t<group::debug_severity>> &&
    concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetDebugMessageLog. Introduced in GL es 3.2
 * \param count GLuint
 * \param bufSize GLsizei
 * \param sources GLenum *
 * \param types GLenum *
 * \param ids GLuint *
 * \param severities GLenum *
 * \param lengths GLsizei *
 * \param messageLog GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_debug_message_log(
    span_debug_source   sources,
    span_debug_type     types,
    span_u32            ids,
    span_debug_severity severities,
    span_i32            lengths,
    span_GLchar         messageLog,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDebugMessageLog)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetDebugMessageLog,
        gsl::span<char>(
            reinterpret_cast<char*>(sources.data()), sources.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(types.data()), types.size_bytes()),
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(severities.data()),
            severities.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(lengths.data()), lengths.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(messageLog.data()),
            messageLog.size_bytes()));
#endif
    auto out = glGetDebugMessageLog(
        sources.size(),
        messageLog.size(),
        sources.size() ? reinterpret_cast<GLenum*>(sources.data()) : nullptr,
        types.size() ? reinterpret_cast<GLenum*>(types.data()) : nullptr,
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr,
        severities.size() ? reinterpret_cast<GLenum*>(severities.data())
                          : nullptr,
        lengths.size() ? reinterpret_cast<GLsizei*>(lengths.data()) : nullptr,
        messageLog.data());
    detail::error_check("GetDebugMessageLog"sv, check_errors);
    return out;
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetObjectLabel. Introduced in GL es 3.2
 * \param identifier GLenum
 * \param name GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param label GLchar *
 * \return void
 */
STATICINLINE void get_object_label(
    group::object_identifier identifier,
    u32                      name,
    i32&                     length,
    span_GLchar              label,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectLabel)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetObjectLabel,
        gsl::span<char>(
            reinterpret_cast<char*>(label.data()), label.size_bytes()),
        identifier,
        name,
        length);
#endif
    glGetObjectLabel(
        static_cast<GLenum>(identifier),
        name,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectLabel"sv, check_errors);
}

template<class span_GLchar, class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_void> &&
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetObjectPtrLabel. Introduced in GL es 3.2
 * \param ptr const void *
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param label GLchar *
 * \return void
 */
STATICINLINE void get_object_ptr_label(
    span_const_void const& ptr,
    i32&                   length,
    span_GLchar            label,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectPtrLabel)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetObjectPtrLabel,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ptr.data()), ptr.size_bytes()),
        length,
        gsl::span<char>(
            reinterpret_cast<char*>(label.data()), label.size_bytes()));
#endif
    glGetObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectPtrLabel"sv, check_errors);
}

template<class span_void>
requires(MinimumVersion<Current, Version<3, 2>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetPointerv. Introduced in GL es 3.2
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_pointerv(
    group::get_pointerv_prop pname,
    span_void                params,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPointerv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetPointerv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glGetPointerv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointerv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glObjectLabel. Introduced in GL es 3.2
 * \param identifier GLenum
 * \param name GLuint
 * \param length GLsizei
 * \param label const GLchar *
 * \return void
 */
STATICINLINE void object_label(
    group::object_identifier identifier,
    u32                      name,
    i32                      length,
    std::string_view const&  label,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectLabel)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glObjectLabel, identifier, name, length, label);
#endif
    glObjectLabel(static_cast<GLenum>(identifier), name, length, label.data());
    detail::error_check("ObjectLabel"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glObjectPtrLabel. Introduced in GL es 3.2
 * \param ptr const void *
 * \param length GLsizei
 * \param label const GLchar *
 * \return void
 */
STATICINLINE void object_ptr_label(
    span_const_void const&  ptr,
    i32                     length,
    std::string_view const& label,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectPtrLabel)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glObjectPtrLabel,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ptr.data()), ptr.size_bytes()),
        length,
        label);
#endif
    glObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        length,
        label.data());
    detail::error_check("ObjectPtrLabel"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glPopDebugGroup. Introduced in GL es 3.2

 * \return void
 */
STATICINLINE void pop_debug_group(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PopDebugGroup)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPopDebugGroup);
#endif
    glPopDebugGroup();
    detail::error_check("PopDebugGroup"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glPushDebugGroup. Introduced in GL es 3.2
 * \param source GLenum
 * \param id GLuint
 * \param length GLsizei
 * \param message const GLchar *
 * \return void
 */
STATICINLINE void push_debug_group(
    group::debug_source     source,
    u32                     id,
    i32                     length,
    std::string_view const& message,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PushDebugGroup)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPushDebugGroup, source, id, length, message);
#endif
    glPushDebugGroup(static_cast<GLenum>(source), id, length, message.data());
    detail::error_check("PushDebugGroup"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glBlendEquationSeparatei. Introduced in GL es 3.2
 * \param buf GLuint
 * \param modeRGB GLenum
 * \param modeAlpha GLenum
 * \return void
 */
STATICINLINE void blend_equation_separatei(
    u32                            buf,
    group::blend_equation_mode_ext modeRGB,
    group::blend_equation_mode_ext modeAlpha,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparatei)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendEquationSeparatei, buf, modeRGB, modeAlpha);
#endif
    glBlendEquationSeparatei(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparatei"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glBlendEquationi. Introduced in GL es 3.2
 * \param buf GLuint
 * \param mode GLenum
 * \return void
 */
STATICINLINE void blend_equationi(
    u32                            buf,
    group::blend_equation_mode_ext mode,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationi)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendEquationi, buf, mode);
#endif
    glBlendEquationi(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationi"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glBlendFuncSeparatei. Introduced in GL es 3.2
 * \param buf GLuint
 * \param srcRGB GLenum
 * \param dstRGB GLenum
 * \param srcAlpha GLenum
 * \param dstAlpha GLenum
 * \return void
 */
STATICINLINE void blend_func_separatei(
    u32                    buf,
    group::blending_factor srcRGB,
    group::blending_factor dstRGB,
    group::blending_factor srcAlpha,
    group::blending_factor dstAlpha,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparatei)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBlendFuncSeparatei, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
#endif
    glBlendFuncSeparatei(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparatei"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glBlendFunci. Introduced in GL es 3.2
 * \param buf GLuint
 * \param src GLenum
 * \param dst GLenum
 * \return void
 */
STATICINLINE void blend_funci(
    u32                    buf,
    group::blending_factor src,
    group::blending_factor dst,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunci)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBlendFunci, buf, src, dst);
#endif
    glBlendFunci(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    detail::error_check("BlendFunci"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glColorMaski. Introduced in GL es 3.2
 * \param index GLuint
 * \param r GLboolean
 * \param g GLboolean
 * \param b GLboolean
 * \param a GLboolean
 * \return void
 */
STATICINLINE void color_maski(
    u32         index,
    bool        r,
    bool        g,
    bool        b,
    bool        a,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMaski)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glColorMaski, index, r, g, b, a);
#endif
    glColorMaski(index, r, g, b, a);
    detail::error_check("ColorMaski"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glDisablei. Introduced in GL es 3.2
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void disablei(
    group::enable_cap target,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Disablei)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDisablei, target, index);
#endif
    glDisablei(static_cast<GLenum>(target), index);
    detail::error_check("Disablei"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glEnablei. Introduced in GL es 3.2
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void enablei(
    group::enable_cap target,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Enablei)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEnablei, target, index);
#endif
    glEnablei(static_cast<GLenum>(target), index);
    detail::error_check("Enablei"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glIsEnabledi. Introduced in GL es 3.2
 * \param target GLenum
 * \param index GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean is_enabledi(
    group::enable_cap target,
    u32               index,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabledi)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsEnabledi, target, index);
#endif
    auto out = glIsEnabledi(static_cast<GLenum>(target), index);
    detail::error_check("IsEnabledi"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glDrawElementsBaseVertex. Introduced in GL es 3.2
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_elements_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       basevertex,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsBaseVertex)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawElementsBaseVertex, mode, count, type, indices, basevertex);
#endif
    glDrawElementsBaseVertex(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        basevertex);
    detail::error_check("DrawElementsBaseVertex"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glDrawElementsInstancedBaseVertex. Introduced in GL
 * es 3.2
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param instancecount GLsizei
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_elements_instanced_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       instancecount,
    i32                       basevertex,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseVertex)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawElementsInstancedBaseVertex,
        mode,
        count,
        type,
        indices,
        instancecount,
        basevertex);
#endif
    glDrawElementsInstancedBaseVertex(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        basevertex);
    detail::error_check("DrawElementsInstancedBaseVertex"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glDrawRangeElementsBaseVertex. Introduced in GL es 3.2
 * \param mode GLenum
 * \param start GLuint
 * \param end GLuint
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_range_elements_base_vertex(
    group::primitive_type     mode,
    u32                       start,
    u32                       end,
    i32                       count,
    group::draw_elements_type type,
    span_const_void const&    indices,
    i32                       basevertex,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElementsBaseVertex)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glDrawRangeElementsBaseVertex,
        gsl::span<const char>(
            reinterpret_cast<const char*>(indices.data()),
            indices.size_bytes()),
        mode,
        start,
        end,
        count,
        type,
        basevertex);
#endif
    glDrawRangeElementsBaseVertex(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr,
        basevertex);
    detail::error_check("DrawRangeElementsBaseVertex"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glFramebufferTexture. Introduced in GL es 3.2
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void framebuffer_texture(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glFramebufferTexture, target, attachment, texture, level);
#endif
    glFramebufferTexture(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level);
    detail::error_check("FramebufferTexture"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glPrimitiveBoundingBox. Introduced in GL es 3.2
 * \param minX GLfloat
 * \param minY GLfloat
 * \param minZ GLfloat
 * \param minW GLfloat
 * \param maxX GLfloat
 * \param maxY GLfloat
 * \param maxZ GLfloat
 * \param maxW GLfloat
 * \return void
 */
STATICINLINE void primitive_bounding_box(
    f32         minX,
    f32         minY,
    f32         minZ,
    f32         minW,
    f32         maxX,
    f32         maxY,
    f32         maxZ,
    f32         maxW,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveBoundingBox)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glPrimitiveBoundingBox, minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
#endif
    glPrimitiveBoundingBox(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    detail::error_check("PrimitiveBoundingBox"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glGetGraphicsResetStatus. Introduced in GL es 3.2

 * \return GraphicsResetStatus
 */
STATICINLINE group::graphics_reset_status get_graphics_reset_status(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatus)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetGraphicsResetStatus);
#endif
    auto out = glGetGraphicsResetStatus();
    detail::error_check("GetGraphicsResetStatus"sv, check_errors);
    return static_cast<group::graphics_reset_status>(out);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetnUniformfv. Introduced in GL es 3.2
 * \param program GLuint
 * \param location GLint
 * \param bufSize GLsizei
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void getn_uniformfv(
    u32         program,
    i32         location,
    i32         bufSize,
    span_f32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetnUniformfv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        location,
        bufSize);
#endif
    glGetnUniformfv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetnUniformiv. Introduced in GL es 3.2
 * \param program GLuint
 * \param location GLint
 * \param bufSize GLsizei
 * \param params GLint *
 * \return void
 */
STATICINLINE void getn_uniformiv(
    u32         program,
    i32         location,
    i32         bufSize,
    span_i32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetnUniformiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        location,
        bufSize);
#endif
    glGetnUniformiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformiv"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetnUniformuiv. Introduced in GL es 3.2
 * \param program GLuint
 * \param location GLint
 * \param bufSize GLsizei
 * \param params GLuint *
 * \return void
 */
STATICINLINE void getn_uniformuiv(
    u32         program,
    i32         location,
    i32         bufSize,
    span_u32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetnUniformuiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        location,
        bufSize);
#endif
    glGetnUniformuiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformuiv"sv, check_errors);
}

template<class size_2_i32, class span_void, class vec_2_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_void>)
/*!
 * \brief Wraps around glReadnPixels. Introduced in GL es 3.2
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param bufSize GLsizei
 * \param data void *
 * \return void
 */
STATICINLINE void readn_pixels(
    vec_2_i32 const&    x,
    size_2_i32 const&   width,
    group::pixel_format format,
    group::pixel_type   type,
    span_void           data,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadnPixels)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glReadnPixels,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        x,
        width,
        format,
        type);
#endif
    glReadnPixels(
        x[0],
        x[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixels"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glMinSampleShading. Introduced in GL es 3.2
 * \param value GLfloat
 * \return void
 */
STATICINLINE void min_sample_shading(
    f32 value, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MinSampleShading)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMinSampleShading, value);
#endif
    glMinSampleShading(value);
    detail::error_check("MinSampleShading"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glPatchParameteri. Introduced in GL es 3.2
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void patch_parameter(
    group::patch_parameter_name pname,
    i32                         value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PatchParameteri)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPatchParameteri, pname, value);
#endif
    glPatchParameteri(static_cast<GLenum>(pname), value);
    detail::error_check("PatchParameteri"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetSamplerParameterIiv. Introduced in GL es 3.2
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_sampler_parameter_iiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_i32                   params,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetSamplerParameterIiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        sampler,
        pname);
#endif
    glGetSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIiv"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetSamplerParameterIuiv. Introduced in GL es 3.2
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_sampler_parameter_iuiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_u32                   params,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIuiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetSamplerParameterIuiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        sampler,
        pname);
#endif
    glGetSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIuiv"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetTexParameterIiv. Introduced in GL es 3.2
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_tex_parameter_iiv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIiv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexParameterIiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetTexParameterIiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIiv"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetTexParameterIuiv. Introduced in GL es 3.2
 * \param target GLenum
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_tex_parameter_iuiv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_u32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIuiv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexParameterIuiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetTexParameterIuiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIuiv"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glSamplerParameterIiv. Introduced in GL es 3.2
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLint *
 * \return void
 */
STATICINLINE void sampler_parameter_iiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_const_i32 const&      param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glSamplerParameterIiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        sampler,
        pname);
#endif
    glSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIiv"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glSamplerParameterIuiv. Introduced in GL es 3.2
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLuint *
 * \return void
 */
STATICINLINE void sampler_parameter_iuiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_const_u32 const&      param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIuiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glSamplerParameterIuiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        sampler,
        pname);
#endif
    glSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLuint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIuiv"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glTexParameterIiv. Introduced in GL es 3.2
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void tex_parameter_iiv(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIiv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexParameterIiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glTexParameterIiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIiv"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 2>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glTexParameterIuiv. Introduced in GL es 3.2
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLuint *
 * \return void
 */
STATICINLINE void tex_parameter_iuiv(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_u32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIuiv)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexParameterIuiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glTexParameterIuiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIuiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glTexBuffer. Introduced in GL es 3.2
 * \param target GLenum
 * \param internalformat GLenum
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void tex_buffer(
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBuffer)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glTexBuffer, target, internalformat, buffer);
#endif
    glTexBuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TexBuffer"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>)
/*!
 * \brief Wraps around glTexBufferRange. Introduced in GL es 3.2
 * \param target GLenum
 * \param internalformat GLenum
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void tex_buffer_range(
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer,
    GLintptr                     offset,
    GLsizeiptr                   size,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBufferRange)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glTexBufferRange, target, internalformat, buffer, offset, size);
#endif
    glTexBufferRange(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer,
        offset,
        size);
    detail::error_check("TexBufferRange"sv, check_errors);
}

template<class size_3_i32>
requires(
    MinimumVersion<Current, Version<3, 2>> &&
    concepts::size_2d<size_3_i32, i32>)
/*!
 * \brief Wraps around glTexStorage3DMultisample. Introduced in GL es 3.2
 * \param target GLenum
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param fixedsamplelocations GLboolean
 * \return void
 */
STATICINLINE void tex_storage_3d_multisample(
    group::texture_target        target,
    i32                          samples,
    group::sized_internal_format internalformat,
    size_3_i32 const&            width,
    bool                         fixedsamplelocations,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage3DMultisample)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glTexStorage3DMultisample,
        target,
        samples,
        internalformat,
        width,
        fixedsamplelocations);
#endif
    glTexStorage3DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TexStorage3DMultisample"sv, check_errors);
}

#endif // GL_ES_VERSION_3_2
