#ifdef GL_VERSION_4_3
/* Introduced in GL core 4.3 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 3>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_buffer_data(
        group::buffer_storage_target target,
        group::sized_internal_format internalformat,
        group::pixel_format          format,
        group::pixel_type            type,
        span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferData)
    }
    glClearBufferData(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearBufferData"sv);
}

/* Introduced in GL core 4.3 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 3>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void clear_buffer_sub_data(
        group::buffer_target_arb     target,
        group::sized_internal_format internalformat,
        GLintptr                     offset,
        GLsizeiptr                   size,
        group::pixel_format          format,
        group::pixel_type            type,
        span_const_void const&       data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferSubData)
    }
    glClearBufferSubData(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        offset,
        size,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ClearBufferSubData"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void dispatch_compute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchCompute)
    }
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    detail::error_check("DispatchCompute"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void dispatch_compute_indirect(GLintptr indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchComputeIndirect)
    }
    glDispatchComputeIndirect(indirect);
    detail::error_check("DispatchComputeIndirect"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
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

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void framebuffer_parameter(
        group::framebuffer_target         target,
        group::framebuffer_parameter_name pname,
        i32                               param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferParameteri)
    }
    glFramebufferParameteri(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("FramebufferParameteri"sv);
}

/* Introduced in GL core 4.3 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 3>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_framebuffer_parameter(
        group::framebuffer_target                    target,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferParameteriv)
    }
    glGetFramebufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferParameteriv"sv);
}

/* Introduced in GL core 4.3 */
template<class span_i64>
requires(MinimumVersion<Current, Version<4, 3>>&&
             semantic::concepts::Span<span_i64>&& std::is_same_v<
                 std::decay_t<typename span_i64::value_type>,
                 std::decay_t<i64>>) STATICINLINE
    void get_internalformati64v(
        group::texture_target       target,
        group::internal_format      internalformat,
        group::internal_format_prop pname,
        span_i64                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInternalformati64v)
    }
    glGetInternalformati64v(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        static_cast<GLenum>(pname),
        params.size(),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetInternalformati64v"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void invalidate_buffer_data(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateBufferData)
        glIsBuffer(buffer);
    }
    glInvalidateBufferData(buffer);
    detail::error_check("InvalidateBufferData"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void invalidate_buffer_sub_data(
        u32 buffer, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateBufferSubData)
        glIsBuffer(buffer);
    }
    glInvalidateBufferSubData(buffer, offset, length);
    detail::error_check("InvalidateBufferSubData"sv);
}

/* Introduced in GL core 4.3 */
template<class span_const_invalidate_framebuffer_attachment>
requires(
    MinimumVersion<Current, Version<4, 3>>&& semantic::concepts::
        Span<span_const_invalidate_framebuffer_attachment>&& std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>)
    STATICINLINE void invalidate_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateFramebuffer)
    }
    glInvalidateFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("InvalidateFramebuffer"sv);
}

/* Introduced in GL core 4.3 */
template<
    class size_2_i32,
    class span_const_invalidate_framebuffer_attachment,
    class vec_2_i32>
requires(
    MinimumVersion<Current, Version<4, 3>>&& semantic::concepts::
        Span<span_const_invalidate_framebuffer_attachment>&& std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>&&
                semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void invalidate_sub_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments,
        vec_2_i32 const&                                    x,
        size_2_i32 const&                                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateSubFramebuffer)
    }
    glInvalidateSubFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr,
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("InvalidateSubFramebuffer"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void invalidate_tex_image(u32 texture, i32 level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateTexImage)
        glIsTexture(texture);
    }
    glInvalidateTexImage(texture, level);
    detail::error_check("InvalidateTexImage"sv);
}

/* Introduced in GL core 4.3 */
template<class size_3_i32, class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 3>>&&
                 semantic::concepts::Vector<vec_3_i32, i32, 3>&&
                 semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void invalidate_tex_sub_image(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateTexSubImage)
        glIsTexture(texture);
    }
    glInvalidateTexSubImage(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2]);
    detail::error_check("InvalidateTexSubImage"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void multi_draw_arrays_indirect(
        group::primitive_type mode, ptroff indirect, i32 drawcount, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysIndirect)
    }
    glMultiDrawArraysIndirect(
        static_cast<GLenum>(mode),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawArraysIndirect"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void multi_draw_elements_indirect(
        group::primitive_type     mode,
        group::draw_elements_type type,
        ptroff                    indirect,
        i32                       drawcount,
        i32                       stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsIndirect)
    }
    glMultiDrawElementsIndirect(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect),
        drawcount,
        stride);
    detail::error_check("MultiDrawElementsIndirect"sv);
}

/* Introduced in GL core 4.3 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 3>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_program_interfaceiv(
        u32                           program,
        group::program_interface      programInterface,
        group::program_interface_prop pname,
        span_i32                      params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramInterfaceiv)
        glIsProgram(program);
    }
    glGetProgramInterfaceiv(
        program,
        static_cast<GLenum>(programInterface),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramInterfaceiv"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE GLuint
    get_program_resource_index(
        u32                      program,
        group::program_interface programInterface,
        std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceIndex)
        glIsProgram(program);
    }
    auto out = glGetProgramResourceIndex(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceIndex"sv);
    return out;
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE GLint
    get_program_resource_location(
        u32                      program,
        group::program_interface programInterface,
        std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceLocation)
        glIsProgram(program);
    }
    auto out = glGetProgramResourceLocation(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocation"sv);
    return out;
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE GLint
    get_program_resource_location_index(
        u32                      program,
        group::program_interface programInterface,
        std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceLocationIndex)
        glIsProgram(program);
    }
    auto out = glGetProgramResourceLocationIndex(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocationIndex"sv);
    return out;
}

/* Introduced in GL core 4.3 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<4, 3>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_program_resource_name(
        u32                      program,
        group::program_interface programInterface,
        u32                      index,
        i32&                     length,
        span_GLchar              name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceName)
        glIsProgram(program);
    }
    glGetProgramResourceName(
        program,
        static_cast<GLenum>(programInterface),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetProgramResourceName"sv);
}

/* Introduced in GL core 4.3 */
template<class span_const_program_resource_property, class span_i32>
requires(MinimumVersion<Current, Version<4, 3>>&&         semantic::concepts::
             Span<span_const_program_resource_property>&& std::is_same_v<
                 std::decay_t<
                     typename span_const_program_resource_property::value_type>,
                 std::decay_t<group::program_resource_property>>&&
                 semantic::concepts::Span<span_i32>&& std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>) STATICINLINE
    void get_program_resourceiv(
        u32                                         program,
        group::program_interface                    programInterface,
        u32                                         index,
        span_const_program_resource_property const& props,
        i32&                                        length,
        span_i32                                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceiv)
        glIsProgram(program);
    }
    glGetProgramResourceiv(
        program,
        static_cast<GLenum>(programInterface),
        index,
        props.size(),
        props.size() ? reinterpret_cast<const GLenum*>(props.data()) : nullptr,
        params.size(),
        &length,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramResourceiv"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void shader_storage_block_binding(
        u32 program, u32 storageBlockIndex, u32 storageBlockBinding)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderStorageBlockBinding)
        glIsProgram(program);
    }
    glShaderStorageBlockBinding(
        program, storageBlockIndex, storageBlockBinding);
    detail::error_check("ShaderStorageBlockBinding"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
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

/* Introduced in GL core 4.3 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 3>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void tex_storage_2d_multisample(
        group::texture_target        target,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage2DMultisample)
    }
    glTexStorage2DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TexStorage2DMultisample"sv);
}

/* Introduced in GL core 4.3 */
template<class size_3_i32>
requires(MinimumVersion<Current, Version<4, 3>>&&
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

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE void texture_view(
    u32                          texture,
    group::texture_target        target,
    u32                          origtexture,
    group::sized_internal_format internalformat,
    u32                          minlevel,
    u32                          numlevels,
    u32                          minlayer,
    u32                          numlayers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TextureView)
        glIsTexture(texture);
    }
    glTextureView(
        texture,
        static_cast<GLenum>(target),
        origtexture,
        static_cast<GLenum>(internalformat),
        minlevel,
        numlevels,
        minlayer,
        numlayers);
    detail::error_check("TextureView"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void bind_vertex_buffer(
        u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexBuffer)
        glIsBuffer(buffer);
    }
    glBindVertexBuffer(bindingindex, buffer, offset, stride);
    detail::error_check("BindVertexBuffer"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void vertex_attrib_binding(u32 attribindex, u32 bindingindex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribBinding)
    }
    glVertexAttribBinding(attribindex, bindingindex);
    detail::error_check("VertexAttribBinding"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void vertex_attrib_format(
        u32                       attribindex,
        i32                       size,
        group::vertex_attrib_type type,
        bool                      normalized,
        u32                       relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribFormat)
    }
    glVertexAttribFormat(
        attribindex,
        size,
        static_cast<GLenum>(type),
        normalized,
        relativeoffset);
    detail::error_check("VertexAttribFormat"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void vertex_attrib_i_format(
        u32                      attribindex,
        i32                      size,
        group::vertex_attrib_int type,
        u32                      relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIFormat)
    }
    glVertexAttribIFormat(
        attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexAttribIFormat"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void vertex_attrib_l_format(
        u32                       attribindex,
        i32                       size,
        group::vertex_attrib_long type,
        u32                       relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribLFormat)
    }
    glVertexAttribLFormat(
        attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexAttribLFormat"sv);
}

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
    void vertex_binding_divisor(u32 bindingindex, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexBindingDivisor)
    }
    glVertexBindingDivisor(bindingindex, divisor);
    detail::error_check("VertexBindingDivisor"sv);
}

/* Introduced in GL core 4.3 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 3>>&&
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

/* Introduced in GL core 4.3 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 3>>&&
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

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
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

/* Introduced in GL core 4.3 */
template<
    class span_GLchar,
    class span_debug_severity,
    class span_debug_source,
    class span_debug_type,
    class span_i32,
    class span_u32>
requires(
    MinimumVersion<Current, Version<4, 3>>&&
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

/* Introduced in GL core 4.3 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<4, 3>>&&
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

/* Introduced in GL core 4.3 */
template<class span_GLchar, class span_const_void>
requires(MinimumVersion<Current, Version<4, 3>>&&
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

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE void object_label(
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

/* Introduced in GL core 4.3 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 3>>&&
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

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
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

/* Introduced in GL core 4.3 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 3>>) STATICINLINE
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

#endif // GL_VERSION_4_3