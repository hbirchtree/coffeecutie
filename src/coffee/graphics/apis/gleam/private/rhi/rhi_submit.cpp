#include <coffee/graphics/apis/gleam/rhi_submit.h>

namespace gleam::detail {

std::size_t element_size(const draw_command::data_t& data)
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

void create_draw(
    const draw_command::call_spec_t& call,
    const draw_command::data_t&      data,
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

void indirect_draw(
    const draw_command::call_spec_t& call,
    const draw_command::data_t&      data,
    circular_buffer_t&               buffer)
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

void multi_indirect_draw(
    draw_command::call_spec_t const&    call,
    decltype(draw_command::data) const& data,
    circular_buffer_t&                  buffer)
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

std::optional<std::tuple<error, std::string_view>> direct_draw(
    const draw_command::call_spec_t& call,
    const draw_command::data_t&      data,
    shader_bookkeeping_t&            bookkeeping,
    const workarounds&               workarounds)
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

std::optional<std::tuple<error, std::string_view>> legacy_draw(
    const draw_command::call_spec_t& call, const draw_command::data_t& data)
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

void compute_ubo_instance(
    buffer_list*                                 buffers,
    std::pair<libc_types::u32, libc_types::u32>& span,
    shader_bookkeeping_t&                        bookkeeping,
    libc_types::u32                              ubo_alignment)
{
    /* What we're trying to do here is to adjust the baseInstance such that we
     * comply with the silly UBO alignment rules.
     * If we're passing in f.ex. a 64-byte chunk to the shader, and we want to
     * start drawing at the 3rd element, the offset would be 192 bytes, which
     * would not work.
     * If we adjust the baseInstance to say "start drawing at the 3rd instance"
     * this is not so much of a problem.
     *
     * First we need to find the smallest stride and find out how often we need
     * to move the baseInstance based on that.
     * If we say the UBO alignment is 256 bytes, then:
     * For a 16-byte structure, we only move the baseInstance every 16 instances
     * For a 64-byte structure we only move it every 4 instances and so on
     */
    u32 smallest_stride{ubo_alignment};
    for(auto const& buffer : *buffers)
    {
        if(buffer.stride == 0)
            continue;
        smallest_stride
            = std::min<u32>(static_cast<u32>(buffer.stride), smallest_stride);
    }

    u32 skip = ubo_alignment / smallest_stride;

    if(skip == 1)
    {
        bookkeeping.baseInstance = 0;
        return;
    }

    /* Now look at the span that was planned and compute base instance */
    u32 num_skips     = span.first / skip;
    u32 base_instance = span.first - num_skips * skip;

    /* Set the end of the buffer range */
    u32 end_instance = span.first + span.second;
    u32 pad = skip - (end_instance - (end_instance / skip) * skip);
    u32 num_blocks = (end_instance + pad) / skip - num_skips;

    bookkeeping.baseInstance = static_cast<i32>(base_instance);
    span.first               = num_skips * skip;
    span.second              = num_blocks * skip;
}

std::optional<error> evaluate_draw_state(
    const api_limits& limits, const draw_command& command)
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

} // namespace gleam::detail
