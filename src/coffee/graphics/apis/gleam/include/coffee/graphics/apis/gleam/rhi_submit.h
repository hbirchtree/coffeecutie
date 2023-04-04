#pragma once

#include "rhi.h"
#include "rhi_compat.h"
#include "rhi_translate.h"
#include "rhi_uniforms.h"

namespace gleam {
namespace detail {

inline std::optional<error> evaluate_draw_state(
    api_limits const& limits, draw_command const& command)
{
    auto const& call = command.call;
    auto const& data = command.data;
    if(call.indexed)
    {
        i32 current_binding{0};
        cmd::get_integerv(
            group::get_prop::element_array_buffer_binding,
            SpanOne(current_binding));
        if(current_binding == 0)
            return error::draw_no_element_buffer;
        if(stl_types::any_of(
               data, [](auto const& d) { return d.elements.count == 0; }))
            return error::draw_no_elements;
        for(auto const& d : data)
        {
            if(d.elements.count > limits.draws.element_count
               /*|| d.elements.offset > 1024 * 1024*/)
                return error::draw_unsupported_call;
            /* We can't check if we exceed the max index vertex */
        }
    } else
    {
        if(stl_types::any_of(
               data, [](auto const& d) { return d.arrays.count == 0; }))
            return error::draw_no_arrays;
        for(auto const& d : data)
            if(d.arrays.count > 1024 * 1024 || d.arrays.offset > 1024 * 1024)
                return error::draw_unsupported_call;
    }

    if(call.instanced)
    {
        for(auto const& d : data)
            if(d.instances.count > limits.draws.instance_count
               || d.instances.offset > limits.draws.instance_offset)
                return error::draw_unsupported_call;
    }

    return std::nullopt;
}

inline constexpr auto unsupported_drawcall()
{
    using namespace std::string_view_literals;
    return std::make_tuple(
        error::no_implementation_for_draw_call,
        "draw call not implemented on this api"sv);
}

inline std::optional<std::tuple<error, std::string_view>> legacy_draw(
    draw_command::call_spec_t const& call, draw_command::data_t const& data)
{
    if(call.indexed)
    {
        cmd::draw_elements(
            convert::to(call.mode),
            data.elements.count,
            convert::to<group::draw_elements_type>(data.elements.type),
            data.elements.offset);
    } else
    {
        cmd::draw_arrays(
            convert::to(call.mode), data.arrays.offset, data.arrays.count);
    }
    return std::nullopt;
}

inline std::optional<std::tuple<error, std::string_view>> direct_draw(
    draw_command::call_spec_t const& call,
    draw_command::data_t const&      data,
    shader_bookkeeping_t&            bookkeeping,
    workarounds const&               workarounds)
{
    [[maybe_unused]] const auto base_instance
        = data.instances.offset != 0
          && !workarounds.draw.emulated_base_instance;
    const auto instanced = call.instanced || data.instances.offset > 0
                           || (call.indexed && data.elements.vertex_offset > 0);

    if(call.indexed)
    {
        [[maybe_unused]] const auto base_vertex
            = data.elements.vertex_offset != 0
              && !workarounds.draw.emulated_vertex_offset;
        if(instanced)
        {
            bookkeeping.baseInstance = data.instances.offset;
#if GLEAM_MAX_VERSION_ES >= 0x300 || GLEAM_MAX_VERSION > 0
            if(base_instance && base_vertex)
            {
#if GLEAM_MAX_VERSION >= 0x420
                cmd::draw_elements_instanced_base_vertex_base_instance(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count,
                    data.elements.vertex_offset,
                    data.instances.offset);
                return std::nullopt;
#endif
            } else if(base_instance)
            {
#if GLEAM_MAX_VERSION >= 0x420
                cmd::draw_elements_instanced_base_instance(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count,
                    data.instances.offset);
                return std::nullopt;
#endif
            } else if(base_vertex)
            {
#if GLEAM_MAX_VERSION >= 0x320 || GLEAM_MAX_VERSION_ES >= 0x320
                cmd::draw_elements_instanced_base_vertex(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count,
                    data.elements.vertex_offset);
                return std::nullopt;
#endif
            } else
            {
                cmd::draw_elements_instanced(
                    convert::to(call.mode),
                    data.elements.count,
                    convert::to<group::draw_elements_type>(data.elements.type),
                    data.elements.offset,
                    data.instances.count);
                return std::nullopt;
            }
#endif
            return detail::unsupported_drawcall();
        } else
            cmd::draw_elements(
                convert::to(call.mode),
                data.elements.count,
                convert::to<group::draw_elements_type>(data.elements.type),
                data.elements.offset);
    } else
    {
        if(instanced)
        {
            bookkeeping.baseInstance = data.instances.offset;
#if GLEAM_MAX_VERSION_ES != 0x200
            if(base_instance)
            {
#if GLEAM_MAX_VERSION >= 0x420
                cmd::draw_arrays_instanced_base_instance(
                    convert::to(call.mode),
                    data.arrays.offset,
                    data.arrays.count,
                    data.instances.count,
                    data.instances.offset);
                return std::nullopt;
#endif
            } else
            {
                cmd::draw_arrays_instanced(
                    convert::to(call.mode),
                    data.arrays.offset,
                    data.arrays.count,
                    data.instances.count);
                return std::nullopt;
            }
#endif
            return detail::unsupported_drawcall();
        } else
            cmd::draw_arrays(
                convert::to(call.mode), data.arrays.offset, data.arrays.count);
    }
    return std::nullopt;
}

union alignas(4) indirect_command_buffer
{
    struct elements_indirect_t
    {
        u32 count;
        u32 instanceCount;
        u32 first;
        i32 baseVertex;
        u32 baseInstance;
    } elements;
    struct arrays_indirect_t
    {
        u32 count;
        u32 instanceCount;
        u32 first;
        u32 baseInstance;
    } arrays;
};

inline size_t element_size(draw_command::data_t const& data)
{
    using semantic::TypeEnum;
    switch(data.elements.type)
    {
    case TypeEnum::UByte:
        return 1;
    case TypeEnum::UShort:
        return 2;
    case TypeEnum::UInt:
        return 4;
    default:
        Throw(std::runtime_error("unsupported element type"));
    }
}

inline void create_draw(
    draw_command::call_spec_t const& call,
    draw_command::data_t const&      data,
    indirect_command_buffer*         buffer)
{
    if(call.indexed)
        buffer->elements = {
            .count         = data.elements.count,
            .instanceCount = data.instances.count,
            .first
            = static_cast<u32>(data.elements.offset / element_size(data)),
            .baseVertex   = static_cast<i32>(data.elements.vertex_offset),
            .baseInstance = data.instances.offset,
        };
    else
        buffer->arrays = {
            .count         = data.arrays.count,
            .instanceCount = data.instances.count,
            .first         = data.arrays.offset,
            .baseInstance  = data.instances.offset,
        };
}

inline void indirect_draw(
    [[maybe_unused]] draw_command::call_spec_t const& call,
    [[maybe_unused]] draw_command::data_t const&      data,
    [[maybe_unused]] circular_buffer_t&                  buffer)
{
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION_ES >= 0x310
    indirect_command_buffer cmd{};
    create_draw(call, data, &cmd);

    buffer.buffer().commit(SpanOne(cmd));
    cmd::bind_buffer(
        group::buffer_target_arb::draw_indirect_buffer,
        buffer.buffer().handle());

    if(call.indexed)
        cmd::draw_elements_indirect(
            convert::to(call.mode),
            convert::to<group::draw_elements_type>(data.elements.type),
            0);
    else
        cmd::draw_arrays_indirect(convert::to(call.mode), 0);
    cmd::bind_buffer(group::buffer_target_arb::draw_indirect_buffer, 0);
#endif
}

inline void multi_indirect_draw(
    [[maybe_unused]] draw_command::call_spec_t const&    call,
    [[maybe_unused]] decltype(draw_command::data) const& data,
    [[maybe_unused]] circular_buffer_t&                  buffer)
{
#if GLEAM_MAX_VERSION >= 0x430
    std::vector<indirect_command_buffer> cmds;
    cmds.resize(data.size());
    size_t i = 0;
    stl_types::for_each(data, [&call, &cmds, &i](auto const& d) {
        create_draw(call, d, &cmds.at(i++));
    });

    auto commands
        = semantic::SpanOver<indirect_command_buffer>(cmds.begin(), cmds.end());
    //    auto fence = buffer.push(buffer.move_fences(commands), commands);

    buffer.buffer().commit(commands);

    cmd::bind_buffer(
        group::buffer_target_arb::draw_indirect_buffer,
        buffer.buffer().handle());
    if(call.indexed)
    {
        auto element_type = data.at(0).elements.type;
        cmd::multi_draw_elements_indirect(
            convert::to(call.mode),
            convert::to<group::draw_elements_type>(element_type),
            reinterpret_cast<uintptr_t>(0ul /*fence.start*/),
            cmds.size(),
            sizeof(decltype(cmds)::value_type));
    } else
    {
        cmd::multi_draw_arrays_indirect(
            convert::to(call.mode),
            reinterpret_cast<uintptr_t>(0ul /*fence.start*/),
            cmds.size(),
            sizeof(indirect_command_buffer));
    }
    cmd::bind_buffer(group::buffer_target_arb::draw_indirect_buffer, 0);
//    buffer.add_fence(std::move(fence));
#endif
}

} // namespace detail

template<typename... MList>
inline optional<tuple<error, std::string_view>> api::submit(
    draw_command const& command, MList&&... modifiers)
{
    using namespace std::string_view_literals;

    auto const& call          = command.call;
    auto const& data          = command.data;
    auto        program       = command.program.lock();
    auto        render_target = command.render_target.expired()
                                    ? default_rendertarget()
                                    : command.render_target.lock();

    if(data.empty())
        return std::nullopt;

    [[maybe_unused]] auto _ = debug().scope();

    render_target->internal_bind(
#if GLEAM_MAX_VERSION >= 0x300 || GLEAM_MAX_VERSION_ES >= 0x300
        group::framebuffer_target::draw_framebuffer);
#else
        group::framebuffer_target::framebuffer);
#endif

    const bool uses_baseinstance = stl_types::any_of(
        data, [](auto const& d) { return d.instances.offset > 0; });
    const bool uses_vertex_offset
        = call.indexed && stl_types::any_of(data, [](auto const& d) {
              return d.elements.vertex_offset > 0;
          });
    if(m_api_type == api_type_t::es && uses_baseinstance
       && !m_workarounds.draw.emulated_base_instance)
        return std::make_tuple(
            error::no_implementation_for_draw_call,
            "baseinstance not enabled in GL ES"sv);
    if(m_api_type == api_type_t::es && m_api_version == 0x200 && call.instanced
       && !m_workarounds.draw.emulated_instance_id)
        return std::make_tuple(
            error::no_implementation_for_draw_call,
            "instancing not enabled in GL ES 2.0"sv);
    if(m_api_type == api_type_t::es && m_api_version <= 0x300
       && uses_vertex_offset && !m_workarounds.draw.emulated_vertex_offset)
        return std::make_tuple(
            error::no_implementation_for_draw_call,
            "base vertex not enabled in GL ES 3.0 and below"sv);

    if(!program || program->m_handle == 0)
        return std::make_tuple(error::no_program, "no program provided");

    auto vao = command.vertices.lock();

    if(!vao)
        return std::make_tuple(error::no_data, "no vertex data provided");

    std::shared_ptr<buffer_t> element_buf;
#if GLEAM_MAX_VERSION_ES != 0x200
    if(m_features.vertex.layout_binding)
    {
        cmd::bind_vertex_array(vao->m_handle);
    } else if(m_features.vertex.vertex_arrays)
    {
        while(!m_features.vertex.layout_binding
              || m_workarounds.draw.force_vertex_attrib_names)
        {
            if(program->m_attribute_names == vao->m_attribute_names)
                break;
            for(auto const& attrib : vao->m_attribute_names)
                cmd::bind_attrib_location(
                    program->m_handle, attrib.second, attrib.first);
            cmd::link_program(program->m_handle);
            program->m_attribute_names = vao->m_attribute_names;
            break;
        }
        cmd::bind_vertex_array(vao->m_handle);
    } else
#endif
    {
        using buffer_target = group::buffer_target_arb;

        for(auto const& attrib : vao->m_attribute_names)
            cmd::bind_attrib_location(
                program->m_handle, attrib.second, attrib.first);
        cmd::link_program(program->m_handle);
        for(auto const& attrib : vao->m_attributes)
        {
            cmd::enable_vertex_attrib_array(attrib.index);
            cmd::bind_buffer(
                buffer_target::array_buffer,
                vao->m_buffers.at(attrib.buffer.id).lock()->m_handle);
            detail::vertex_setup_attribute(attrib);
        }

        if(!vao->m_element_buffer.expired())
        {
            element_buf = vao->m_element_buffer.lock();
            cmd::bind_buffer(
                buffer_target::element_array_buffer, element_buf->m_handle);
        }
    }

    cmd::use_program(program->m_handle);

    {
        u32 vertex_program
            = m_features.program.separable_programs
                  ? program->m_stages.at(program_t::stage_t::Vertex)->m_handle
                  : program->m_handle;
        for(auto const& attrib : vao->m_attribute_names)
            cmd::bind_attrib_location(
                vertex_program, attrib.second, attrib.first);
    }

    detail::shader_bookkeeping_t bookkeeping{};
    (detail::apply_command_modifier(*program, bookkeeping, modifiers), ...);

    std::function<void()>    apply_instance_id   = []() {};
    std::function<void()>    apply_base_instance = []() {};
    std::function<void(u32)> apply_vertex_offset = [](u32) {};

    if(m_workarounds.draw.emulated_instance_id)
    {
        if constexpr(compile_info::debug_mode)
        {
            auto loc = detail::get_program_uniform_location(
                *program, program_t::stage_t::Vertex, {"glw_InstanceID"sv});
            if(loc == invalid_uniform)
                debug().message(
                    "uniform 'glw_InstanceID' not located with emulated InstanceID enabled"sv);
        }
        apply_instance_id = [&bookkeeping, &program]() {
            auto instanceUniform = make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                uniform_pair{
                    {"glw_InstanceID"sv},
                    SpanOne<const i32>(bookkeeping.instanceId),
                });
            detail::apply_command_modifier(
                *program, bookkeeping, instanceUniform);
        };
    }
    if(m_workarounds.draw.emulated_base_instance && uses_baseinstance)
    {
        if constexpr(compile_info::debug_mode)
        {
            auto loc = detail::get_program_uniform_location(
                *program, program_t::stage_t::Vertex, {"glw_BaseInstance"sv});
            if(loc == invalid_uniform)
                debug().message(
                    "uniform 'glw_BaseInstance' not located with emulated BaseInstance enabled"sv);
        }
        apply_base_instance = [&bookkeeping, &program]() {
            auto baseUniform = make_uniform_list(
                typing::graphics::ShaderStage::Vertex,
                uniform_pair{
                    {"glw_BaseInstance"sv},
                    SpanOne<const i32>(bookkeeping.baseInstance),
                });
            detail::apply_command_modifier(*program, bookkeeping, baseUniform);
        };
    }
    if(m_workarounds.draw.emulated_vertex_offset && uses_vertex_offset)
    {
        apply_vertex_offset = [&vao](u32 offset) {
            for(auto const& attrib : vao->m_attributes)
            {
                cmd::bind_buffer(
                    gl::group::buffer_target_arb::array_buffer,
                    vao->m_buffers.at(attrib.buffer.id).lock()->m_handle);
                detail::vertex_setup_attribute(
                    attrib, offset * attrib.value.stride);
            }
        };
    }

    const bool indirect_supported //= false;
        = m_api_type == api_type_t::core
              ? (uses_baseinstance ? m_api_version >= 0x420
                                   : m_api_version >= 0x400)
              : m_api_version >= 0x310;
    const bool has_uniform_element_type
        = call.indexed && stl_types::all_of(data, [&data](auto const& d) {
              return d.elements.type == data.at(0).elements.type;
          });
    const bool multi_indirect_supported //= false;
        = m_api_type == api_type_t::core ? m_api_version >= 0x430 : false;
    const bool legacy_draw_only
        = m_api_type == api_type_t::es && m_api_version == 0x200;

    if constexpr(compile_info::debug_mode)
    {
        if(multi_indirect_supported && call.indexed
           && !has_uniform_element_type)
            debug().message(
                "varying element types prevents use of MultiDrawIndirect"sv);
    }

    if constexpr(compile_info::debug_mode)
    {
        auto log = detail::program_log(program->m_handle);
        if(auto error = detail::evaluate_draw_state(m_limits, command);
           error.has_value())
            return std::make_tuple(*error, string());
    }

    if(multi_indirect_supported && (!call.indexed || has_uniform_element_type))
    {
        detail::multi_indirect_draw(call, data, *m_indirect_buffer);
    } else if(indirect_supported)
    {
        for(auto const& d : data)
        {
            bookkeeping.baseInstance = d.instances.offset;
            apply_base_instance();
            detail::indirect_draw(call, d, *m_indirect_buffer);
        }
    } else if(!legacy_draw_only)
    {
        for(auto const& d : data)
        {
            bookkeeping.baseInstance = d.instances.offset;
            apply_base_instance();
            apply_vertex_offset(call.indexed ? d.elements.vertex_offset : 0);
            detail::direct_draw(call, d, bookkeeping, m_workarounds);
        }
    } else
    {
        using stl_types::range;
        for(auto const& d : data)
        {
            [[maybe_unused]] auto _
                = debug().scope("legacy_draw::base_instance");
            auto       num_instances    = std::max(d.instances.count, 1u);
            auto const base_sampler_idx = bookkeeping.sampler_idx;
            apply_base_instance();
            if(call.indexed && d.elements.vertex_offset > 0)
                for(auto const& attrib : vao->m_attributes)
                {
                    cmd::bind_buffer(
                        group::buffer_target_arb::array_buffer,
                        vao->m_buffers.at(attrib.buffer.id).lock()->m_handle);
                    detail::vertex_setup_attribute(
                        attrib, d.elements.vertex_offset * attrib.value.stride);
                }
            for([[maybe_unused]] auto i : range<u32>(num_instances))
            {
                [[maybe_unused]] auto _
                    = debug().scope("legacy_draw::instance");
                (detail::apply_command_modifier_per_call(
                     *program,
                     bookkeeping,
                     modifiers,
                     bookkeeping.baseInstance,
                     d.instances.offset + bookkeeping.instanceId),
                 ...);
                apply_instance_id();
                detail::legacy_draw(call, d);
                /* We reset the sampler_idx between instances, to not run out
                 * when we exceed 16+ */
                bookkeeping.sampler_idx = base_sampler_idx;
                bookkeeping.instanceId++;
            }
            bookkeeping.baseInstance++;
        }
    }

    if(!m_features.vertex.vertex_arrays)
        for(auto const& attrib : vao->m_attributes)
            cmd::disable_vertex_attrib_array(attrib.index);

    if(element_buf)
        cmd::bind_buffer(group::buffer_target_arb::element_array_buffer, 0);

    if(m_features.vertex.vertex_arrays)
        cmd::bind_vertex_array(0);

    (detail::undo_command_modifier(*program, bookkeeping, std::move(modifiers)),
     ...);

    return std::nullopt;
}

} // namespace gleam
