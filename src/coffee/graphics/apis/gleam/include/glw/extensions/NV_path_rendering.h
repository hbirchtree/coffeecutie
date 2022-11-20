#pragma once

#ifdef GL_NV_path_rendering
#include "../enums/CombinerRegisterNV.h"
#include "../enums/FragmentShaderGenericSourceATI.h"
#include "../enums/PathColor.h"
#include "../enums/PathCoordType.h"
#include "../enums/PathCoverMode.h"
#include "../enums/PathElementType.h"
#include "../enums/PathFillMode.h"
#include "../enums/PathFontStyle.h"
#include "../enums/PathFontTarget.h"
#include "../enums/PathGenMode.h"
#include "../enums/PathHandleMissingGlyphs.h"
#include "../enums/PathListMode.h"
#include "../enums/PathMetricMask.h"
#include "../enums/PathParameter.h"
#include "../enums/PathStringFormat.h"
#include "../enums/PathTransformType.h"
#include "../enums/TextureEnvParameter.h"
namespace gl::nv::path_rendering {
using gl::group::combiner_register_nv;
using gl::group::fragment_shader_generic_source_ati;
using gl::group::path_color;
using gl::group::path_coord_type;
using gl::group::path_cover_mode;
using gl::group::path_element_type;
using gl::group::path_fill_mode;
using gl::group::path_font_style;
using gl::group::path_font_target;
using gl::group::path_gen_mode;
using gl::group::path_handle_missing_glyphs;
using gl::group::path_list_mode;
using gl::group::path_metric_mask;
using gl::group::path_parameter;
using gl::group::path_string_format;
using gl::group::path_transform_type;
using gl::group::texture_env_parameter;
namespace values {
constexpr libc_types::u32 path_modelview_stack_depth      = 0x0BA3;
constexpr libc_types::u32 path_projection_stack_depth     = 0x0BA4;
constexpr libc_types::u32 path_modelview_matrix           = 0x0BA6;
constexpr libc_types::u32 path_projection_matrix          = 0x0BA7;
constexpr libc_types::u32 path_max_modelview_stack_depth  = 0x0D36;
constexpr libc_types::u32 path_max_projection_stack_depth = 0x0D38;
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 n2_bytes = 0x1407;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 n3_bytes = 0x1408;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 n4_bytes = 0x1409;
#endif
constexpr libc_types::u32 path_modelview  = 0x1700;
constexpr libc_types::u32 path_projection = 0x1701;
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 eye_linear = 0x2400;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 object_linear = 0x2401;
#endif
constexpr libc_types::u32 path_transpose_modelview_matrix  = 0x84E3;
constexpr libc_types::u32 path_transpose_projection_matrix = 0x84E4;
constexpr libc_types::u32 square                           = 0x90A3;
constexpr libc_types::u32 round                            = 0x90A4;
constexpr libc_types::u32 triangular                       = 0x90A5;
constexpr libc_types::u32 bevel                            = 0x90A6;
constexpr libc_types::u32 miter_revert                     = 0x90A7;
constexpr libc_types::u32 miter_truncate                   = 0x90A8;
constexpr libc_types::u32 path_error_position              = 0x90AB;
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 path_fog_gen_mode = 0x90AC;
#endif
constexpr libc_types::u32 path_gen_mode  = 0x90B0;
constexpr libc_types::u32 path_gen_coeff = 0x90B1;
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 path_gen_color_format = 0x90B2;
#endif
constexpr libc_types::u32 path_gen_components              = 0x90B3;
constexpr libc_types::u32 move_to_resets                   = 0x90B5;
constexpr libc_types::u32 move_to_continues                = 0x90B6;
constexpr libc_types::u32 path_stencil_func                = 0x90B7;
constexpr libc_types::u32 path_stencil_ref                 = 0x90B8;
constexpr libc_types::u32 path_stencil_value_mask          = 0x90B9;
constexpr libc_types::u32 path_stencil_depth_offset_factor = 0x90BD;
constexpr libc_types::u32 path_stencil_depth_offset_units  = 0x90BE;
constexpr libc_types::u32 path_cover_depth_func            = 0x90BF;
constexpr libc_types::u32 font_glyphs_available            = 0x9368;
constexpr libc_types::u32 font_target_unavailable          = 0x9369;
constexpr libc_types::u32 font_unavailable                 = 0x936A;
constexpr libc_types::u32 font_unintelligible              = 0x936B;
constexpr libc_types::u32 standard_font_format             = 0x936C;
constexpr libc_types::u32 fragment_input                   = 0x936D;
} // namespace values
/*!
 * \brief Part of GL_NV_path_rendering
 * \param resultPath GLuint
 * \param srcPath GLuint
 * \return void
 */
STATICINLINE void copy_path(u32 resultPath, u32 srcPath)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CopyPathNV)
    }
    glCopyPathNV(resultPath, srcPath);
    detail::error_check("CopyPathNV"sv);
}

template<class span_const_f32, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param coverMode GLenum
     * \param transformType GLenum
     * \param transformValues const GLfloat *
     * \return void
     */
    STATICINLINE void cover_fill_path_instanced(
        i32                        numPaths,
        group::path_element_type   pathNameType,
        span_const_void const&     paths,
        u32                        pathBase,
        group::path_cover_mode     coverMode,
        group::path_transform_type transformType,
        span_const_f32 const&      transformValues)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverFillPathInstancedNV)
    }
    glCoverFillPathInstancedNV(
        numPaths,
        static_cast<GLenum>(pathNameType),
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        static_cast<GLenum>(coverMode),
        static_cast<GLenum>(transformType),
        transformValues.size()
            ? reinterpret_cast<const GLfloat*>(transformValues.data())
            : nullptr);
    detail::error_check("CoverFillPathInstancedNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param coverMode GLenum
 * \return void
 */
STATICINLINE void cover_fill_path(u32 path, group::path_cover_mode coverMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverFillPathNV)
    }
    glCoverFillPathNV(path, static_cast<GLenum>(coverMode));
    detail::error_check("CoverFillPathNV"sv);
}

template<class span_const_f32, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param coverMode GLenum
     * \param transformType GLenum
     * \param transformValues const GLfloat *
     * \return void
     */
    STATICINLINE void cover_stroke_path_instanced(
        i32                        numPaths,
        group::path_element_type   pathNameType,
        span_const_void const&     paths,
        u32                        pathBase,
        group::path_cover_mode     coverMode,
        group::path_transform_type transformType,
        span_const_f32 const&      transformValues)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverStrokePathInstancedNV)
    }
    glCoverStrokePathInstancedNV(
        numPaths,
        static_cast<GLenum>(pathNameType),
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        static_cast<GLenum>(coverMode),
        static_cast<GLenum>(transformType),
        transformValues.size()
            ? reinterpret_cast<const GLfloat*>(transformValues.data())
            : nullptr);
    detail::error_check("CoverStrokePathInstancedNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param coverMode GLenum
 * \return void
 */
STATICINLINE void cover_stroke_path(u32 path, group::path_cover_mode coverMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CoverStrokePathNV)
    }
    glCoverStrokePathNV(path, static_cast<GLenum>(coverMode));
    detail::error_check("CoverStrokePathNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param range GLsizei
 * \return void
 */
STATICINLINE void delete_paths(u32 path, i32 range)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeletePathsNV)
    }
    glDeletePathsNV(path, range);
    detail::error_check("DeletePathsNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param range GLsizei
 * \return Path
 */
STATICINLINE GLuint gen_paths(i32 range)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenPathsNV)
    }
    auto out = glGenPathsNV(range);
    detail::error_check("GenPathsNV"sv);
    return out;
}

template<class span_u8>
requires(semantic::concepts::Span<span_u8>&& std::is_same_v<
         std::decay_t<typename span_u8::value_type>,
         std::decay_t<u8>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param commands GLubyte *
     * \return void
     */
    STATICINLINE void get_path_commands(u32 path, span_u8 commands)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathCommandsNV)
    }
    glGetPathCommandsNV(
        path,
        commands.size() ? reinterpret_cast<GLubyte*>(commands.data())
                        : nullptr);
    detail::error_check("GetPathCommandsNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param coords GLfloat *
     * \return void
     */
    STATICINLINE void get_path_coords(u32 path, span_f32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathCoordsNV)
    }
    glGetPathCoordsNV(
        path,
        coords.size() ? reinterpret_cast<GLfloat*>(coords.data()) : nullptr);
    detail::error_check("GetPathCoordsNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param dashArray GLfloat *
     * \return void
     */
    STATICINLINE void get_path_dash_array(u32 path, span_f32 dashArray)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathDashArrayNV)
    }
    glGetPathDashArrayNV(
        path,
        dashArray.size() ? reinterpret_cast<GLfloat*>(dashArray.data())
                         : nullptr);
    detail::error_check("GetPathDashArrayNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param startSegment GLsizei
 * \param numSegments GLsizei
 * \return GLfloat
 */
STATICINLINE GLfloat
get_path_length(u32 path, i32 startSegment, i32 numSegments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathLengthNV)
    }
    auto out = glGetPathLengthNV(path, startSegment, numSegments);
    detail::error_check("GetPathLengthNV"sv);
    return out;
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param metricQueryMask GLbitfield
     * \param firstPathName GLuint
     * \param numPaths GLsizei
     * \param stride GLsizei
     * \param metrics GLfloat *
     * \return void
     */
    STATICINLINE void get_path_metric_range(
        group::path_metric_mask metricQueryMask,
        u32                     firstPathName,
        i32                     numPaths,
        i32                     stride,
        span_f32                metrics)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathMetricRangeNV)
    }
    glGetPathMetricRangeNV(
        static_cast<GLenum>(metricQueryMask),
        firstPathName,
        numPaths,
        stride,
        metrics.size() ? reinterpret_cast<GLfloat*>(metrics.data()) : nullptr);
    detail::error_check("GetPathMetricRangeNV"sv);
}

template<class span_const_void, class span_f32>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param metricQueryMask GLbitfield
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param stride GLsizei
     * \param metrics GLfloat *
     * \return void
     */
    STATICINLINE void get_path_metrics(
        group::path_metric_mask  metricQueryMask,
        i32                      numPaths,
        group::path_element_type pathNameType,
        span_const_void const&   paths,
        u32                      pathBase,
        i32                      stride,
        span_f32                 metrics)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathMetricsNV)
    }
    glGetPathMetricsNV(
        static_cast<GLenum>(metricQueryMask),
        numPaths,
        static_cast<GLenum>(pathNameType),
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        stride,
        metrics.size() ? reinterpret_cast<GLfloat*>(metrics.data()) : nullptr);
    detail::error_check("GetPathMetricsNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param pname GLenum
     * \param value GLfloat *
     * \return void
     */
    STATICINLINE void get_path_parameter(
        u32 path, group::path_parameter pname, span_f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathParameterfvNV)
    }
    glGetPathParameterfvNV(
        path,
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<GLfloat*>(value.data()) : nullptr);
    detail::error_check("GetPathParameterfvNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param pname GLenum
     * \param value GLint *
     * \return void
     */
    STATICINLINE void get_path_parameter(
        u32 path, group::path_parameter pname, span_i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathParameterivNV)
    }
    glGetPathParameterivNV(
        path,
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<GLint*>(value.data()) : nullptr);
    detail::error_check("GetPathParameterivNV"sv);
}

template<class span_const_void, class span_f32>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param pathListMode GLenum
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param advanceScale GLfloat
     * \param kerningScale GLfloat
     * \param transformType GLenum
     * \param returnedSpacing GLfloat *
     * \return void
     */
    STATICINLINE void get_path_spacing(
        group::path_list_mode      pathListMode,
        i32                        numPaths,
        group::path_element_type   pathNameType,
        span_const_void const&     paths,
        u32                        pathBase,
        f32                        advanceScale,
        f32                        kerningScale,
        group::path_transform_type transformType,
        span_f32                   returnedSpacing)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathSpacingNV)
    }
    glGetPathSpacingNV(
        static_cast<GLenum>(pathListMode),
        numPaths,
        static_cast<GLenum>(pathNameType),
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        advanceScale,
        kerningScale,
        static_cast<GLenum>(transformType),
        returnedSpacing.size()
            ? reinterpret_cast<GLfloat*>(returnedSpacing.data())
            : nullptr);
    detail::error_check("GetPathSpacingNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param resultPath GLuint
 * \param pathA GLuint
 * \param pathB GLuint
 * \param weight GLfloat
 * \return void
 */
STATICINLINE void interpolate_paths(
    u32 resultPath, u32 pathA, u32 pathB, f32 weight)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InterpolatePathsNV)
    }
    glInterpolatePathsNV(resultPath, pathA, pathB, weight);
    detail::error_check("InterpolatePathsNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_path(u32 path)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsPathNV)
    }
    auto out = glIsPathNV(path);
    detail::error_check("IsPathNV"sv);
    return out;
}

template<class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param mask GLuint
     * \param x GLfloat
     * \param y GLfloat
     * \return Boolean
     */
    STATICINLINE GLboolean
    is_point_in_fill_path(u32 path, u32 mask, vec_2_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsPointInFillPathNV)
    }
    auto out = glIsPointInFillPathNV(path, mask, x.x(), x.y());
    detail::error_check("IsPointInFillPathNV"sv);
    return out;
}

template<class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param x GLfloat
     * \param y GLfloat
     * \return Boolean
     */
    STATICINLINE GLboolean is_point_in_stroke_path(u32 path, vec_2_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsPointInStrokePathNV)
    }
    auto out = glIsPointInStrokePathNV(path, x.x(), x.y());
    detail::error_check("IsPointInStrokePathNV"sv);
    return out;
}

template<class span_const_u8, class span_const_void>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>&& semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param numCommands GLsizei
     * \param commands const GLubyte *
     * \param numCoords GLsizei
     * \param coordType GLenum
     * \param coords const void *
     * \return void
     */
    STATICINLINE void path_commands(
        u32                    path,
        span_const_u8 const&   commands,
        i32                    numCoords,
        group::path_coord_type coordType,
        span_const_void const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathCommandsNV)
    }
    glPathCommandsNV(
        path,
        commands.size(),
        commands.size() ? reinterpret_cast<const GLubyte*>(commands.data())
                        : nullptr,
        numCoords,
        static_cast<GLenum>(coordType),
        coords.size() ? reinterpret_cast<const void*>(coords.data()) : nullptr);
    detail::error_check("PathCommandsNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param numCoords GLsizei
     * \param coordType GLenum
     * \param coords const void *
     * \return void
     */
    STATICINLINE void path_coords(
        u32                    path,
        i32                    numCoords,
        group::path_coord_type coordType,
        span_const_void const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathCoordsNV)
    }
    glPathCoordsNV(
        path,
        numCoords,
        static_cast<GLenum>(coordType),
        coords.size() ? reinterpret_cast<const void*>(coords.data()) : nullptr);
    detail::error_check("PathCoordsNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param func GLenum
 * \return void
 */
STATICINLINE void path_cover_depth_func(group::depth_function func)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathCoverDepthFuncNV)
    }
    glPathCoverDepthFuncNV(static_cast<GLenum>(func));
    detail::error_check("PathCoverDepthFuncNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param dashCount GLsizei
     * \param dashArray const GLfloat *
     * \return void
     */
    STATICINLINE void path_dash_array(u32 path, span_const_f32 const& dashArray)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathDashArrayNV)
    }
    glPathDashArrayNV(
        path,
        dashArray.size(),
        dashArray.size() ? reinterpret_cast<const GLfloat*>(dashArray.data())
                         : nullptr);
    detail::error_check("PathDashArrayNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param firstPathName GLuint
     * \param fontTarget GLenum
     * \param fontName const void *
     * \param fontStyle GLbitfield
     * \param firstGlyph GLuint
     * \param numGlyphs GLsizei
     * \param handleMissingGlyphs GLenum
     * \param pathParameterTemplate GLuint
     * \param emScale GLfloat
     * \return void
     */
    STATICINLINE void path_glyph_range(
        u32                               firstPathName,
        group::path_font_target           fontTarget,
        span_const_void const&            fontName,
        group::path_font_style            fontStyle,
        u32                               firstGlyph,
        i32                               numGlyphs,
        group::path_handle_missing_glyphs handleMissingGlyphs,
        u32                               pathParameterTemplate,
        f32                               emScale)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathGlyphRangeNV)
    }
    glPathGlyphRangeNV(
        firstPathName,
        static_cast<GLenum>(fontTarget),
        fontName.size() ? reinterpret_cast<const void*>(fontName.data())
                        : nullptr,
        static_cast<GLenum>(fontStyle),
        firstGlyph,
        numGlyphs,
        static_cast<GLenum>(handleMissingGlyphs),
        pathParameterTemplate,
        emScale);
    detail::error_check("PathGlyphRangeNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param firstPathName GLuint
     * \param fontTarget GLenum
     * \param fontName const void *
     * \param fontStyle GLbitfield
     * \param numGlyphs GLsizei
     * \param type GLenum
     * \param charcodes const void *
     * \param handleMissingGlyphs GLenum
     * \param pathParameterTemplate GLuint
     * \param emScale GLfloat
     * \return void
     */
    STATICINLINE void path_glyphs(
        u32                               firstPathName,
        group::path_font_target           fontTarget,
        span_const_void const&            fontName,
        group::path_font_style            fontStyle,
        i32                               numGlyphs,
        group::path_element_type          type,
        span_const_void const&            charcodes,
        group::path_handle_missing_glyphs handleMissingGlyphs,
        u32                               pathParameterTemplate,
        f32                               emScale)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathGlyphsNV)
    }
    glPathGlyphsNV(
        firstPathName,
        static_cast<GLenum>(fontTarget),
        fontName.size() ? reinterpret_cast<const void*>(fontName.data())
                        : nullptr,
        static_cast<GLenum>(fontStyle),
        numGlyphs,
        static_cast<GLenum>(type),
        charcodes.size() ? reinterpret_cast<const void*>(charcodes.data())
                         : nullptr,
        static_cast<GLenum>(handleMissingGlyphs),
        pathParameterTemplate,
        emScale);
    detail::error_check("PathGlyphsNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param pname GLenum
 * \param value GLfloat
 * \return void
 */
STATICINLINE void path_parameter(
    u32 path, group::path_parameter pname, f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathParameterfNV)
    }
    glPathParameterfNV(path, static_cast<GLenum>(pname), value);
    detail::error_check("PathParameterfNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param pname GLenum
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void path_parameter(
        u32 path, group::path_parameter pname, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathParameterfvNV)
    }
    glPathParameterfvNV(
        path,
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<const GLfloat*>(value.data())
                     : nullptr);
    detail::error_check("PathParameterfvNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void path_parameter(
    u32 path, group::path_parameter pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathParameteriNV)
    }
    glPathParameteriNV(path, static_cast<GLenum>(pname), value);
    detail::error_check("PathParameteriNV"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param pname GLenum
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void path_parameter(
        u32 path, group::path_parameter pname, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathParameterivNV)
    }
    glPathParameterivNV(
        path,
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<const GLint*>(value.data()) : nullptr);
    detail::error_check("PathParameterivNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param factor GLfloat
 * \param units GLfloat
 * \return void
 */
STATICINLINE void path_stencil_depth_offset(f32 factor, f32 units)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathStencilDepthOffsetNV)
    }
    glPathStencilDepthOffsetNV(factor, units);
    detail::error_check("PathStencilDepthOffsetNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param func GLenum
 * \param ref GLint
 * \param mask GLuint
 * \return void
 */
STATICINLINE void path_stencil_func(
    group::stencil_function func, i32 ref, u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathStencilFuncNV)
    }
    glPathStencilFuncNV(static_cast<GLenum>(func), ref, mask);
    detail::error_check("PathStencilFuncNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param format GLenum
     * \param length GLsizei
     * \param pathString const void *
     * \return void
     */
    STATICINLINE void path_string(
        u32                       path,
        group::path_string_format format,
        span_const_void const&    pathString)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathStringNV)
    }
    glPathStringNV(
        path,
        static_cast<GLenum>(format),
        pathString.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        pathString.size() ? reinterpret_cast<const void*>(pathString.data())
                          : nullptr);
    detail::error_check("PathStringNV"sv);
}

template<class span_const_u8, class span_const_void>
requires(semantic::concepts::Span<span_const_u8>&& std::is_same_v<
         std::decay_t<typename span_const_u8::value_type>,
         std::decay_t<u8>>&& semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param commandStart GLsizei
     * \param commandsToDelete GLsizei
     * \param numCommands GLsizei
     * \param commands const GLubyte *
     * \param numCoords GLsizei
     * \param coordType GLenum
     * \param coords const void *
     * \return void
     */
    STATICINLINE void path_sub_commands(
        u32                    path,
        i32                    commandStart,
        i32                    commandsToDelete,
        span_const_u8 const&   commands,
        i32                    numCoords,
        group::path_coord_type coordType,
        span_const_void const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathSubCommandsNV)
    }
    glPathSubCommandsNV(
        path,
        commandStart,
        commandsToDelete,
        commands.size(),
        commands.size() ? reinterpret_cast<const GLubyte*>(commands.data())
                        : nullptr,
        numCoords,
        static_cast<GLenum>(coordType),
        coords.size() ? reinterpret_cast<const void*>(coords.data()) : nullptr);
    detail::error_check("PathSubCommandsNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param coordStart GLsizei
     * \param numCoords GLsizei
     * \param coordType GLenum
     * \param coords const void *
     * \return void
     */
    STATICINLINE void path_sub_coords(
        u32                    path,
        i32                    coordStart,
        i32                    numCoords,
        group::path_coord_type coordType,
        span_const_void const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathSubCoordsNV)
    }
    glPathSubCoordsNV(
        path,
        coordStart,
        numCoords,
        static_cast<GLenum>(coordType),
        coords.size() ? reinterpret_cast<const void*>(coords.data()) : nullptr);
    detail::error_check("PathSubCoordsNV"sv);
}

template<class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param path GLuint
     * \param startSegment GLsizei
     * \param numSegments GLsizei
     * \param distance GLfloat
     * \param x GLfloat *
     * \param y GLfloat *
     * \param tangentX GLfloat *
     * \param tangentY GLfloat *
     * \return Boolean
     */
    STATICINLINE GLboolean point_along_path(
        u32              path,
        i32              startSegment,
        i32              numSegments,
        f32              distance,
        vec_2_f32 const& x,
        f32&             tangentX,
        f32&             tangentY)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PointAlongPathNV)
    }
    auto out = glPointAlongPathNV(
        path,
        startSegment,
        numSegments,
        distance,
        x.x(),
        x.y(),
        &tangentX,
        &tangentY);
    detail::error_check("PointAlongPathNV"sv);
    return out;
}

template<class span_const_f32, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param fillMode GLenum
     * \param mask GLuint
     * \param transformType GLenum
     * \param transformValues const GLfloat *
     * \return void
     */
    STATICINLINE void stencil_fill_path_instanced(
        i32                        numPaths,
        group::path_element_type   pathNameType,
        span_const_void const&     paths,
        u32                        pathBase,
        group::path_fill_mode      fillMode,
        u32                        mask,
        group::path_transform_type transformType,
        span_const_f32 const&      transformValues)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilFillPathInstancedNV)
    }
    glStencilFillPathInstancedNV(
        numPaths,
        static_cast<GLenum>(pathNameType),
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        static_cast<GLenum>(fillMode),
        mask,
        static_cast<GLenum>(transformType),
        transformValues.size()
            ? reinterpret_cast<const GLfloat*>(transformValues.data())
            : nullptr);
    detail::error_check("StencilFillPathInstancedNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param fillMode GLenum
 * \param mask GLuint
 * \return void
 */
STATICINLINE void stencil_fill_path(
    u32 path, group::path_fill_mode fillMode, u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilFillPathNV)
    }
    glStencilFillPathNV(path, static_cast<GLenum>(fillMode), mask);
    detail::error_check("StencilFillPathNV"sv);
}

template<class span_const_f32, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param reference GLint
     * \param mask GLuint
     * \param transformType GLenum
     * \param transformValues const GLfloat *
     * \return void
     */
    STATICINLINE void stencil_stroke_path_instanced(
        i32                        numPaths,
        group::path_element_type   pathNameType,
        span_const_void const&     paths,
        u32                        pathBase,
        i32                        reference,
        u32                        mask,
        group::path_transform_type transformType,
        span_const_f32 const&      transformValues)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilStrokePathInstancedNV)
    }
    glStencilStrokePathInstancedNV(
        numPaths,
        static_cast<GLenum>(pathNameType),
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        reference,
        mask,
        static_cast<GLenum>(transformType),
        transformValues.size()
            ? reinterpret_cast<const GLfloat*>(transformValues.data())
            : nullptr);
    detail::error_check("StencilStrokePathInstancedNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param reference GLint
 * \param mask GLuint
 * \return void
 */
STATICINLINE void stencil_stroke_path(u32 path, i32 reference, u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilStrokePathNV)
    }
    glStencilStrokePathNV(path, reference, mask);
    detail::error_check("StencilStrokePathNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param resultPath GLuint
     * \param srcPath GLuint
     * \param transformType GLenum
     * \param transformValues const GLfloat *
     * \return void
     */
    STATICINLINE void transform_path(
        u32                        resultPath,
        u32                        srcPath,
        group::path_transform_type transformType,
        span_const_f32 const&      transformValues)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformPathNV)
    }
    glTransformPathNV(
        resultPath,
        srcPath,
        static_cast<GLenum>(transformType),
        transformValues.size()
            ? reinterpret_cast<const GLfloat*>(transformValues.data())
            : nullptr);
    detail::error_check("TransformPathNV"sv);
}

template<class span_const_f32, class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>&& semantic::concepts::Span<span_const_f32>&&
                              std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param resultPath GLuint
     * \param numPaths GLsizei
     * \param paths const GLuint *
     * \param weights const GLfloat *
     * \return void
     */
    STATICINLINE void weight_paths(
        u32                   resultPath,
        span_const_u32 const& paths,
        span_const_f32 const& weights)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WeightPathsNV)
    }
    glWeightPathsNV(
        resultPath,
        paths.size(),
        paths.size() ? reinterpret_cast<const GLuint*>(paths.data()) : nullptr,
        weights.size() ? reinterpret_cast<const GLfloat*>(weights.data())
                       : nullptr);
    detail::error_check("WeightPathsNV"sv);
}

template<class span_const_mat_3x2_f32>
requires(semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param matrixMode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE
    void matrix_load3x2f(GLenum matrixMode, span_const_mat_3x2_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoad3x2fNV)
    }
    glMatrixLoad3x2fNV(matrixMode, reinterpret_cast<const GLfloat*>(m.data()));
    detail::error_check("MatrixLoad3x2fNV"sv);
}

template<class span_const_mat_3x3_f32>
requires(semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param matrixMode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE
    void matrix_load3x3f(GLenum matrixMode, span_const_mat_3x3_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoad3x3fNV)
    }
    glMatrixLoad3x3fNV(matrixMode, reinterpret_cast<const GLfloat*>(m.data()));
    detail::error_check("MatrixLoad3x3fNV"sv);
}

template<class span_const_mat_3x3_f32>
requires(semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param matrixMode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE void matrix_load_transpose3x3f(
        GLenum matrixMode, span_const_mat_3x3_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadTranspose3x3fNV)
    }
    glMatrixLoadTranspose3x3fNV(
        matrixMode, reinterpret_cast<const GLfloat*>(m.data()));
    detail::error_check("MatrixLoadTranspose3x3fNV"sv);
}

template<class span_const_mat_3x2_f32>
requires(semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param matrixMode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE
    void matrix_mult3x2f(GLenum matrixMode, span_const_mat_3x2_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMult3x2fNV)
    }
    glMatrixMult3x2fNV(matrixMode, reinterpret_cast<const GLfloat*>(m.data()));
    detail::error_check("MatrixMult3x2fNV"sv);
}

template<class span_const_mat_3x3_f32>
requires(semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param matrixMode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE
    void matrix_mult3x3f(GLenum matrixMode, span_const_mat_3x3_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMult3x3fNV)
    }
    glMatrixMult3x3fNV(matrixMode, reinterpret_cast<const GLfloat*>(m.data()));
    detail::error_check("MatrixMult3x3fNV"sv);
}

template<class span_const_mat_3x3_f32>
requires(semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param matrixMode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE void matrix_mult_transpose3x3f(
        GLenum matrixMode, span_const_mat_3x3_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultTranspose3x3fNV)
    }
    glMatrixMultTranspose3x3fNV(
        matrixMode, reinterpret_cast<const GLfloat*>(m.data()));
    detail::error_check("MatrixMultTranspose3x3fNV"sv);
}

template<class span_const_void, class span_u32>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param fontTarget GLenum
     * \param fontName const void *
     * \param fontStyle GLbitfield
     * \param pathParameterTemplate GLuint
     * \param emScale GLfloat
     * \param baseAndCount GLuint *
     * \return GLenum
     */
    STATICINLINE GLenum path_glyph_index_range(
        GLenum                 fontTarget,
        span_const_void const& fontName,
        group::path_font_style fontStyle,
        u32                    pathParameterTemplate,
        f32                    emScale,
        span_u32               baseAndCount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathGlyphIndexRangeNV)
    }
    auto out = glPathGlyphIndexRangeNV(
        fontTarget,
        fontName.size() ? reinterpret_cast<const void*>(fontName.data())
                        : nullptr,
        static_cast<GLenum>(fontStyle),
        pathParameterTemplate,
        emScale,
        baseAndCount.size() ? reinterpret_cast<GLuint*>(baseAndCount.data())
                            : nullptr);
    detail::error_check("PathGlyphIndexRangeNV"sv);
    return out;
}

template<class span_const_f32, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param fillMode GLenum
     * \param mask GLuint
     * \param coverMode GLenum
     * \param transformType GLenum
     * \param transformValues const GLfloat *
     * \return void
     */
    STATICINLINE void stencil_then_cover_fill_path_instanced(
        i32                    numPaths,
        GLenum                 pathNameType,
        span_const_void const& paths,
        u32                    pathBase,
        GLenum                 fillMode,
        u32                    mask,
        GLenum                 coverMode,
        GLenum                 transformType,
        span_const_f32 const&  transformValues)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilThenCoverFillPathInstancedNV)
    }
    glStencilThenCoverFillPathInstancedNV(
        numPaths,
        pathNameType,
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        fillMode,
        mask,
        coverMode,
        transformType,
        transformValues.size()
            ? reinterpret_cast<const GLfloat*>(transformValues.data())
            : nullptr);
    detail::error_check("StencilThenCoverFillPathInstancedNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param fillMode GLenum
 * \param mask GLuint
 * \param coverMode GLenum
 * \return void
 */
STATICINLINE void stencil_then_cover_fill_path(
    u32 path, GLenum fillMode, u32 mask, GLenum coverMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilThenCoverFillPathNV)
    }
    glStencilThenCoverFillPathNV(path, fillMode, mask, coverMode);
    detail::error_check("StencilThenCoverFillPathNV"sv);
}

template<class span_const_f32, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param numPaths GLsizei
     * \param pathNameType GLenum
     * \param paths const void *
     * \param pathBase GLuint
     * \param reference GLint
     * \param mask GLuint
     * \param coverMode GLenum
     * \param transformType GLenum
     * \param transformValues const GLfloat *
     * \return void
     */
    STATICINLINE void stencil_then_cover_stroke_path_instanced(
        i32                    numPaths,
        GLenum                 pathNameType,
        span_const_void const& paths,
        u32                    pathBase,
        i32                    reference,
        u32                    mask,
        GLenum                 coverMode,
        GLenum                 transformType,
        span_const_f32 const&  transformValues)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilThenCoverStrokePathInstancedNV)
    }
    glStencilThenCoverStrokePathInstancedNV(
        numPaths,
        pathNameType,
        paths.size() ? reinterpret_cast<const void*>(paths.data()) : nullptr,
        pathBase,
        reference,
        mask,
        coverMode,
        transformType,
        transformValues.size()
            ? reinterpret_cast<const GLfloat*>(transformValues.data())
            : nullptr);
    detail::error_check("StencilThenCoverStrokePathInstancedNV"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param path GLuint
 * \param reference GLint
 * \param mask GLuint
 * \param coverMode GLenum
 * \return void
 */
STATICINLINE void stencil_then_cover_stroke_path(
    u32 path, i32 reference, u32 mask, GLenum coverMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilThenCoverStrokePathNV)
    }
    glStencilThenCoverStrokePathNV(path, reference, mask, coverMode);
    detail::error_check("StencilThenCoverStrokePathNV"sv);
}

template<class span_const_GLenum, class span_f32>
requires(semantic::concepts::Span<span_const_GLenum>&& std::is_same_v<
         std::decay_t<typename span_const_GLenum::value_type>,
         std::decay_t<GLenum>>&& semantic::concepts::Span<span_f32>&&
                                 std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param program GLuint
     * \param programInterface GLenum
     * \param index GLuint
     * \param propCount GLsizei
     * \param props const GLenum *
     * \param count GLsizei
     * \param length GLsizei *
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE void get_program_resourcefv(
        u32                      program,
        group::program_interface programInterface,
        u32                      index,
        i32                      propCount,
        span_const_GLenum const& props,
        i32&                     length,
        span_f32                 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourcefvNV)
        glIsProgram(program);
    }
    glGetProgramResourcefvNV(
        program,
        static_cast<GLenum>(programInterface),
        index,
        propCount,
        props.size() ? reinterpret_cast<const GLenum*>(props.data()) : nullptr,
        params.size(),
        &length,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetProgramResourcefvNV"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param firstPathName GLuint
     * \param fontTarget GLenum
     * \param fontName const void *
     * \param fontStyle GLbitfield
     * \param firstGlyphIndex GLuint
     * \param numGlyphs GLsizei
     * \param pathParameterTemplate GLuint
     * \param emScale GLfloat
     * \return GLenum
     */
    STATICINLINE GLenum path_glyph_index_array(
        u32                    firstPathName,
        GLenum                 fontTarget,
        span_const_void const& fontName,
        group::path_font_style fontStyle,
        u32                    firstGlyphIndex,
        i32                    numGlyphs,
        u32                    pathParameterTemplate,
        f32                    emScale)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathGlyphIndexArrayNV)
    }
    auto out = glPathGlyphIndexArrayNV(
        firstPathName,
        fontTarget,
        fontName.size() ? reinterpret_cast<const void*>(fontName.data())
                        : nullptr,
        static_cast<GLenum>(fontStyle),
        firstGlyphIndex,
        numGlyphs,
        pathParameterTemplate,
        emScale);
    detail::error_check("PathGlyphIndexArrayNV"sv);
    return out;
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param firstPathName GLuint
     * \param fontTarget GLenum
     * \param fontSize GLsizeiptr
     * \param fontData const void *
     * \param faceIndex GLsizei
     * \param firstGlyphIndex GLuint
     * \param numGlyphs GLsizei
     * \param pathParameterTemplate GLuint
     * \param emScale GLfloat
     * \return GLenum
     */
    STATICINLINE GLenum path_memory_glyph_index_array(
        u32                    firstPathName,
        GLenum                 fontTarget,
        GLsizeiptr             fontSize,
        span_const_void const& fontData,
        i32                    faceIndex,
        u32                    firstGlyphIndex,
        i32                    numGlyphs,
        u32                    pathParameterTemplate,
        f32                    emScale)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathMemoryGlyphIndexArrayNV)
    }
    auto out = glPathMemoryGlyphIndexArrayNV(
        firstPathName,
        fontTarget,
        fontSize,
        fontData.size() ? reinterpret_cast<const void*>(fontData.data())
                        : nullptr,
        faceIndex,
        firstGlyphIndex,
        numGlyphs,
        pathParameterTemplate,
        emScale);
    detail::error_check("PathMemoryGlyphIndexArrayNV"sv);
    return out;
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param program GLuint
     * \param location GLint
     * \param genMode GLenum
     * \param components GLint
     * \param coeffs const GLfloat *
     * \return void
     */
    STATICINLINE void program_path_fragment_input_gen(
        u32                   program,
        i32                   location,
        GLenum                genMode,
        i32                   components,
        span_const_f32 const& coeffs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramPathFragmentInputGenNV)
        glIsProgram(program);
    }
    glProgramPathFragmentInputGenNV(
        program,
        location,
        genMode,
        components,
        coeffs.size() ? reinterpret_cast<const GLfloat*>(coeffs.data())
                      : nullptr);
    detail::error_check("ProgramPathFragmentInputGenNV"sv);
}

#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param color GLenum
     * \param pname GLenum
     * \param value GLfloat *
     * \return void
     */
    STATICINLINE void get_path_color_genfv(
        group::path_color color, group::path_gen_mode pname, span_f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathColorGenfvNV)
    }
    glGetPathColorGenfvNV(
        static_cast<GLenum>(color),
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<GLfloat*>(value.data()) : nullptr);
    detail::error_check("GetPathColorGenfvNV"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param color GLenum
     * \param pname GLenum
     * \param value GLint *
     * \return void
     */
    STATICINLINE void get_path_color_geniv(
        group::path_color color, group::path_gen_mode pname, span_i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathColorGenivNV)
    }
    glGetPathColorGenivNV(
        static_cast<GLenum>(color),
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<GLint*>(value.data()) : nullptr);
    detail::error_check("GetPathColorGenivNV"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param texCoordSet GLenum
     * \param pname GLenum
     * \param value GLfloat *
     * \return void
     */
    STATICINLINE void get_path_tex_genfv(
        group::texture_unit  texCoordSet,
        group::path_gen_mode pname,
        span_f32             value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathTexGenfvNV)
    }
    glGetPathTexGenfvNV(
        static_cast<GLenum>(texCoordSet),
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<GLfloat*>(value.data()) : nullptr);
    detail::error_check("GetPathTexGenfvNV"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param texCoordSet GLenum
     * \param pname GLenum
     * \param value GLint *
     * \return void
     */
    STATICINLINE void get_path_tex_geniv(
        group::texture_unit  texCoordSet,
        group::path_gen_mode pname,
        span_i32             value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPathTexGenivNV)
    }
    glGetPathTexGenivNV(
        static_cast<GLenum>(texCoordSet),
        static_cast<GLenum>(pname),
        value.size() ? reinterpret_cast<GLint*>(value.data()) : nullptr);
    detail::error_check("GetPathTexGenivNV"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param color GLenum
     * \param genMode GLenum
     * \param colorFormat GLenum
     * \param coeffs const GLfloat *
     * \return void
     */
    STATICINLINE void path_color_gen(
        group::path_color        color,
        group::path_gen_mode     genMode,
        group::path_color_format colorFormat,
        span_const_f32 const&    coeffs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathColorGenNV)
    }
    glPathColorGenNV(
        static_cast<GLenum>(color),
        static_cast<GLenum>(genMode),
        static_cast<GLenum>(colorFormat),
        coeffs.size() ? reinterpret_cast<const GLfloat*>(coeffs.data())
                      : nullptr);
    detail::error_check("PathColorGenNV"sv);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_NV_path_rendering
 * \param genMode GLenum
 * \return void
 */
STATICINLINE void path_fog_gen(group::path_gen_mode genMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathFogGenNV)
    }
    glPathFogGenNV(static_cast<GLenum>(genMode));
    detail::error_check("PathFogGenNV"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param texCoordSet GLenum
     * \param genMode GLenum
     * \param components GLint
     * \param coeffs const GLfloat *
     * \return void
     */
    STATICINLINE void path_tex_gen(
        group::path_color     texCoordSet,
        group::path_gen_mode  genMode,
        i32                   components,
        span_const_f32 const& coeffs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PathTexGenNV)
    }
    glPathTexGenNV(
        static_cast<GLenum>(texCoordSet),
        static_cast<GLenum>(genMode),
        components,
        coeffs.size() ? reinterpret_cast<const GLfloat*>(coeffs.data())
                      : nullptr);
    detail::error_check("PathTexGenNV"sv);
}

#endif
/*!
 * \brief Part of GL_NV_path_rendering
 * \param mode GLenum
 * \param left GLdouble
 * \param right GLdouble
 * \param bottom GLdouble
 * \param top GLdouble
 * \param zNear GLdouble
 * \param zFar GLdouble
 * \return void
 */
STATICINLINE void matrix_frustum_ext(
    group::matrix_mode mode,
    f64                left,
    f64                right,
    f64                bottom,
    f64                top,
    f64                zNear,
    f64                zFar)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixFrustumEXT)
    }
    glMatrixFrustumEXT(
        static_cast<GLenum>(mode), left, right, bottom, top, zNear, zFar);
    detail::error_check("MatrixFrustumEXT"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param mode GLenum
 * \return void
 */
STATICINLINE void matrix_load_identity_ext(group::matrix_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadIdentityEXT)
    }
    glMatrixLoadIdentityEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixLoadIdentityEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLdouble *
     * \return void
     */
    STATICINLINE void matrix_load_transposed_ext(
        group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadTransposedEXT)
    }
    glMatrixLoadTransposedEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadTransposedEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE void matrix_load_transposef_ext(
        group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadTransposefEXT)
    }
    glMatrixLoadTransposefEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadTransposefEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLdouble *
     * \return void
     */
    STATICINLINE
    void matrix_loadd_ext(group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoaddEXT)
    }
    glMatrixLoaddEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixLoaddEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE
    void matrix_loadf_ext(group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixLoadfEXT)
    }
    glMatrixLoadfEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixLoadfEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLdouble *
     * \return void
     */
    STATICINLINE void matrix_mult_transposed_ext(
        group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultTransposedEXT)
    }
    glMatrixMultTransposedEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixMultTransposedEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE void matrix_mult_transposef_ext(
        group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultTransposefEXT)
    }
    glMatrixMultTransposefEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixMultTransposefEXT"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLdouble *
     * \return void
     */
    STATICINLINE
    void matrix_multd_ext(group::matrix_mode mode, span_const_f64 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultdEXT)
    }
    glMatrixMultdEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLdouble*>(m.data()) : nullptr);
    detail::error_check("MatrixMultdEXT"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param m const GLfloat *
     * \return void
     */
    STATICINLINE
    void matrix_multf_ext(group::matrix_mode mode, span_const_f32 const& m)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixMultfEXT)
    }
    glMatrixMultfEXT(
        static_cast<GLenum>(mode),
        m.size() ? reinterpret_cast<const GLfloat*>(m.data()) : nullptr);
    detail::error_check("MatrixMultfEXT"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param mode GLenum
 * \param left GLdouble
 * \param right GLdouble
 * \param bottom GLdouble
 * \param top GLdouble
 * \param zNear GLdouble
 * \param zFar GLdouble
 * \return void
 */
STATICINLINE void matrix_ortho_ext(
    group::matrix_mode mode,
    f64                left,
    f64                right,
    f64                bottom,
    f64                top,
    f64                zNear,
    f64                zFar)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixOrthoEXT)
    }
    glMatrixOrthoEXT(
        static_cast<GLenum>(mode), left, right, bottom, top, zNear, zFar);
    detail::error_check("MatrixOrthoEXT"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param mode GLenum
 * \return void
 */
STATICINLINE void matrix_pop_ext(group::matrix_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixPopEXT)
    }
    glMatrixPopEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixPopEXT"sv);
}

/*!
 * \brief Part of GL_NV_path_rendering
 * \param mode GLenum
 * \return void
 */
STATICINLINE void matrix_push_ext(group::matrix_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixPushEXT)
    }
    glMatrixPushEXT(static_cast<GLenum>(mode));
    detail::error_check("MatrixPushEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param angle GLdouble
     * \param x GLdouble
     * \param y GLdouble
     * \param z GLdouble
     * \return void
     */
    STATICINLINE void matrix_rotated_ext(
        group::matrix_mode mode, f64 angle, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixRotatedEXT)
    }
    glMatrixRotatedEXT(static_cast<GLenum>(mode), angle, x.x(), x.y(), x.z());
    detail::error_check("MatrixRotatedEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param angle GLfloat
     * \param x GLfloat
     * \param y GLfloat
     * \param z GLfloat
     * \return void
     */
    STATICINLINE void matrix_rotatef_ext(
        group::matrix_mode mode, f32 angle, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixRotatefEXT)
    }
    glMatrixRotatefEXT(static_cast<GLenum>(mode), angle, x.x(), x.y(), x.z());
    detail::error_check("MatrixRotatefEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param x GLdouble
     * \param y GLdouble
     * \param z GLdouble
     * \return void
     */
    STATICINLINE
    void matrix_scaled_ext(group::matrix_mode mode, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixScaledEXT)
    }
    glMatrixScaledEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixScaledEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param x GLfloat
     * \param y GLfloat
     * \param z GLfloat
     * \return void
     */
    STATICINLINE
    void matrix_scalef_ext(group::matrix_mode mode, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixScalefEXT)
    }
    glMatrixScalefEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixScalefEXT"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param x GLdouble
     * \param y GLdouble
     * \param z GLdouble
     * \return void
     */
    STATICINLINE
    void matrix_translated_ext(group::matrix_mode mode, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixTranslatedEXT)
    }
    glMatrixTranslatedEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixTranslatedEXT"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>)
    /*!
     * \brief Part of GL_NV_path_rendering
     * \param mode GLenum
     * \param x GLfloat
     * \param y GLfloat
     * \param z GLfloat
     * \return void
     */
    STATICINLINE
    void matrix_translatef_ext(group::matrix_mode mode, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MatrixTranslatefEXT)
    }
    glMatrixTranslatefEXT(static_cast<GLenum>(mode), x.x(), x.y(), x.z());
    detail::error_check("MatrixTranslatefEXT"sv);
}

} // namespace gl::nv::path_rendering
#endif // GL_NV_path_rendering
namespace gl::nv::path_rendering {
constexpr auto name = "GL_NV_path_rendering";
}
