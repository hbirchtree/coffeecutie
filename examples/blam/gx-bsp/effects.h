#pragma once

/* Effects as sprites: effe bursts of part particles, and mgs2 light volumes
 * (plasma bolts' glow). Decoded while the map is mapped into what drawing
 * them needs, as the map is let go after loading; their sprite sheets are
 * uploaded with the other textures. */

#include <blam/common/endian.h>
#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_particles.h>
#include <blam/volta/blam_stl.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>
#include <vector>

#include "sprites.h"

namespace gx_effects {

using blam::from_le;
using gx_sprites::blend_t;
using gx_sprites::frame_t;
using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u32;

inline glm::vec2 le(typing::vector_types::Vecf2 const& v)
{
    return {from_le(v.x), from_le(v.y)};
}

/* Tag colors are a, r, g, b */
inline glm::vec4 rgba(typing::vector_types::Vecf4 const& argb)
{
    return {from_le(argb.y), from_le(argb.z), from_le(argb.w), from_le(argb.x)};
}

/* A run of frames in library_t::frames */
struct frames_t
{
    u32 first{0}, count{0};
};

struct volume_t
{
    frames_t  frames;
    i32       count{1};
    f32       offset{0.f}, offset_exponent{1.f}, length{0.f};
    f32       radius[2]{}, radius_exponent{1.f};
    glm::vec4 tint[2]{};
    f32       tint_exponent{1.f};
};

struct particle_t
{
    frames_t  frames;
    u32       initial{0}, looping{0}; // frames
    glm::vec2 lifespan{1.f};
    f32       fade_in{0.f}, fade_out{0.f};
    glm::vec2 frame_rate{0.f}; // per second
    blend_t   blend{blend_t::alpha};
    f32       gravity{0.f}, friction{0.f};
};

/* An effe event's burst of one part */
struct burst_t
{
    u32       particle{0};
    glm::vec2 count{0.f}, velocity{0.f}, radius{0.f}, spread{0.f}, spin{0.f};
    f32       cone{0.f};
    glm::vec4 tint[2]{};
    glm::vec3 offset{0.f};
    glm::vec2 direction{0.f}; // yaw, pitch off the effect's forward
    bool      random_angle{false};
};

struct event_t
{
    glm::vec2            delay{0.f};
    std::vector<burst_t> bursts;
};

struct effect_t
{
    std::vector<event_t> events;
};

inline blend_t blend_of(blam::scn::framebuffer_blending f)
{
    using fb = blam::scn::framebuffer_blending;
    switch(f)
    {
    case fb::add:
    case fb::subtract:
    case fb::component_max:
        return blend_t::add;
    case fb::multiply:
    case fb::double_multiply:
        return blend_t::multiply;
    default:
        return blend_t::alpha;
    }
}

/* What the map's effects need, decoded once */
class library_t
{
  public:
    std::vector<frame_t> frames;
    // Per frame until uploaded; none for the glow a sprite without a bitmap has
    std::vector<blam::bitm::image_t const*> images;
    std::vector<volume_t>                   volumes;
    std::vector<particle_t>                 particles;
    std::vector<effect_t>                   effects;

    template<typename V, typename Ref>
    std::optional<u32> effect(blam::map_container<V> const& c, Ref const& ref)
    {
        auto const* fx = data_of<blam::scn::effect>(c, ref);
        if(!fx)
            return std::nullopt;
        if(auto it = m_effects.find(ref.tag_id.value()); it != m_effects.end())
            return it->second;
        effect_t out;
        auto     events = fx->events.data(c.magic);
        if(events.has_error())
            return std::nullopt;
        for(auto const& ev : events.value())
        {
            event_t e{.delay = le(ev.delay_bounds)};
            auto    bursts = ev.particles.data(c.magic);
            if(bursts.has_error())
                continue;
            for(auto const& b : bursts.value())
            {
                auto const part = particle(c, b.particle_type);
                if(!part)
                    continue;
                using flags = blam::scn::effect::particle_t::flags_t;
                e.bursts.push_back({
                    .particle = *part,
                    .count =
                        {static_cast<f32>(from_le(b.count.x)),
                         static_cast<f32>(from_le(b.count.y))},
                    .velocity = le(b.velocity),
                    .radius   = le(b.radius),
                    .spread   = le(b.distribution_radius),
                    .spin     = le(b.angular_velocity),
                    .cone     = from_le(b.velocity_cone_angle),
                    .tint =
                        {rgba(b.tint_lower_bound), rgba(b.tint_upper_bound)},
                    .offset =
                        {from_le(b.relative_offset.x),
                         from_le(b.relative_offset.y),
                         from_le(b.relative_offset.z)},
                    .direction = le(b.relative_direction),
                    .random_angle =
                        (from_le(b.flags) & flags::random_initial_angle) !=
                        flags::none,
                });
            }
            if(!e.bursts.empty())
                out.events.push_back(std::move(e));
        }
        if(out.events.empty())
            return std::nullopt;
        effects.push_back(std::move(out));
        return m_effects[ref.tag_id.value()] =
                   static_cast<u32>(effects.size() - 1);
    }

    template<typename V, typename Ref>
    std::optional<u32> volume(blam::map_container<V> const& c, Ref const& ref)
    {
        auto const* lv = data_of<blam::scn::light_volume>(c, ref);
        if(!lv)
            return std::nullopt;
        if(auto it = m_volumes.find(ref.tag_id.value()); it != m_volumes.end())
            return it->second;
        auto fr = lv->frames.data(c.magic);
        if(fr.has_error() || fr.value().empty())
            return std::nullopt;
        auto const& f = fr.value()[0];
        volume_t    out{
               .frames = sheet(c, lv->map, from_le(lv->sequence_index), 1),
               .count  = std::max<i32>(from_le(lv->count), 1),
               .offset = from_le(f.offset_from_marker),
               .offset_exponent = from_le(f.offset_exponent),
               .length          = from_le(f.length),
               .radius = {from_le(f.radius_hither), from_le(f.radius_yon)},
               .radius_exponent = from_le(f.radius_exponent),
               .tint = {rgba(f.tint_color_hither), rgba(f.tint_color_yon)},
               .tint_exponent = from_le(f.tint_color_exponent),
        };
        if(!out.frames.count)
        {
            out.frames = {static_cast<u32>(frames.size()), 1};
            frames.push_back({});
            images.push_back(nullptr);
        }
        volumes.push_back(out);
        return m_volumes[ref.tag_id.value()] =
                   static_cast<u32>(volumes.size() - 1);
    }

  private:
    template<typename T, typename V, typename Ref>
    static T const* data_of(blam::map_container<V> const& c, Ref const& ref)
    {
        auto const* t = blam::tag_at(c, ref.tag_id);
        if(!t || !t->matches(T::tag_class))
            return nullptr;
        auto d = t->template data<T>(c.magic);
        return d.has_error() ? nullptr : d.value();
    }

    template<typename V, typename Ref>
    std::optional<u32> particle(blam::map_container<V> const& c, Ref const& ref)
    {
        auto const* part = data_of<blam::scn::particle>(c, ref);
        if(!part)
            return std::nullopt;
        if(auto it = m_particles.find(ref.tag_id.value());
           it != m_particles.end())
            return it->second;
        using pflags           = blam::scn::particle::flags_t;
        auto const     flags   = from_le(part->flags);
        i16 const      first   = from_le(part->first_sequence_index);
        i16 const      initial = from_le(part->initial_sequence_count);
        i16 const      looping = from_le(part->looping_sequence_count);
        frames_t const head =
            sheet(c, part->bitmap, first, std::max<i16>(initial, 0));
        frames_t const loop =
            sheet(c, part->bitmap, first + initial, std::max<i16>(looping, 0));
        if(!head.count && !loop.count)
            return std::nullopt;
        particle_t out{
            .frames =
                {head.count ? head.first : loop.first, head.count + loop.count},
            .initial  = head.count,
            .looping  = loop.count,
            .lifespan = le(part->lifespan),
            .fade_in  = from_le(part->fade_in_time),
            .fade_out = from_le(part->fade_out_time),
            .frame_rate =
                (flags & pflags::animate_once_per_frame) != pflags::none
                    ? glm::vec2{30.f}
                    : le(part->animation_rate),
            .blend = blend_of(from_le(part->shader.blend_function)),
        };
        if(auto const* phys =
               data_of<blam::scn::point_physics>(c, part->physics))
        {
            using phflags = blam::scn::point_physics::flags_t;
            out.gravity =
                (from_le(phys->flags) & phflags::no_gravity) != phflags::none
                    ? 0.f
                    : from_le(phys->air_gravity_scale);
            out.friction = from_le(phys->air_friction);
        }
        particles.push_back(out);
        return m_particles[ref.tag_id.value()] =
                   static_cast<u32>(particles.size() - 1);
    }

    /* The frames of `count` sequences of a sprite sheet, in order */
    template<typename V, typename Ref>
    frames_t sheet(
        blam::map_container<V> const& c, Ref const& ref, i32 first, i32 count)
    {
        frames_t    out{static_cast<u32>(frames.size()), 0};
        auto const* t = blam::tag_at(c, ref.tag_id);
        if(!t || count <= 0)
            return out;
        auto hdr = t->template data<blam::bitm::header_t>(c.magic);
        if(hdr.has_error())
            return out;
        auto seqs = hdr.value()->sequences.data(c.magic);
        auto imgs = hdr.value()->images.data(c.magic);
        if(imgs.has_error() || imgs.value().empty())
            return out;
        auto const& images = imgs.value();
        auto        add    = [&](u32 image, glm::vec4 uv) {
            if(image >= images.size())
                return;
            frames.push_back({nullptr, uv});
            this->images.push_back(&images[image]);
            out.count++;
        };
        if(seqs.has_error() || seqs.value().empty())
        {
            add(0, {0, 0, 1, 1}); // a plain bitmap
            return out;
        }
        auto const& sequences = seqs.value();
        for(i32 s = std::max(first, 0);
            s < first + count && s < static_cast<i32>(sequences.size());
            s++)
        {
            auto const& seq     = sequences[s];
            auto        sprites = seq.sprites.data(c.magic);
            if(!sprites.has_error() && !sprites.value().empty())
                for(auto const& sp : sprites.value())
                    add(from_le(sp.bitmap_index),
                        {from_le(sp.left),
                         from_le(sp.top),
                         from_le(sp.right),
                         from_le(sp.bottom)});
            else
                for(u32 i = 0; i < from_le(seq.bitmap_count); i++)
                    add(from_le(seq.first_bitmap) + i, {0, 0, 1, 1});
        }
        if(!out.count)
            add(0, {0, 0, 1, 1}); // no such sequence: the bitmap as a whole
        return out;
    }

    std::map<u32, u32> m_effects, m_volumes, m_particles; // by tag id
};

/* Effects in play */
class system_t
{
  public:
    explicit system_t(library_t const& lib)
        : m_lib(lib)
    {
    }

    void spawn(u32 effect, glm::vec3 const& at, glm::vec3 const& forward)
    {
        if(effect >= m_lib.effects.size())
            return;
        auto const&     fx  = m_lib.effects[effect];
        glm::vec3 const dir = glm::length(forward) > 1e-4f
                                  ? glm::normalize(forward)
                                  : glm::vec3{0, 0, 1};
        for(u32 e = 0; e < fx.events.size(); e++)
            m_pending.push_back(
                {effect, e, uniform(fx.events[e].delay), at, dir});
    }

    void step(f32 dt)
    {
        for(auto& p : m_pending)
            if((p.delay -= dt) <= 0.f)
                emit(p);
        std::erase_if(
            m_pending, [](pending_t const& p) { return p.delay <= 0.f; });
        for(auto& s : m_live)
        {
            auto const& def = m_lib.particles[s.particle];
            s.velocity.z -= gravity * def.gravity * dt;
            s.velocity *= std::max(0.f, 1.f - def.friction * dt);
            s.position += s.velocity * dt;
            s.rotation += s.spin * dt;
            s.frame += s.frame_rate * dt;
            s.age += dt;
        }
        std::erase_if(m_live, [](live_t const& s) { return s.age >= s.life; });
    }

    void draw(gx_sprites::batcher_t& out) const
    {
        for(auto const& s : m_live)
        {
            auto const& def = m_lib.particles[s.particle];
            u32         f   = static_cast<u32>(s.frame);
            if(f >= def.initial)
                f = def.looping ? def.initial + (f - def.initial) % def.looping
                                : def.frames.count - 1;
            f32 fade = 1.f;
            if(def.fade_in > 0.f)
                fade = std::min(fade, s.age / def.fade_in);
            if(def.fade_out > 0.f)
                fade = std::min(fade, (s.life - s.age) / def.fade_out);
            out.add(
                m_lib.frames
                    [def.frames.first + std::min(f, def.frames.count - 1)],
                def.blend,
                s.position,
                s.radius,
                s.tint * glm::vec4(1.f, 1.f, 1.f, std::max(fade, 0.f)),
                s.rotation);
        }
    }

    /* A light volume trailing from `at` along `forward` */
    void volume(
        gx_sprites::batcher_t& out,
        u32                    index,
        glm::vec3 const&       at,
        glm::vec3 const&       forward) const
    {
        if(index >= m_lib.volumes.size())
            return;
        auto const& v = m_lib.volumes[index];
        for(i32 i = 0; i < v.count; i++)
        {
            f32 const  t     = v.count > 1 ? f32(i) / f32(v.count - 1) : 0.f;
            auto const curve = [t](f32 e) {
                return std::pow(t, std::max(e, .01f));
            };
            out.add(
                m_lib.frames[v.frames.first],
                blend_t::add,
                at + forward * (v.offset + v.length * curve(v.offset_exponent)),
                glm::mix(v.radius[0], v.radius[1], curve(v.radius_exponent)),
                glm::mix(v.tint[0], v.tint[1], curve(v.tint_exponent)));
        }
    }

    size_t live() const
    {
        return m_live.size();
    }

  private:
    static constexpr f32    gravity       = 3.27f; // wu/s^2
    static constexpr size_t max_particles = 384;

    struct pending_t
    {
        u32       effect, event;
        f32       delay;
        glm::vec3 at, forward;
    };

    struct live_t
    {
        u32       particle;
        glm::vec3 position, velocity;
        glm::vec4 tint;
        f32       radius, rotation, spin;
        f32       age, life, frame, frame_rate;
    };

    f32 uniform(f32 lo, f32 hi)
    {
        m_seed ^= m_seed << 13;
        m_seed ^= m_seed >> 17;
        m_seed ^= m_seed << 5;
        return lo + (hi - lo) * (m_seed & 0xFFFFFF) / f32(0x1000000);
    }

    f32 uniform(glm::vec2 const& range)
    {
        return uniform(range.x, range.y);
    }

    static void basis(glm::vec3 const& dir, glm::vec3& x, glm::vec3& y)
    {
        glm::vec3 const up =
            std::abs(dir.z) < .99f ? glm::vec3{0, 0, 1} : glm::vec3{1, 0, 0};
        x = glm::normalize(glm::cross(up, dir));
        y = glm::cross(dir, x);
    }

    /* A direction within `angle` of dir */
    glm::vec3 cone(glm::vec3 const& dir, f32 angle)
    {
        if(angle <= 0.f)
            return dir;
        f32 const z   = uniform(std::cos(std::min(angle, glm::pi<f32>())), 1.f);
        f32 const phi = uniform(0.f, glm::two_pi<f32>());
        f32 const r   = std::sqrt(std::max(0.f, 1.f - z * z));
        glm::vec3 x, y;
        basis(dir, x, y);
        return x * (r * std::cos(phi)) + y * (r * std::sin(phi)) + dir * z;
    }

    void emit(pending_t const& p)
    {
        glm::vec3 x, y;
        basis(p.forward, x, y);
        for(auto const& b : m_lib.effects[p.effect].events[p.event].bursts)
        {
            auto const&     def   = m_lib.particles[b.particle];
            f32 const       yaw   = b.direction.x;
            f32 const       pitch = b.direction.y;
            glm::vec3 const dir   = glm::normalize(
                p.forward * (std::cos(pitch) * std::cos(yaw)) +
                x * (std::cos(pitch) * std::sin(yaw)) + y * std::sin(pitch));
            glm::vec3 const origin =
                p.at + p.forward * b.offset.x + x * b.offset.y + y * b.offset.z;
            i32 const count = static_cast<i32>(std::round(uniform(b.count)));
            for(i32 i = 0; i < count && m_live.size() < max_particles; i++)
            {
                live_t s{
                    .particle = b.particle,
                    .position =
                        origin +
                        cone(glm::vec3{0, 0, 1}, glm::pi<f32>()) *
                            (uniform(b.spread) * std::cbrt(uniform(0.f, 1.f))),
                    .velocity = cone(dir, b.cone) * uniform(b.velocity),
                    .tint   = glm::mix(b.tint[0], b.tint[1], uniform(0.f, 1.f)),
                    .radius = uniform(b.radius),
                    .rotation =
                        b.random_angle ? uniform(0.f, glm::two_pi<f32>()) : 0.f,
                    .spin = uniform(b.spin) *
                            (uniform(0.f, 1.f) < .5f ? -1.f : 1.f),
                    .age        = 0.f,
                    .life       = std::max(uniform(def.lifespan), .05f),
                    .frame      = 0.f,
                    .frame_rate = uniform(def.frame_rate),
                };
                // Without a loop the animation's end is the particle's
                if(!def.looping && def.frames.count && s.frame_rate > 0.f)
                    s.life = std::min(s.life, def.frames.count / s.frame_rate);
                m_live.push_back(s);
            }
        }
    }

    library_t const&       m_lib;
    std::vector<pending_t> m_pending;
    std::vector<live_t>    m_live;
    u32                    m_seed{0x9E3779B9u};
};

} // namespace gx_effects
