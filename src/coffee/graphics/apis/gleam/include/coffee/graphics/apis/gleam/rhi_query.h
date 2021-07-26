#pragma once

#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include <glw/extensions/EXT_disjoint_timer_query.h>

namespace gleam {
namespace detail {

inline void query_alloc(
    queries::type type, hnd& handle, features::queries& features)
{
#if GLEAM_MAX_VERSION >= 0x450
    if(features.dsa)
    {
        cmd::create_queries(convert::to(type), SpanOne<u32>(handle));
        return;
    }
#endif
#if defined(GL_EXT_disjoint_timer_query)
    if(features.disjoint_timer_query)
    {
        gl::ext::disjoint_timer_query::gen_queries(SpanOne<u32>(handle));
        return;
    }
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    cmd::gen_queries(SpanOne<u32>(handle));
#else
    Throw(unimplemented_path("allocator not available"));
#endif
}

inline void query_dealloc(hnd& handle, features::queries& features)
{
#if defined(GL_EXT_disjoint_timer_query)
    if(features.disjoint_timer_query)
    {
        gl::ext::disjoint_timer_query::delete_queries(SpanOne<u32>(handle));
        return;
    }
#endif
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
    cmd::delete_queries(SpanOne<u32>(handle));
#else
    Throw(unimplemented_path("allocator not available"));
#endif
}

} // namespace detail

#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
struct query_t
{
    query_t(features::queries& features, queries::type type) :
        m_features(features), m_type(type)
    {
    }

    inline void alloc()
    {
        detail::query_alloc(m_type, m_handle, m_features);
    }
    inline void dealloc()
    {
        detail::query_dealloc(m_handle, m_features);
        m_handle.release();
    }

    inline void start()
    {
        cmd::begin_query(convert::to(m_type), m_handle);
    }

    inline void stop()
    {
        cmd::end_query(convert::to(m_type));
    }

    inline i64 resultSync() const
    {
        u32 result = 0;
        cmd::get_query_objectuiv(
            m_handle,
            group::query_object_parameter_name::query_result,
            SpanOne(result));
        return result;
    }
    inline std::optional<i64> result() const
    {
        u32 available = 0;
        cmd::get_query_objectuiv(
            m_handle,
            group::query_object_parameter_name::query_result_available,
            SpanOne(available));
        if(!available)
            return std::nullopt;
        return resultSync();
    }

  private:
    features::queries m_features;
    hnd               m_handle;
    queries::type     m_type;
};
#endif

#if GLEAM_MAX_VERSION >= 0x300 || defined(GL_EXT_disjoint_timer_query)
struct timestamp_query
{
    timestamp_query(features::queries& features, queries::type) :
        m_features(features)
    {
    }

    inline void alloc()
    {
        detail::query_alloc(queries::type::timestamp, m_handle, m_features);
    }
    inline void dealloc()
    {
        detail::query_dealloc(m_handle, m_features);
        m_handle.release();
    }

    inline u64 sample()
    {
#if defined(GL_EXT_disjoint_timer_query)
        if(m_features.disjoint_timer_query)
        {
            gl::ext::disjoint_timer_query::query_counter(
                m_handle,
                static_cast<group::query_counter_target>(
                    gl::ext::disjoint_timer_query::values::timestamp));
        }
#elif GLEAM_MAX_VERSION >= 0x300
        {
            cmd::query_counter(
                m_handle, group::query_counter_target::timestamp);
        }
#else
        Throw(unimplemented_path("no query counter impl"));
#endif

        u32 result = 0;
#if defined(GL_EXT_disjoint_timer_query)
        if(m_features.disjoint_timer_query)
        {
            gl::ext::disjoint_timer_query::get_query_objectuiv(
                m_handle,
                static_cast<group::query_object_parameter_name>(
                    gl::ext::disjoint_timer_query::values::query_result),
                SpanOne(result));
            return result;
        }
#elif GLEAM_MAX_VERSION >= 0x300
        cmd::get_query_objectuiv(
            m_handle,
            group::query_object_parameter_name::query_result,
            SpanOne(result));
#endif
        return result;
    }

  private:
    features::queries m_features;
    hnd               m_handle;
};
#endif

struct null_query_t
{
    // clang-format off
    inline void alloc() {}
    inline void start() {}
    inline void stop() {}
    inline i64 resultSync() const { return 0; }
    inline std::optional<i64> result() const { return std::nullopt; }
    // clang-format on
};

#if defined(GL_EXT_disjoint_timer_query)
namespace ext_disjoint_timer {
namespace detail {
using namespace ::gl::ext::disjoint_timer_query;
}

struct query_t
{
    query_t(features::queries&, queries::type type) : m_type(type)
    {
    }

    inline void alloc()
    {
        detail::gen_queries(SpanOne<u32>(m_handle));
    }
    inline void dealloc()
    {
        detail::delete_queries(SpanOne<u32>(m_handle));
        m_handle.release();
    }

    inline void start()
    {
        detail::begin_query(convert::to(m_type), m_handle);
    }
    inline void stop()
    {
        detail::end_query(convert::to(m_type));
    }

    inline i64 resultSync() const
    {
        i32 result = 0;
        detail::get_query_objectiv(
            m_handle,
            static_cast<group::query_object_parameter_name>(
                gl::ext::disjoint_timer_query::values::query_result),
            SpanOne(result));
        return result;
    }
    inline std::optional<i64> result() const
    {
        i32 available = 0;
        detail::get_query_objectiv(
            m_handle,
            static_cast<group::query_object_parameter_name>(
                gl::ext::disjoint_timer_query::values::query_result),
            SpanOne(available));
        if(!available)
            return std::nullopt;
        return resultSync();
    }

  private:
    hnd           m_handle;
    queries::type m_type;
};

} // namespace ext_disjoint_timer
#endif
} // namespace gleam
