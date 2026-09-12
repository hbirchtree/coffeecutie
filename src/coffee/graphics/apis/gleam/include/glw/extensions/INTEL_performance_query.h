#pragma once

#ifdef GL_INTEL_performance_query
#include "../enums/PerfQueryDataFlags.h"
#include "../enums/PerformanceQueryCapsMaskINTEL.h"

namespace gl::intel::performance_query {
using gl::group::perf_query_data_flags;
using gl::group::performance_query_caps_mask_intel;

namespace values {
constexpr u32 perfquery_counter_event           = 0x94F0;
constexpr u32 perfquery_counter_duration_norm   = 0x94F1;
constexpr u32 perfquery_counter_duration_raw    = 0x94F2;
constexpr u32 perfquery_counter_throughput      = 0x94F3;
constexpr u32 perfquery_counter_raw             = 0x94F4;
constexpr u32 perfquery_counter_timestamp       = 0x94F5;
constexpr u32 perfquery_counter_data_uint32     = 0x94F8;
constexpr u32 perfquery_counter_data_uint64     = 0x94F9;
constexpr u32 perfquery_counter_data_float      = 0x94FA;
constexpr u32 perfquery_counter_data_double     = 0x94FB;
constexpr u32 perfquery_counter_data_bool32     = 0x94FC;
constexpr u32 perfquery_query_name_length_max   = 0x94FD;
constexpr u32 perfquery_counter_name_length_max = 0x94FE;
constexpr u32 perfquery_counter_desc_length_max = 0x94FF;
constexpr u32 perfquery_gpa_extended_counters   = 0x9500;
} // namespace values

/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryHandle GLuint
 * \return void
 */
STATICINLINE void begin_perf_query(
    u32 queryHandle, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BeginPerfQueryINTEL)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBeginPerfQueryINTEL, queryHandle);
#endif
    glBeginPerfQueryINTEL(queryHandle);
    detail::error_check("BeginPerfQueryINTEL"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryId GLuint
 * \param queryHandle GLuint *
 * \return void
 */
STATICINLINE void create_perf_query(
    u32         queryId,
    span_u32    queryHandle,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(CreatePerfQueryINTEL)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCreatePerfQueryINTEL, ::glw::trace::byte_span(queryHandle), queryId);
#endif
    glCreatePerfQueryINTEL(
        queryId,
        queryHandle.size() ? reinterpret_cast<GLuint*>(queryHandle.data())
                           : nullptr);
    detail::error_check("CreatePerfQueryINTEL"sv, check_errors);
}

/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryHandle GLuint
 * \return void
 */
STATICINLINE void delete_perf_query(
    u32 queryHandle, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DeletePerfQueryINTEL)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDeletePerfQueryINTEL, queryHandle);
#endif
    glDeletePerfQueryINTEL(queryHandle);
    detail::error_check("DeletePerfQueryINTEL"sv, check_errors);
}

/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryHandle GLuint
 * \return void
 */
STATICINLINE void end_perf_query(
    u32 queryHandle, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(EndPerfQueryINTEL)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEndPerfQueryINTEL, queryHandle);
#endif
    glEndPerfQueryINTEL(queryHandle);
    detail::error_check("EndPerfQueryINTEL"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryId GLuint *
 * \return void
 */
STATICINLINE void get_first_perf_query_id(
    span_u32 queryId, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetFirstPerfQueryIdINTEL)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetFirstPerfQueryIdINTEL, ::glw::trace::byte_span(queryId));
#endif
    glGetFirstPerfQueryIdINTEL(
        queryId.size() ? reinterpret_cast<GLuint*>(queryId.data()) : nullptr);
    detail::error_check("GetFirstPerfQueryIdINTEL"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryId GLuint
 * \param nextQueryId GLuint *
 * \return void
 */
STATICINLINE void get_next_perf_query_id(
    u32         queryId,
    span_u32    nextQueryId,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetNextPerfQueryIdINTEL)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetNextPerfQueryIdINTEL,
        ::glw::trace::byte_span(nextQueryId),
        queryId);
#endif
    glGetNextPerfQueryIdINTEL(
        queryId,
        nextQueryId.size() ? reinterpret_cast<GLuint*>(nextQueryId.data())
                           : nullptr);
    detail::error_check("GetNextPerfQueryIdINTEL"sv, check_errors);
}

template<class span_GLchar, class span_u32, class span_u64>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>> &&
    concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_u64> &&
    std::is_same_v<
        std::decay_t<typename span_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryId GLuint
 * \param counterId GLuint
 * \param counterNameLength GLuint
 * \param counterName GLchar *
 * \param counterDescLength GLuint
 * \param counterDesc GLchar *
 * \param counterOffset GLuint *
 * \param counterDataSize GLuint *
 * \param counterTypeEnum GLuint *
 * \param counterDataTypeEnum GLuint *
 * \param rawCounterMaxValue GLuint64 *
 * \return void
 */
STATICINLINE void get_perf_counter_info(
    u32         queryId,
    u32         counterId,
    span_GLchar counterName,
    span_GLchar counterDesc,
    span_u32    counterOffset,
    span_u32    counterDataSize,
    span_u32    counterTypeEnum,
    span_u32    counterDataTypeEnum,
    span_u64    rawCounterMaxValue,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetPerfCounterInfoINTEL)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetPerfCounterInfoINTEL,
        queryId,
        counterId,
        ::glw::trace::byte_span(counterName),
        ::glw::trace::byte_span(counterDesc),
        ::glw::trace::byte_span(counterOffset),
        ::glw::trace::byte_span(counterDataSize),
        ::glw::trace::byte_span(counterTypeEnum),
        ::glw::trace::byte_span(counterDataTypeEnum),
        ::glw::trace::byte_span(rawCounterMaxValue));
#endif
    glGetPerfCounterInfoINTEL(
        queryId,
        counterId,
        counterName.size(),
        counterName.data(),
        counterDesc.size(),
        counterDesc.data(),
        counterOffset.size() ? reinterpret_cast<GLuint*>(counterOffset.data())
                             : nullptr,
        counterDataSize.size()
            ? reinterpret_cast<GLuint*>(counterDataSize.data())
            : nullptr,
        counterTypeEnum.size()
            ? reinterpret_cast<GLuint*>(counterTypeEnum.data())
            : nullptr,
        counterDataTypeEnum.size()
            ? reinterpret_cast<GLuint*>(counterDataTypeEnum.data())
            : nullptr,
        rawCounterMaxValue.size()
            ? reinterpret_cast<GLuint64*>(rawCounterMaxValue.data())
            : nullptr);
    detail::error_check("GetPerfCounterInfoINTEL"sv, check_errors);
}

template<class span_u32, class span_void>
requires(
    concepts::span<span_void> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryHandle GLuint
 * \param flags GLuint
 * \param dataSize GLsizei
 * \param data void *
 * \param bytesWritten GLuint *
 * \return void
 */
STATICINLINE void get_perf_query_data(
    u32         queryHandle,
    u32         flags,
    i32         dataSize,
    span_void   data,
    span_u32    bytesWritten,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetPerfQueryDataINTEL)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetPerfQueryDataINTEL,
        queryHandle,
        flags,
        dataSize,
        ::glw::trace::byte_span(data),
        ::glw::trace::byte_span(bytesWritten));
#endif
    glGetPerfQueryDataINTEL(
        queryHandle,
        flags,
        dataSize,
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr,
        bytesWritten.size() ? reinterpret_cast<GLuint*>(bytesWritten.data())
                            : nullptr);
    detail::error_check("GetPerfQueryDataINTEL"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryName GLchar *
 * \param queryId GLuint *
 * \return void
 */
STATICINLINE void get_perf_query_id_by_name(
    span_u32 queryId, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetPerfQueryIdByNameINTEL)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetPerfQueryIdByNameINTEL, ::glw::trace::byte_span(queryId));
#endif
    glGetPerfQueryIdByNameINTEL(
        queryId.size() ? reinterpret_cast<GLuint*>(queryId.data()) : nullptr);
    detail::error_check("GetPerfQueryIdByNameINTEL"sv, check_errors);
}

template<class span_GLchar, class span_u32>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>> &&
    concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_INTEL_performance_query
 * \param queryId GLuint
 * \param queryNameLength GLuint
 * \param queryName GLchar *
 * \param dataSize GLuint *
 * \param noCounters GLuint *
 * \param noInstances GLuint *
 * \param capsMask GLuint *
 * \return void
 */
STATICINLINE void get_perf_query_info(
    u32         queryId,
    span_GLchar queryName,
    span_u32    dataSize,
    span_u32    noCounters,
    span_u32    noInstances,
    span_u32    capsMask,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetPerfQueryInfoINTEL)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetPerfQueryInfoINTEL,
        queryId,
        ::glw::trace::byte_span(queryName),
        ::glw::trace::byte_span(dataSize),
        ::glw::trace::byte_span(noCounters),
        ::glw::trace::byte_span(noInstances),
        ::glw::trace::byte_span(capsMask));
#endif
    glGetPerfQueryInfoINTEL(
        queryId,
        queryName.size(),
        queryName.data(),
        dataSize.size() ? reinterpret_cast<GLuint*>(dataSize.data()) : nullptr,
        noCounters.size() ? reinterpret_cast<GLuint*>(noCounters.data())
                          : nullptr,
        noInstances.size() ? reinterpret_cast<GLuint*>(noInstances.data())
                           : nullptr,
        capsMask.size() ? reinterpret_cast<GLuint*>(capsMask.data()) : nullptr);
    detail::error_check("GetPerfQueryInfoINTEL"sv, check_errors);
}

} // namespace gl::intel::performance_query
#endif // GL_INTEL_performance_query
namespace gl::intel::performance_query {
constexpr auto name = "GL_INTEL_performance_query";
}
