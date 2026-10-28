#include <blam/volta/blam_antr_sample.h>

#include <glm/gtc/quaternion.hpp>

#include <cstring>

namespace blam::antr {

namespace {

/* 48-bit rotation: x, y, z, w as 12-bit ones' complement, high to low */
Quatf quat48(u8 const* at)
{
    u16 words[3];
    std::memcpy(words, at, sizeof(words));
    for(auto& w : words)
        w = from_le(w);
    u64 const bits = u64(words[2]) | (u64(words[1]) << 16) | (u64(words[0]) << 32);
    auto part = [bits](u32 shift) {
        i32 const v = static_cast<i32>((bits >> shift) & 0xfff);
        return static_cast<f32>(v & 0x800 ? v - 4095 : v);
    };
    Quatf const q(part(0), part(36), part(24), part(12));
    f32 const   length = glm::length(q);
    return length > 0.f ? q / length : Quatf(1, 0, 0, 0);
}

Vecf3 vec3_at(u8 const* at)
{
    Vecf3 out;
    std::memcpy(&out, at, sizeof(out));
    return {from_le(out.x), from_le(out.y), from_le(out.z)};
}

f32 f32_at(u8 const* at)
{
    f32 out;
    std::memcpy(&out, at, sizeof(out));
    return from_le(out);
}

Quatf nlerp(Quatf const& a, Quatf b, f32 t)
{
    if(glm::dot(a, b) < 0.f)
        b = -b;
    return glm::normalize(a * (1.f - t) + b * t);
}

/* One channel of a compressed clip: per-node keyframe heads (count:12,
 * first:20), the frame of each keyframe, a default per node, and values */
struct compressed_stream_t
{
    u32 heads, frames, defaults, values, end;
};

/* A node's channel at `frame`: its default before keyframes take over,
 * blended between the keyframes around the frame, held after the last */
template<typename T, typename Decode, typename Mix>
T sample_channel(
    Span<const u8>             block,
    compressed_stream_t const& stream,
    i32                        track,
    T const&                   fallback,
    u32                        frame,
    u32                        size,
    Decode&&                   decode,
    Mix&&                      mix)
{
    auto fits = [&block](size_t at, size_t bytes) { return at + bytes <= block.size(); };
    size_t const head_at = stream.heads + 4u * static_cast<u32>(track);
    if(track < 0 || frame == 0 || !fits(head_at, 4))
        return fallback;
    u32 head;
    std::memcpy(&head, block.data() + head_at, 4);
    head            = from_le(head);
    u32 const count = head & 0xfff, first = head >> 12;
    if(count == 0 || !fits(stream.frames + 2u * (first + count), 0) ||
       !fits(stream.values + size * (first + count), 0))
        return fallback;
    auto key = [&](u32 i) {
        u16 v;
        std::memcpy(&v, block.data() + stream.frames + 2u * (first + i), 2);
        return static_cast<u32>(from_le(v));
    };
    auto value = [&](u32 i) { return decode(block.data() + stream.values + size * (first + i)); };
    if(frame < key(0))
        return mix(fallback, value(0), static_cast<f32>(frame) / static_cast<f32>(key(0)));
    if(frame >= key(count - 1))
        return value(count - 1);
    u32 i = 0;
    while(i + 2 < count && key(i + 1) <= frame)
        i++;
    u32 const a = key(i), b = key(i + 1);
    return mix(value(i), value(i + 1), static_cast<f32>(frame - a) / static_cast<f32>(b - a));
}

} // namespace

bool sample_compressed(
    animation const&    anim,
    Span<const u8>      frame_bytes,
    u32                 frame,
    u32                 node_count,
    std::vector<Quatf>& rot,
    std::vector<Vecf3>& trans,
    std::vector<f32>*   scale)
{
    constexpr u32 header_size = 44;
    i32 const     at          = from_le(anim.compressed_data_offset);
    auto const    offset      = static_cast<size_t>(at);
    if(at < 0 || offset + header_size > frame_bytes.size())
        return false;
    auto const block = frame_bytes.subspan(offset);
    u32        h[11];
    std::memcpy(h, block.data(), sizeof(h));
    for(auto& v : h)
        v = from_le(v);
    auto const size = static_cast<u32>(block.size());
    compressed_stream_t const rotation{header_size, h[0], h[1], h[2], h[3]};
    compressed_stream_t const translation{h[3], h[4], h[5], h[6], h[7]};
    compressed_stream_t const scaling{h[7], h[8], h[9], h[10], size};
    if(rotation.defaults + 6u * node_count > size || translation.defaults + 12u * node_count > size)
        return false;

    rot.assign(node_count, Quatf(1, 0, 0, 0));
    trans.assign(node_count, Vecf3(0));
    if(scale)
        scale->assign(node_count, 1.f);
    bool const scaled = scale && scaling.defaults + 4u * node_count <= size;
    i32 rotated = 0, moved = 0, grown = 0;
    for(u32 i = 0; i < node_count; i++)
    {
        rot[i] = sample_channel(
            block,
            rotation,
            anim.has_rotation(i) ? rotated++ : -1,
            quat48(block.data() + rotation.defaults + 6u * i),
            frame,
            6,
            quat48,
            nlerp);
        trans[i] = sample_channel(
            block,
            translation,
            anim.has_translation(i) ? moved++ : -1,
            vec3_at(block.data() + translation.defaults + 12u * i),
            frame,
            12,
            vec3_at,
            [](Vecf3 const& a, Vecf3 const& b, f32 t) { return glm::mix(a, b, t); });
        if(scaled)
            (*scale)[i] = sample_channel(
                block,
                scaling,
                anim.has_scale(i) ? grown++ : -1,
                f32_at(block.data() + scaling.defaults + 4u * i),
                frame,
                4,
                f32_at,
                [](f32 a, f32 b, f32 t) { return a == b ? a : glm::mix(a, b, t); });
    }
    for(auto& q : rot)
        q = glm::conjugate(q);
    return true;
}

bool sample_uncompressed(
    animation const&    anim,
    Span<const u8>      default_bytes,
    Span<const u8>      frame_bytes,
    u32                 frame,
    u32                 node_count,
    std::vector<Quatf>& rot,
    std::vector<Vecf3>& trans,
    std::vector<f32>*   scale)
{
    rot.assign(node_count, Quatf(1, 0, 0, 0));
    trans.assign(node_count, Vecf3(0));
    if(scale)
        scale->assign(node_count, 1.f);

    size_t d = 0;
    size_t f = static_cast<size_t>(frame) * from_le(anim.frame_size);

    auto read_quat = [](Span<const u8> buf, size_t off) {
        return reinterpret_cast<compressed_quat_t const*>(buf.data() + off)->decompress();
    };
    auto read_vec3 = [](Span<const u8> buf, size_t off) {
        return vec3_at(buf.data() + off);
    };

    for(u32 i = 0; i < node_count; i++)
    {
        if(anim.has_rotation(i))
        {
            if(f + 8 <= frame_bytes.size())
                rot[i] = read_quat(frame_bytes, f);
            f += 8;
        } else
        {
            if(d + 8 <= default_bytes.size())
                rot[i] = read_quat(default_bytes, d);
            d += 8;
        }
        if(anim.has_translation(i))
        {
            if(f + 12 <= frame_bytes.size())
                trans[i] = read_vec3(frame_bytes, f);
            f += 12;
        } else
        {
            if(d + 12 <= default_bytes.size())
                trans[i] = read_vec3(default_bytes, d);
            d += 12;
        }
        auto const& bytes = anim.has_scale(i) ? frame_bytes : default_bytes;
        size_t&     at    = anim.has_scale(i) ? f : d;
        if(scale && at + 4 <= bytes.size())
            (*scale)[i] = f32_at(bytes.data() + at);
        at += 4;
    }

    /* antr quats use the same conjugate convention as the mod2 bind quats. */
    for(u32 i = 0; i < node_count; i++)
        rot[i] = glm::conjugate(rot[i]);

    return true;
}

std::pair<Vecf3, f32> root_motion(animation const& clip, Span<const u8> frame_info, i32 frame)
{
    u32 const stride = clip.frame_info_stride();
    if(!stride || (frame + 1) * stride > frame_info.size())
        return {};
    f32 delta[4]{};
    std::memcpy(delta, frame_info.data() + frame * stride, stride);
    for(auto& d : delta)
        d = from_le(d);
    Vecf3 const move{delta[0], delta[1], stride == 16 ? delta[2] : 0.f};
    f32 const   yaw = stride == 16 ? delta[3] : stride == 12 ? delta[2] : 0.f;
    return {move, yaw};
}

} // namespace blam::antr
