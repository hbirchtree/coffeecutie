#pragma once

#include "rhi_features.h"
#include "rhi_versioning.h"

#include <glw/enums/DebugSeverity.h>
#include <glw/enums/ObjectIdentifier.h>
#include <glw/extensions/KHR_debug.h>

#include <peripherals/stl/source_location.h>

namespace gleam::debug {

struct null_api
{
    inline void enable()
    {
    }

    template<typename... Args>
    inline void add_callback(Args...)
    {
    }

    inline auto scope(std::string_view const& = {})
    {
        return nullptr;
    }

    inline void message(
        std::string_view const&,
        group::debug_severity = group::debug_severity::dont_care)
    {
    }

    inline void annotate(u32, std::string_view const&)
    {
    }

    template<typename T>
    inline void annotate(T&, std::string_view const&)
    {
    }
};

#if (GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320 || \
    defined(GL_KHR_debug)) && !defined(COFFEE_MINGW32)
struct scope
{
    scope(features::debugging debug, std::string_view const& name)
        : ext(debug)
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(ext.debug)
        {
            cmd::push_debug_group(
                group::debug_source::application, 0, name.size(), name);
            return;
        }
#endif
#if defined(GL_KHR_debug)
        if(ext.khr.debug)
            gl::khr::debug::push_debug_group(
                group::debug_source::application, 0, name.size(), name);
#endif
    }

    ~scope()
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(ext.debug)
        {
            cmd::pop_debug_group();
            return;
        }
#endif
#if defined(GL_KHR_debug)
        if(ext.khr.debug)
        {
            gl::khr::debug::pop_debug_group();
            return;
        }
#endif
    }

    features::debugging ext;
};

struct api
{
    api(features::debugging& debug)
        : ext(debug)
    {
    }

    inline auto scope(
        std::string_view const& name =
            stl_types::source_location().function_name())
    {
        return std::make_unique<debug::scope>(std::ref(ext), name);
    }

    inline void message(
        std::string_view const& msg,
        group::debug_severity   severity = group::debug_severity::notification)
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(ext.debug)
        {
            cmd::debug_message_insert(
                group::debug_source::application,
                group::debug_type::performance,
                0,
                severity,
                msg.size(),
                msg);
            return;
        }
#endif
#if defined(GL_KHR_debug)
        if(ext.khr.debug)
        {
            gl::khr::debug::debug_message_insert(
                group::debug_source::application,
                group::debug_type::performance,
                0,
                severity,
                msg.size(),
                msg);
            return;
        }
#endif
    }

    static inline void debug_func(
        GLenum        source,
        GLenum        type,
        GLuint        id,
        GLenum        severity,
        GLsizei       length,
        const GLchar* message,
        const void*   userParam)
    {
        using namespace group;
        auto self = reinterpret_cast<const api*>(userParam);

        for(auto const& callback : self->m_debugFunc)
            callback(
                static_cast<debug_source>(source),
                static_cast<debug_type>(type),
                id,
                static_cast<debug_severity>(severity),
                std::string_view(message, length));
    }

    inline void enable()
    {
        auto _ = scope(__PRETTY_FUNCTION__);
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(ext.debug)
        {
            cmd::enable(group::enable_cap::debug_output_synchronous);
            cmd::debug_message_callback(
                debug_func, semantic::Span<const api>(this, 1));
            cmd::debug_message_control(
                group::debug_source::dont_care,
                group::debug_type::dont_care,
                group::debug_severity::dont_care,
                null_span<u32>{},
                true);
            return;
        }
#endif
#if defined(GL_KHR_debug)
        if(ext.khr.debug)
        {
            cmd::enable(group::enable_cap::debug_output_synchronous);
            gl::khr::debug::debug_message_callback(
                debug_func,
                semantic::Span<const api>(this, 1));
            return;
        }
#endif
    }

    inline void annotate(
        group::object_identifier identifier,
        u32                      hnd,
        std::string_view const&  label)
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320
        if(ext.debug)
        {
            cmd::object_label(identifier, hnd, label.size(), label);
        }
#endif
#if defined(GL_KHR_debug)
        if(ext.khr.debug)
        {
            gl::khr::debug::object_label(identifier, hnd, label.size(), label);
        }
#endif
    }

    template<typename T>
    inline void annotate(T& object, std::string_view const& label)
    {
        annotate(T::debug_identifier, object.m_handle, label);
    }

    using debug_function  = stl_types::Function<void(
        group::debug_source,
        group::debug_type,
        u32,
        group::debug_severity,
        std::string_view const&)>;
    using simple_function = stl_types::Function<void(
        group::debug_severity, std::string_view const&)>;

    inline void add_callback(debug_function&& func)
    {
        if(!ext.debug && !ext.khr.debug)
            return;
        m_debugFunc.emplace_back(std::move(func));
    }

    inline void add_callback(simple_function&& func)
    {
        if(!ext.debug && !ext.khr.debug)
            return;
        m_debugFunc.emplace_back(
            [func](
                group::debug_source,
                group::debug_type,
                u32,
                group::debug_severity   severity,
                std::string_view const& message) { func(severity, message); });
    }

    std::vector<debug_function> m_debugFunc;
    features::debugging&        ext;
};

#define GLEAM_RHI_REAL_DEBUG_ENABLED 1
constexpr bool api_available = true;
#else
#define GLEAM_RHI_REAL_DEBUG_ENABLED 0
constexpr bool api_available = false;
using api                    = null_api;
#endif

} // namespace gleam::debug
