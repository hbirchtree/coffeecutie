#ifdef GL_ES_VERSION_3_2
/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void blend_barrier()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendBarrier)
    }
    glBlendBarrier();
    detail::error_check("BlendBarrier"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void copy_image_sub_data(
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
        i32                               srcDepth)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyImageSubData)
    }
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
    detail::error_check("CopyImageSubData"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void debug_message_callback(
        GLDEBUGPROC callback, span_const_void const& userParam)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageCallback)
    }
    glDebugMessageCallback(
        callback,
        userParam.size() ? reinterpret_cast<const void*>(userParam.data())
                         : nullptr);
    detail::error_check("DebugMessageCallback"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void debug_message_control(
        group::debug_source   source,
        group::debug_type     type,
        group::debug_severity severity,
        span_const_u32 const& ids,
        bool                  enabled)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageControl)
    }
    glDebugMessageControl(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        static_cast<GLenum>(severity),
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageControl"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void debug_message_insert(
        group::debug_source     source,
        group::debug_type       type,
        u32                     id,
        group::debug_severity   severity,
        i32                     length,
        std::string_view const& buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageInsert)
    }
    glDebugMessageInsert(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        id,
        static_cast<GLenum>(severity),
        length,
        buf.data());
    detail::error_check("DebugMessageInsert"sv);
}

/* Introduced in GL es 3.2 */
template<
    class span_GLchar,
    class span_debug_severity,
    class span_debug_source,
    class span_debug_type,
    class span_i32,
    class span_u32>
requires(
    MinimumVersion<Current, Version<3, 2>>&&
        semantic::concepts::Span<span_debug_source>&& std::is_same_v<
            std::decay_t<typename span_debug_source::value_type>,
            std::decay_t<group::debug_source>>&&
            semantic::concepts::Span<span_debug_type>&& std::is_same_v<
                std::decay_t<typename span_debug_type::value_type>,
                std::decay_t<group::debug_type>>&&
                semantic::concepts::Span<span_u32>&& std::is_same_v<
                    std::decay_t<typename span_u32::value_type>,
                    std::decay_t<u32>>&& semantic::concepts::
                    Span<span_debug_severity>&&      std::is_same_v<
                        std::decay_t<typename span_debug_severity::value_type>,
                        std::decay_t<group::debug_severity>>&&
                        semantic::concepts::Span<span_i32>&& std::is_same_v<
                            std::decay_t<typename span_i32::value_type>,
                            std::decay_t<i32>>&& semantic::concepts::
                            Span<span_GLchar>&&  std::is_same_v<
                                std::decay_t<typename span_GLchar::value_type>,
                                std::decay_t<GLchar>>) STATICINLINE GLuint
    get_debug_message_log(
        span_debug_source   sources,
        span_debug_type     types,
        span_u32            ids,
        span_debug_severity severities,
        span_i32            lengths,
        span_GLchar         messageLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDebugMessageLog)
    }
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
    detail::error_check("GetDebugMessageLog"sv);
    return out;
}

/* Introduced in GL es 3.2 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_object_label(
        group::object_identifier identifier,
        u32                      name,
        i32&                     length,
        span_GLchar              label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectLabel)
    }
    glGetObjectLabel(
        static_cast<GLenum>(identifier),
        name,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectLabel"sv);
}

/* Introduced in GL es 3.2 */
template<class span_GLchar, class span_const_void>
requires(MinimumVersion<Current, Version<3, 2>>&&
                 semantic::concepts::Span<span_const_void>&&
                 semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                     std::decay_t<typename span_GLchar::value_type>,
                     std::decay_t<GLchar>>) STATICINLINE
    void get_object_ptr_label(
        span_const_void const& ptr, i32& length, span_GLchar label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectPtrLabel)
    }
    glGetObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectPtrLabel"sv);
}

/* Introduced in GL es 3.2 */
template<class span_void>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_pointerv(group::get_pointerv_prop pname, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPointerv)
    }
    glGetPointerv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointerv"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE void object_label(
    group::object_identifier identifier,
    u32                      name,
    i32                      length,
    std::string_view const&  label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectLabel)
    }
    glObjectLabel(static_cast<GLenum>(identifier), name, length, label.data());
    detail::error_check("ObjectLabel"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void object_ptr_label(
        span_const_void const& ptr, i32 length, std::string_view const& label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectPtrLabel)
    }
    glObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        length,
        label.data());
    detail::error_check("ObjectPtrLabel"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void pop_debug_group()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PopDebugGroup)
    }
    glPopDebugGroup();
    detail::error_check("PopDebugGroup"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void push_debug_group(
        group::debug_source     source,
        u32                     id,
        i32                     length,
        std::string_view const& message)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PushDebugGroup)
    }
    glPushDebugGroup(static_cast<GLenum>(source), id, length, message.data());
    detail::error_check("PushDebugGroup"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void blend_equation_separatei(
        u32                            buf,
        group::blend_equation_mode_ext modeRGB,
        group::blend_equation_mode_ext modeAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparatei)
    }
    glBlendEquationSeparatei(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparatei"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void blend_equationi(u32 buf, group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationi)
    }
    glBlendEquationi(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationi"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void blend_func_separatei(
        u32                    buf,
        group::blending_factor srcRGB,
        group::blending_factor dstRGB,
        group::blending_factor srcAlpha,
        group::blending_factor dstAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparatei)
    }
    glBlendFuncSeparatei(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparatei"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE void blend_funci(
    u32 buf, group::blending_factor src, group::blending_factor dst)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunci)
    }
    glBlendFunci(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    detail::error_check("BlendFunci"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void color_maski(u32 index, bool r, bool g, bool b, bool a)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMaski)
    }
    glColorMaski(index, r, g, b, a);
    detail::error_check("ColorMaski"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void disablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Disablei)
    }
    glDisablei(static_cast<GLenum>(target), index);
    detail::error_check("Disablei"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void enablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Enablei)
    }
    glEnablei(static_cast<GLenum>(target), index);
    detail::error_check("Enablei"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE GLboolean
    is_enabledi(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabledi)
    }
    auto out = glIsEnabledi(static_cast<GLenum>(target), index);
    detail::error_check("IsEnabledi"sv);
    return out;
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void draw_elements_base_vertex(
        group::primitive_type     mode,
        i32                       count,
        group::draw_elements_type type,
        ptroff                    indices,
        i32                       basevertex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsBaseVertex)
    }
    glDrawElementsBaseVertex(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        basevertex);
    detail::error_check("DrawElementsBaseVertex"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void draw_elements_instanced_base_vertex(
        group::primitive_type     mode,
        i32                       count,
        group::draw_elements_type type,
        ptroff                    indices,
        i32                       instancecount,
        i32                       basevertex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseVertex)
    }
    glDrawElementsInstancedBaseVertex(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        basevertex);
    detail::error_check("DrawElementsInstancedBaseVertex"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void draw_range_elements_base_vertex(
        group::primitive_type     mode,
        u32                       start,
        u32                       end,
        i32                       count,
        group::draw_elements_type type,
        span_const_void const&    indices,
        i32                       basevertex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElementsBaseVertex)
    }
    glDrawRangeElementsBaseVertex(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr,
        basevertex);
    detail::error_check("DrawRangeElementsBaseVertex"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void framebuffer_texture(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        u32                           texture,
        i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture)
        glIsTexture(texture);
    }
    glFramebufferTexture(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level);
    detail::error_check("FramebufferTexture"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void primitive_bounding_box(
        f32 minX,
        f32 minY,
        f32 minZ,
        f32 minW,
        f32 maxX,
        f32 maxY,
        f32 maxZ,
        f32 maxW)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PrimitiveBoundingBox)
    }
    glPrimitiveBoundingBox(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
    detail::error_check("PrimitiveBoundingBox"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE GLenum
    get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatus)
    }
    auto out = glGetGraphicsResetStatus();
    detail::error_check("GetGraphicsResetStatus"sv);
    return out;
}

/* Introduced in GL es 3.2 */
template<class span_f32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfv)
        glIsProgram(program);
    }
    glGetnUniformfv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformiv)
        glIsProgram(program);
    }
    glGetnUniformiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void getn_uniformuiv(
        u32 program, i32 location, i32 bufSize, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuiv)
        glIsProgram(program);
    }
    glGetnUniformuiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformuiv"sv);
}

/* Introduced in GL es 3.2 */
template<class size_2_i32, class span_void, class vec_2_i32>
requires(MinimumVersion<Current, Version<3, 2>>&&
                     semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                     semantic::concepts::Size2D<size_2_i32, i32>&&
                     semantic::concepts::Span<span_void>) STATICINLINE
    void readn_pixels(
        vec_2_i32 const&    x,
        size_2_i32 const&   width,
        group::pixel_format format,
        group::pixel_type   type,
        span_void           data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadnPixels)
    }
    glReadnPixels(
        x.x(),
        x.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixels"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void min_sample_shading(f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MinSampleShading)
    }
    glMinSampleShading(value);
    detail::error_check("MinSampleShading"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void patch_parameter(group::patch_parameter_name pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PatchParameteri)
    }
    glPatchParameteri(static_cast<GLenum>(pname), value);
    detail::error_check("PatchParameteri"sv);
}

/* Introduced in GL es 3.2 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_sampler_parameter_iiv(
        u32 sampler, group::sampler_parameter_i pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIiv)
        glIsSampler(sampler);
    }
    glGetSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_sampler_parameter_iuiv(
        u32 sampler, group::sampler_parameter_i pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIuiv)
        glIsSampler(sampler);
    }
    glGetSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIuiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_tex_parameter_iiv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIiv)
    }
    glGetTexParameterIiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_tex_parameter_iuiv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_u32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIuiv)
    }
    glGetTexParameterIuiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIuiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void sampler_parameter_iiv(
        u32                        sampler,
        group::sampler_parameter_i pname,
        span_const_i32 const&      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIiv)
        glIsSampler(sampler);
    }
    glSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void sampler_parameter_iuiv(
        u32                        sampler,
        group::sampler_parameter_i pname,
        span_const_u32 const&      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIuiv)
        glIsSampler(sampler);
    }
    glSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLuint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIuiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void tex_parameter_iiv(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIiv)
    }
    glTexParameterIiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIiv"sv);
}

/* Introduced in GL es 3.2 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void tex_parameter_iuiv(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIuiv)
    }
    glTexParameterIuiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIuiv"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE void tex_buffer(
    group::texture_target        target,
    group::sized_internal_format internalformat,
    u32                          buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBuffer)
        glIsBuffer(buffer);
    }
    glTexBuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer);
    detail::error_check("TexBuffer"sv);
}

/* Introduced in GL es 3.2 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 2>>) STATICINLINE
    void tex_buffer_range(
        group::texture_target        target,
        group::sized_internal_format internalformat,
        u32                          buffer,
        GLintptr                     offset,
        GLsizeiptr                   size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexBufferRange)
        glIsBuffer(buffer);
    }
    glTexBufferRange(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        buffer,
        offset,
        size);
    detail::error_check("TexBufferRange"sv);
}

/* Introduced in GL es 3.2 */
template<class size_3_i32>
requires(MinimumVersion<Current, Version<3, 2>>&&
             semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void tex_storage_3d_multisample(
        group::texture_target        target,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_3_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage3DMultisample)
    }
    glTexStorage3DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        fixedsamplelocations);
    detail::error_check("TexStorage3DMultisample"sv);
}

#endif // GL_ES_VERSION_3_2
