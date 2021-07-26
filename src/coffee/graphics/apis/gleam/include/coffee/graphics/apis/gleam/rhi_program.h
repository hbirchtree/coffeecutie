#pragma once

#include "rhi_debug.h"
#include "rhi_features.h"
#include "rhi_translate.h"
#include "rhi_versioning.h"

namespace gleam {
namespace detail {

inline stl_types::String shader_log(u32 handle)
{
    using shader_param = group::shader_parameter_name;
    stl_types::String info;
    i32               info_len{0};
    cmd::get_shaderiv(handle, shader_param::info_log_length, SpanOne(info_len));
    info.resize(info_len);
    cmd::get_shader_info_log(
        handle, info_len, semantic::Span<char>(info.data(), info.size()));
    return info;
}

inline stl_types::String program_log(u32 handle)
{
    using program_param = group::program_property_arb;
    stl_types::String info;
    i32               info_len{0};
    cmd::get_programiv(
        handle, program_param::info_log_length, SpanOne(info_len));
    info.resize(info_len);
    cmd::get_program_info_log(
        handle, info_len, semantic::Span<char>(info.data(), info.size()));
    return info;
}

} // namespace detail

struct shader_t
{
    using constants_t = stl_types::Map<stl_types::String, stl_types::String>;

    template<class span_data>
    requires semantic::concepts::Span<span_data> shader_t(
        span_data const& data, constants_t const& constants = {}) :
        m_data(reinterpret_cast<const char*>(data.data()), data.size()),
        m_constants(constants)
    {
    }

    semantic::Span<const char> m_data;
    constants_t                m_constants;
    hnd                        m_handle;
};

struct program_t
{
    using stage_t     = typing::graphics::ShaderStage;
    using stage_map_t = stl_types::Map<stage_t, stl_types::ShPtr<shader_t>>;

    using compile_error_t = std::tuple<stl_types::String>;
    using compile_log_t   = std::tuple<stl_types::String, int>;

    program_t(features::programs features, debug::api& debug) :
        m_features(features), m_debug(debug)
    {
    }

    void add(stage_t stage, stl_types::ShPtr<shader_t> shader)
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
    }

    NO_DISCARD semantic::result<compile_log_t, compile_error_t> compile()
    {
        auto _ = m_debug.scope(__PRETTY_FUNCTION__);
#if GLEAM_MAX_VERSION >= 0x430 || GLEAM_MAX_VERSION_ES >= 0x310
        if(m_features.separable_programs)
        {
            using pip_param = group::pipeline_parameter_name;

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
            stl_types::String log;
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
                using shader_param = group::shader_parameter_name;

                auto [stage_type, stage_info] = stage;

                if(stage_info->m_handle != 0)
                    continue;

                stage_info->m_handle = cmd::create_shader(
                    convert::to<group::shader_type>(stage_type));

                i32 data_length = static_cast<i32>(stage_info->m_data.size());
                cmd::shader_source(
                    stage_info->m_handle,
                    {
                        std::string_view(stage_info->m_data.data()),
                    },
                    SpanOne(data_length));
                cmd::compile_shader(stage_info->m_handle);
                auto info = detail::shader_log(stage_info->m_handle);

                i32 status{0};
                cmd::get_shaderiv(
                    stage_info->m_handle,
                    shader_param::compile_status,
                    SpanOne(status));
                if(!status)
                    return stl_types::failure(compile_error_t{info});

                cmd::attach_shader(m_handle, stage_info->m_handle);
            }

            using program_param = group::program_property_arb;

            cmd::link_program(m_handle);
            auto info = detail::program_log(m_handle);

            i32 link_status{0};
            cmd::get_programiv(
                m_handle, program_param::link_status, SpanOne(link_status));
            if(!link_status)
                return stl_types::failure(compile_error_t{info});

            return stl_types::success(compile_log_t{info, 1});
        }
    }

    features::programs m_features;
    debug::api         m_debug;
    stage_map_t        m_stages;
    hnd                m_handle;
};

} // namespace gleam
