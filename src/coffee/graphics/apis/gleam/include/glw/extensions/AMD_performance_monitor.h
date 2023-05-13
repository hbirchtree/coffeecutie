#pragma once

#ifdef GL_AMD_performance_monitor
namespace gl::amd::performance_monitor {
namespace values {
constexpr u32 counter_type             = 0x8BC0;
constexpr u32 counter_range            = 0x8BC1;
constexpr u32 unsigned_int64           = 0x8BC2;
constexpr u32 percentage               = 0x8BC3;
constexpr u32 perfmon_result_available = 0x8BC4;
constexpr u32 perfmon_result_size      = 0x8BC5;
constexpr u32 perfmon_result           = 0x8BC6;
} // namespace values
/*!
 * \brief Part of GL_AMD_performance_monitor
 * \param monitor GLuint
 * \return void
 */
STATICINLINE void begin_perf_monitor(u32 monitor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginPerfMonitorAMD)
    }
    glBeginPerfMonitorAMD(monitor);
    detail::error_check("BeginPerfMonitorAMD"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param n GLsizei
     * \param monitors GLuint *
     * \return void
     */
    STATICINLINE void delete_perf_monitors(span_u32 monitors)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeletePerfMonitorsAMD)
    }
    glDeletePerfMonitorsAMD(
        monitors.size(),
        monitors.size() ? reinterpret_cast<GLuint*>(monitors.data()) : nullptr);
    detail::error_check("DeletePerfMonitorsAMD"sv);
}

/*!
 * \brief Part of GL_AMD_performance_monitor
 * \param monitor GLuint
 * \return void
 */
STATICINLINE void end_perf_monitor(u32 monitor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndPerfMonitorAMD)
    }
    glEndPerfMonitorAMD(monitor);
    detail::error_check("EndPerfMonitorAMD"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param n GLsizei
     * \param monitors GLuint *
     * \return void
     */
    STATICINLINE void gen_perf_monitors(span_u32 monitors)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenPerfMonitorsAMD)
    }
    glGenPerfMonitorsAMD(
        monitors.size(),
        monitors.size() ? reinterpret_cast<GLuint*>(monitors.data()) : nullptr);
    detail::error_check("GenPerfMonitorsAMD"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param monitor GLuint
     * \param pname GLenum
     * \param dataSize GLsizei
     * \param data GLuint *
     * \param bytesWritten GLint *
     * \return void
     */
    STATICINLINE void get_perf_monitor_counter_data(
        u32      monitor,
        GLenum   pname,
        i32      dataSize,
        span_u32 data,
        i32&     bytesWritten)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPerfMonitorCounterDataAMD)
    }
    glGetPerfMonitorCounterDataAMD(
        monitor,
        pname,
        dataSize,
        data.size() ? reinterpret_cast<GLuint*>(data.data()) : nullptr,
        &bytesWritten);
    detail::error_check("GetPerfMonitorCounterDataAMD"sv);
}

template<class span_void>
requires(concepts::span<span_void>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param group GLuint
     * \param counter GLuint
     * \param pname GLenum
     * \param data void *
     * \return void
     */
    STATICINLINE void get_perf_monitor_counter_info(
        u32 group, u32 counter, GLenum pname, span_void data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPerfMonitorCounterInfoAMD)
    }
    glGetPerfMonitorCounterInfoAMD(
        group,
        counter,
        pname,
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetPerfMonitorCounterInfoAMD"sv);
}

template<class span_GLchar>
requires(concepts::span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param group GLuint
     * \param counter GLuint
     * \param bufSize GLsizei
     * \param length GLsizei *
     * \param counterString GLchar *
     * \return void
     */
    STATICINLINE void get_perf_monitor_counter_string(
        u32 group, u32 counter, i32& length, span_GLchar counterString)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPerfMonitorCounterStringAMD)
    }
    glGetPerfMonitorCounterStringAMD(
        group, counter, counterString.size(), &length, counterString.data());
    detail::error_check("GetPerfMonitorCounterStringAMD"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param group GLuint
     * \param numCounters GLint *
     * \param maxActiveCounters GLint *
     * \param counterSize GLsizei
     * \param counters GLuint *
     * \return void
     */
    STATICINLINE void get_perf_monitor_counters(
        u32 group, i32& numCounters, i32& maxActiveCounters, span_u32 counters)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPerfMonitorCountersAMD)
    }
    glGetPerfMonitorCountersAMD(
        group,
        &numCounters,
        &maxActiveCounters,
        counters.size(),
        counters.size() ? reinterpret_cast<GLuint*>(counters.data()) : nullptr);
    detail::error_check("GetPerfMonitorCountersAMD"sv);
}

template<class span_GLchar>
requires(concepts::span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param group GLuint
     * \param bufSize GLsizei
     * \param length GLsizei *
     * \param groupString GLchar *
     * \return void
     */
    STATICINLINE void get_perf_monitor_group_string(
        u32 group, i32& length, span_GLchar groupString)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPerfMonitorGroupStringAMD)
    }
    glGetPerfMonitorGroupStringAMD(
        group, groupString.size(), &length, groupString.data());
    detail::error_check("GetPerfMonitorGroupStringAMD"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param numGroups GLint *
     * \param groupsSize GLsizei
     * \param groups GLuint *
     * \return void
     */
    STATICINLINE void get_perf_monitor_groups(i32& numGroups, span_u32 groups)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPerfMonitorGroupsAMD)
    }
    glGetPerfMonitorGroupsAMD(
        &numGroups,
        groups.size(),
        groups.size() ? reinterpret_cast<GLuint*>(groups.data()) : nullptr);
    detail::error_check("GetPerfMonitorGroupsAMD"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_AMD_performance_monitor
     * \param monitor GLuint
     * \param enable GLboolean
     * \param group GLuint
     * \param numCounters GLint
     * \param counterList GLuint *
     * \return void
     */
    STATICINLINE void select_perf_monitor_counters(
        u32 monitor, bool enable, u32 group, span_u32 counterList)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SelectPerfMonitorCountersAMD)
    }
    glSelectPerfMonitorCountersAMD(
        monitor,
        enable,
        group,
        counterList.size(),
        counterList.size() ? reinterpret_cast<GLuint*>(counterList.data())
                           : nullptr);
    detail::error_check("SelectPerfMonitorCountersAMD"sv);
}

} // namespace gl::amd::performance_monitor
#endif // GL_AMD_performance_monitor
namespace gl::amd::performance_monitor {
constexpr auto name = "GL_AMD_performance_monitor";
}
