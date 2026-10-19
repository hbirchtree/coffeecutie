#include <blam/elegy/geometry.h>

namespace blam::elegy {

std::vector<Vecf2> lightmap_texcoords(resource_view const& view, i16 vertex_buffer)
{
    std::vector<Vecf2> out;
    auto const* api = view.root_as<render_geometry_api_t>();
    if(!api || vertex_buffer < 0)
        return out;
    auto vbs = view.block(api->vertex_buffers);
    if(static_cast<size_t>(vertex_buffer) >= vbs.size())
        return out;
    auto const* vb = view.resolve(vbs[static_cast<size_t>(vertex_buffer)]);
    if(!vb || vb->format.value() != vertex_format_t::static_per_pixel)
        return out;
    auto bytes = view.bytes(vb->data);
    u32  count = static_cast<u32>(vb->count.value());
    if(u64(count) * 4 > bytes.size())
        return out;
    out.resize(count);
    for(u32 i = 0; i < count; i++)
    {
        std::array<be<u16>, 2> uv;
        std::memcpy(uv.data(), bytes.data() + i * 4, 4);
        out[i] = Vecf2(uv[0].value(), uv[1].value()) / 65535.f;
    }
    return out;
}

std::vector<vertex_light_t> vertex_lights(resource_view const& view, i32 vertex_buffer)
{
    std::vector<vertex_light_t> out;
    auto const* api = view.root_as<render_geometry_api_t>();
    if(!api || vertex_buffer < 0)
        return out;
    auto vbs = view.block(api->vertex_buffers);
    if(static_cast<size_t>(vertex_buffer) >= vbs.size())
        return out;
    auto const* vb = view.resolve(vbs[static_cast<size_t>(vertex_buffer)]);
    if(!vb || vb->format.value() != vertex_format_t::static_per_vertex)
        return out;
    auto bytes = view.bytes(vb->data);
    u32  count = static_cast<u32>(vb->count.value());
    if(u64(count) * 20 > bytes.size())
        return out;
    auto snorm = [](byte_t b) { return std::max(static_cast<i8>(b) / 127.f, -1.f); };
    out.resize(count);
    for(u32 i = 0; i < count; i++)
    {
        byte_t const* v = bytes.data() + i * 20;
        std::array<Vecf3, 5> values;
        for(u32 k = 0; k < 5; k++)
        {
            byte_t const* e = v + k * 4;
            values[k] = Vecf3(snorm(e[1]), snorm(e[2]), snorm(e[3])) *
                        std::exp2(snorm(e[0]) * 31.75f);
        }
        out[i] = {.sh = {values[1], values[2], values[3], values[4]}, .intensity = values[0]};
    }
    return out;
}

bool mesh_buffers::decode(
    compression_t const* compression, std::vector<vertex_t>& out) const
{
    if(!vertices)
        return false;
    u32 count  = vertex_count();
    u32 stride = static_cast<u32>(vertices->stride.value());
    auto format = vertices->format.value();
    if(stride == 0 || u64(count) * stride > vertex_bytes.size())
        return false;
    out.resize(count);
    for(u32 i = 0; i < count; i++)
    {
        auto const* at = vertex_bytes.data() + u64(i) * stride;
        auto&       v  = out[i];
        if(format == vertex_format_t::world && stride >= sizeof(world_vertex_t))
        {
            auto const& w = *reinterpret_cast<world_vertex_t const*>(at);
            v.position    = w.position;
            v.texcoord    = w.texcoord;
            v.normal      = unpack_dhen3n(w.normal);
            v.tangent     = unpack_dhen3n(w.tangent);
            v.binormal    = unpack_dhen3n(w.binormal);
        } else if(
            (format == vertex_format_t::rigid ||
             format == vertex_format_t::skinned) &&
            stride >= sizeof(rigid_vertex_t) && compression)
        {
            auto const& r = *reinterpret_cast<rigid_vertex_t const*>(at);
            for(int c = 0; c < 3; c++)
                v.position[c] = compression_t::expand(
                    unpack_unorm16(r.position[c]), compression->position[c]);
            for(int c = 0; c < 2; c++)
                v.texcoord[c] = compression_t::expand(
                    unpack_unorm16(r.texcoord[c]), compression->texcoord[c]);
            v.normal   = unpack_dhen3n(r.normal);
            v.tangent  = unpack_dhen3n(r.tangent);
            v.binormal = unpack_dhen3n(r.binormal);
            if(format == vertex_format_t::skinned &&
               stride >= sizeof(skinned_vertex_t))
            {
                auto const& s =
                    *reinterpret_cast<skinned_vertex_t const*>(at);
                v.nodes = s.nodes;
                for(int c = 0; c < 4; c++)
                    v.weights[c] = s.weights[c] / 255.f;
            }
        } else if(format == vertex_format_t::decorator && stride >= sizeof(decorator_vertex_t) && compression)
        {
            auto const& d = *reinterpret_cast<decorator_vertex_t const*>(at);
            for(int c = 0; c < 3; c++)
                v.position[c] = compression_t::expand(unpack_unorm16(d.position[c]), compression->position[c]);
            for(int c = 0; c < 2; c++)
                v.texcoord[c] = compression_t::expand(unpack_unorm16(d.texcoord[c]), compression->texcoord[c]);
            v.normal = unpack_dhen3n(d.normal);
        } else
            return false;
    }
    return true;
}

void mesh_buffers::triangles(
    primitive_t                       type,
    u32                               start,
    u32                               count,
    std::vector<std::array<u16, 3>>& out) const
{
    u32 total = index_count();
    if(start > total)
        return;
    count = std::min(count, total - start);
    if(type == primitive_t::triangle_list)
    {
        for(u32 i = 0; i + 2 < count; i += 3)
            out.push_back(
                {index(start + i), index(start + i + 1),
                 index(start + i + 2)});
    } else if(type == primitive_t::triangle_strip)
    {
        /* 0xFFFF restarts the strip, and with it the winding */
        u32 run = 0;
        u16 a = 0, b = 0;
        for(u32 i = 0; i < count; i++)
        {
            u16 c = index(start + i);
            if(c == 0xFFFF)
            {
                run = 0;
                continue;
            }
            if(run >= 2 && a != b && b != c && a != c)
                out.push_back(run & 1 ? std::array<u16, 3>{b, a, c}
                                      : std::array<u16, 3>{a, b, c});
            a = b;
            b = c;
            run++;
        }
    }
}

} // namespace blam::elegy
