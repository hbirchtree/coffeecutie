#include <blam/elegy/decorator.h>

#include <cmath>
#include <cstring>

namespace blam::elegy {

std::vector<decorator_instance_t> decorator_instances(resource_view const& view, decorator_group_t const& group)
{
    std::vector<decorator_instance_t> out;
    auto const* api = view.root_as<render_geometry_api_t>();
    if(!api || group.instance_buffer < 0)
        return out;
    auto const buffers = view.block(api->vertex_buffers);
    if(static_cast<size_t>(group.instance_buffer) >= buffers.size())
        return out;
    auto const* buffer = view.resolve(buffers[static_cast<size_t>(group.instance_buffer)]);
    if(!buffer || buffer->stride.value() != 16)
        return out;
    auto const bytes = view.bytes(buffer->data);
    Vecf3 const minimum = group.bounds_minimum, step = group.position_step;
    size_t const first = static_cast<size_t>(std::max<i32>(group.instance_offset, 0));
    out.reserve(static_cast<size_t>(std::max<i16>(group.placement_count, 0)));
    for(i16 i = 0; i < group.placement_count; i++)
    {
        size_t const at = first + static_cast<size_t>(i) * 16;
        if(at + 16 > bytes.size())
            break;
        u8 const* v = bytes.data() + at;
        auto u16_at     = [&](size_t o) { return static_cast<u16>((v[o] << 8) | v[o + 1]); };
        /* 32-bit words are byte swapped on fetch: component i is byte 3 - i */
        auto snorm      = [&](size_t o) { return std::max(static_cast<i8>(v[o]) / 127.f, -1.f); };
        decorator_instance_t d;
        d.position = minimum + Vecf3(u16_at(0), u16_at(2), u16_at(4)) * step;
        d.type     = v[6];
        d.motion   = v[7] / 255.f;
        d.rotation = Quatf(snorm(8), snorm(11), snorm(10), snorm(9));
        f32 const scale = std::exp2(v[12] / 255.f * 63.75f - 31.75f);
        d.light         = Vecf3(v[15], v[14], v[13]) / 255.f * scale;
        out.push_back(d);
    }
    return out;
}

} // namespace blam::elegy
