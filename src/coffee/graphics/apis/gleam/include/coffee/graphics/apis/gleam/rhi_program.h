#pragma once

#include "rhi_debug.h"
#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

#include <peripherals/error/result.h>

#include <glw/extensions/KHR_parallel_shader_compile.h>

namespace gleam {
namespace detail {

inline std::string shader_log(u32 handle)
{
    using shader_param = group::shader_parameter_name;
    std::string info;
    i32         info_len{0};
    cmd::get_shaderiv(handle, shader_param::info_log_length, SpanOne(info_len));
    if(info_len == 0)
        return {};
    info.resize(info_len);
    cmd::get_shader_info_log(
        handle, info_len, semantic::Span<char>(info.data(), info.size()));
    return info;
}

inline std::string program_log(u32 handle)
{
    using program_param = group::program_property_arb;
    std::string info;
    i32         info_len{0};
    cmd::get_programiv(
        handle, program_param::info_log_length, SpanOne(info_len));
    info.resize(info_len);
    cmd::get_program_info_log(
        handle, info_len, semantic::Span<char>(info.data(), info.size()));
    return info;
}

using namespace std::string_view_literals;

constexpr auto default_entrypoint = "main"sv;

} // namespace detail

using shader_format_t = semantic::concepts::graphics::programs::shader_format_t;

struct shader_t
{
    using constants_t = std::map<std::string, std::string>;

    template<typename CharType>
    shader_t(
        semantic::mem_chunk<CharType> const& data,
        constants_t const&                   constants = {})
        : shader_t(data.view, constants)
    {
    }

    template<class span_data>
    requires semantic::concepts::Span<span_data>
    shader_t(span_data const& data, constants_t const& constants = {})
        : m_data(reinterpret_cast<const char*>(data.data()), data.size())
        , m_constants(constants)
    {
    }

    template<class span_data>
    requires semantic::concepts::Span<span_data>
    shader_t(
        span_data const&   data,
        shader_format_t    format,
        constants_t const& constants  = {},
        std::string_view   entrypoint = detail::default_entrypoint)
        : m_data(reinterpret_cast<const char*>(data.data()), data.size())
        , m_constants(constants)
        , m_entrypoint(entrypoint)
        , m_format(format)
    {
    }

    semantic::Span<const char> m_data;
    constants_t                m_constants;
    std::string_view           m_entrypoint;
    hnd                        m_handle;
    shader_format_t            m_format{shader_format_t::source};
};

struct program_t
{
    static constexpr auto debug_identifier = group::object_identifier::program;

    using stage_t     = typing::graphics::ShaderStage;
    using stage_map_t = std::map<stage_t, std::shared_ptr<shader_t>>;

    using compile_error_t = std::tuple<std::string>;
    using compile_log_t = std::tuple<std::string /* log text*/, int /* ??? */>;

    program_t(features::programs features, debug::api& debug)
        : m_features(features)
        , m_debug(debug)
    {
    }

    void add(stage_t stage, std::shared_ptr<shader_t> shader)
    {
        m_stages.insert({stage, shader});
    }

    void dealloc()
    {
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
        if(m_features.separable_programs)
        {
            cmd::delete_program_pipelines(SpanOne<u32>(m_handle));
            for(auto& shader : m_stages)
            {
                cmd::delete_program(shader.second->m_handle);
                shader.second->m_handle.release();
            }
        } else
#endif
        {
            cmd::delete_program(m_handle);
            for(auto& shader : m_stages)
            {
                cmd::delete_shader(shader.second->m_handle);
                shader.second->m_handle.release();
            }
        }
        m_handle.release();
        m_stages.clear();
    }

    semantic::result<compile_log_t, compile_error_t> validate_shader(
        shader_t const* stage_info)
    {
        auto info = detail::shader_log(stage_info->m_handle);

        i32 status{0};
        cmd::get_shaderiv(
            stage_info->m_handle,
            gl::group::shader_parameter_name::compile_status,
            SpanOne(status));
        if(!status)
            return stl_types::failure(compile_error_t{info});
        return stl_types::success(compile_log_t{info, 1});
    }

    semantic::result<compile_log_t, compile_error_t> validate_program()
    {
        auto info = detail::program_log(m_handle);

        i32 link_status{0};
        cmd::get_programiv(
            m_handle,
            gl::group::program_property_arb::link_status,
            SpanOne(link_status));
        if(!link_status)
            return stl_types::failure(compile_error_t{info});

        return stl_types::success(compile_log_t{info, 1});
    }

    NO_DISCARD semantic::result<compile_log_t, compile_error_t> compile()
    {
        m_buffer_locations.clear();
        m_uniform_locations.clear();

        [[maybe_unused]] auto _ = m_debug.scope(__PRETTY_FUNCTION__);
#if GLEAM_MAX_VERSION >= 0x460
        while(m_features.spirv)
        {
            using shader_def  = std::pair<stage_t, std::shared_ptr<shader_t>>;
            auto contains_spv = std::any_of(
                m_stages.begin(), m_stages.end(), [](shader_def const& shader) {
                    return shader.second->m_format == shader_format_t::spv;
                });

            if(!contains_spv)
                break;

            m_handle = cmd::create_program();

            for(auto const& [stage, shader] : m_stages)
            {
                shader->m_handle =
                    cmd::create_shader(convert::to<group::shader_type>(stage));
                cmd::shader_binary(
                    semantic::SpanOne<u32>(shader->m_handle),
                    group::shader_binary_format::spir_v,
                    shader->m_data,
                    shader->m_data.size());
                cmd::specialize_shader(
                    shader->m_handle,
                    shader->m_entrypoint,
                    null_span<u32>{},
                    null_span<u32>{});
                i32 status{0};
                cmd::get_shaderiv(
                    shader->m_handle,
                    group::shader_parameter_name::compile_status,
                    SpanOne(status));
                if(!status)
                {
                    return stl_types::failure(
                        compile_error_t{detail::shader_log(shader->m_handle)});
                }
                cmd::attach_shader(m_handle, shader->m_handle);
            }

            cmd::link_program(m_handle);
            i32 status{0};
            cmd::get_programiv(
                m_handle,
                group::program_property_arb::link_status,
                SpanOne(status));
            if(!status)
            {
                return stl_types::failure(
                    compile_error_t{detail::program_log(m_handle)});
            }

            return stl_types::success(compile_log_t{});
        }
#endif

#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
        if(m_features.separable_programs)
        {
            using pip_param = group::pipeline_parameter_name;

            // TODO: KHR_parallel_shader_compile

#if GLEAM_MAX_VERSION >= 0x450
            if(m_features.dsa)
                cmd::create_program_pipelines(SpanOne<u32>(m_handle));
            else
#endif
            {
                cmd::gen_program_pipelines(SpanOne<u32>(m_handle));
                cmd::bind_program_pipeline(m_handle);
            }
            for(auto const& stage : m_stages)
            {
                auto [stage_type, stage_info] = stage;

                auto const& shader_data = stage_info->m_data;
                stage_info->m_handle    = cmd::create_shader_programv(
                    convert::to<group::shader_type>(stage_type),
                    {std::string_view(shader_data.data(), shader_data.size())});

                auto log = detail::program_log(stage_info->m_handle);

                if(stage_info->m_handle == 0)
                    return stl_types::failure(compile_error_t{log});

                cmd::active_shader_program(m_handle, stage_info->m_handle);
            }

            cmd::validate_program_pipeline(m_handle);

            i32 log_len{0};
            cmd::get_program_pipelineiv(
                m_handle, pip_param::info_log_length, SpanOne(log_len));
            std::string log;
            log.resize(log_len);
            cmd::get_program_pipeline_info_log(
                m_handle,
                log_len,
                semantic::Span<char>(log.data(), log.size()));

            return stl_types::success(compile_log_t{log, 0});
        } else
#endif
        {
            m_handle = cmd::create_program();

            for(auto const& stage : m_stages)
            {
                auto [stage_type, stage_info] = stage;

                if(stage_info->m_handle != 0)
                    continue;

                stage_info->m_handle = cmd::create_shader(
                    convert::to<group::shader_type>(stage_type));

                i32 data_length = static_cast<i32>(stage_info->m_data.size());

                if(stage_info->m_data.empty())
                    return stl_types::failure(compile_error_t{
                        "No data provided for one of the shaders"});

                cmd::shader_source(
                    stage_info->m_handle,
                    {
                        std::string_view(
                            stage_info->m_data.data(),
                            stage_info->m_data.size()),
                    },
                    SpanOne(data_length));
                cmd::compile_shader(stage_info->m_handle);
                if(!m_features.khr.parallel_shader_compile)
                {
                    if(auto res = validate_shader(stage_info.get());
                       res.has_error())
                        return res.error();
                } else
                    m_async_waiting = true;

                cmd::attach_shader(m_handle, stage_info->m_handle);
            }

            cmd::link_program(m_handle);

            if(!m_features.khr.parallel_shader_compile)
            {
                return validate_program();
            } else
                return stl_types::success(compile_log_t{
                    "Waiting with KHR_parallel_shader_compile", 1});
        }
    }

    stl_types::result<bool, compile_error_t> check_async_ready()
    {
        if(!m_features.khr.parallel_shader_compile)
            return stl_types::success(true);
#if defined(GL_KHR_parallel_shader_compile)
        if(m_error_state)
            return stl_types::failure(compile_error_t{});
        if(m_features.separable_programs)
            return stl_types::success(false);
        else
        {
            i32 status = 0;
            cmd::get_programiv(
                m_handle,
                gl::group::program_property_arb::completion_status_khr,
                semantic::SpanOne(status));
            if(status == GL_FALSE)
                return stl_types::success(false);
            std::string error_log;
            if(auto res = validate_program(); res.has_value())
            {
                m_async_waiting = false;
                return stl_types::success(true);
            } else
                error_log = std::get<0>(res.error());
            for(auto const& stage : m_stages)
            {
                auto [stage_type, stage_info] = stage;
                if(auto res = validate_shader(stage_info.get());
                   res.has_error())
                    error_log += "\nShader log:" + std::get<0>(res.error());
            }
            m_error_state = true;
            return stl_types::failure(compile_error_t{error_log});
        }
#else
        return false;
#endif
    }

    features::programs m_features;
    debug::api         m_debug;
    stage_map_t        m_stages;
    hnd                m_handle;
    bool               m_async_waiting{false};
    bool               m_error_state{false};

    std::vector<std::pair<std::string_view, u32>> m_attribute_names;
    /*! Buffer location + queried size*/
    std::map<std::string, std::pair<u32, i32>> m_buffer_locations;
    std::map<std::string, u32>                 m_uniform_locations;
};

} // namespace gleam

#define GLSHADER(source) #source
