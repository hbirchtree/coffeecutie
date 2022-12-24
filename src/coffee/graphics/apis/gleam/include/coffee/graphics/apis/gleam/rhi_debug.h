#pragma once

#include "rhi_features.h"
#include "rhi_translate.h"

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

    inline auto scope(std::string_view const&)
    {
        return nullptr;
    }

    inline void message(std::string_view const&)
    {
    }
};

#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x320

struct scope
{
    scope(std::string_view const& name)
    {
        cmd::push_debug_group(
            group::debug_source::application, 0, name.size(), name);
    }
    ~scope()
    {
        cmd::pop_debug_group();
    }
};

struct api
{
    inline auto scope(std::string_view const& name)
    {
        return stl_types::MkUq<debug::scope>(name);
    }

    inline void message(std::string_view const& msg)
    {
        cmd::debug_message_insert(
            group::debug_source::application,
            group::debug_type::performance,
            0,
            group::debug_severity::notification,
            msg.size(),
            msg);
    }

    inline void enable()
    {
        auto _ = scope(__PRETTY_FUNCTION__);
        cmd::enable(group::enable_cap::debug_output_synchronous);
        cmd::debug_message_callback(
            [](GLenum        source,
               GLenum        type,
               GLuint        id,
               GLenum        severity,
               GLsizei       length,
               const GLchar* message,
               const void*   userParam) {
                using namespace group;
                auto self = reinterpret_cast<const api*>(userParam);

                for(auto const& callback : self->m_debugFunc)
                    callback(
                        static_cast<debug_source>(source),
                        static_cast<debug_type>(type),
                        id,
                        static_cast<debug_severity>(severity),
                        std::string_view(message, length));
            },
            semantic::Span<const api>(this, 1));
//        cmd::debug_message_control(
//            group::debug_source::dont_care,
//            group::debug_type::dont_care,
//            group::debug_severity::dont_care,
//            null_span<u32>{},
//            true);
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
        m_debugFunc.emplace_back(std::move(func));
    }

    inline void add_callback(simple_function&& func)
    {
        m_debugFunc.emplace_back(
            [func](
                group::debug_source,
                group::debug_type,
                u32,
                group::debug_severity   severity,
                std::string_view const& message) { func(severity, message); });
    }

    stl_types::Vector<debug_function> m_debugFunc;
};

constexpr bool api_available = true;
#else
constexpr bool api_available = false;
using api                    = null_api;
#endif

} // namespace gleam::debug
