#pragma once

#include "blam_antr.h"
#include "blam_base_types.h"
#include "blam_reference.h"

#include <cmath>
#include <cstring>

/* Recorded animations are not keyframes. Sapien samples a unit's control input
 * once per tick and the engine replays it through the same path a player or an
 * actor drives, so playback only reproduces the original because the
 * simulation is deterministic. Dropships and cinematic actors use them.
 *
 * That makes a recording a list of input frames, and playback below hands them
 * over in the same terms a live player produces: a movement axis pair, a view
 * direction and a button mask.
 *
 * Guerilla exposes the 64-byte block and leaves the event stream opaque. The
 * stream layout here was recovered from the retail singleplayer maps: it
 * decodes to the exact byte, and to length_of_animation ticks, for all 349
 * recordings in a10/a30/a50/b30/b40/c10/c20/c40/d20/d40. */

namespace blam::scn::recorded {

/* Each event opens with a byte packing the event id in bits 2-7 and the width
 * of its tick delta in bits 0-1. */
enum class time_delta_t : u8
{
    zero = 0, /* applies on the current tick, no delta stored */
    one  = 1, /* one tick, no delta stored */
    byte = 2, /* delta is the following u8 */
    word = 3, /* delta is the following u16 */
};

enum class event_t : u8
{
    none            = 0,
    end             = 1, /* terminates the stream */
    animation_state = 2, /* u8 */
    aiming_speed    = 3, /* u8 */
    control_flags   = 4, /* u16, see control_flags_t */
    weapon_index    = 5, /* i16 */
    throttle        = 6, /* two f32 */

    /* 7-14 and 15-22: the low three bits name which view vectors the delta
     * applies to (1=facing, 2=aiming, 4=looking). One yaw/pitch pair follows,
     * shared by every selected vector. Retail only ever uses mask 7. */
    vector_delta_i8  = 7,
    vector_delta_i16 = 15,
};

enum control_flags_t : u16
{
    control_crouch            = 0x0001,
    control_jump              = 0x0002,
    control_user1             = 0x0004,
    control_user2             = 0x0008,
    control_light             = 0x0010,
    control_exact_facing      = 0x0020,
    control_action            = 0x0040,
    control_melee             = 0x0080,
    control_look_dont_turn    = 0x0100,
    control_force_alert       = 0x0200,
    control_reload            = 0x0400,
    control_primary_trigger   = 0x0800,
    control_secondary_trigger = 0x1000,
    control_grenade           = 0x2000,
    control_swap_weapon       = 0x4000,
};

enum vector_index : u8
{
    vector_facing  = 0,
    vector_aiming  = 1,
    vector_looking = 2,

    vector_count = 3,
};

constexpr bool is_vector_delta_i8(event_t event)
{
    u8 const id = static_cast<u8>(event);
    return id >= static_cast<u8>(event_t::vector_delta_i8) &&
           id < static_cast<u8>(event_t::vector_delta_i8) + 8;
}

constexpr bool is_vector_delta_i16(event_t event)
{
    u8 const id = static_cast<u8>(event);
    return id >= static_cast<u8>(event_t::vector_delta_i16) &&
           id < static_cast<u8>(event_t::vector_delta_i16) + 8;
}

/* Which view vectors a vector_delta_* event moves, 0 for anything else. */
constexpr u8 vector_mask(event_t event)
{
    if(is_vector_delta_i8(event))
        return static_cast<u8>(event) - static_cast<u8>(event_t::vector_delta_i8);
    if(is_vector_delta_i16(event))
        return static_cast<u8>(event) -
               static_cast<u8>(event_t::vector_delta_i16);
    return 0;
}

/* Bytes of operand following the event header, or nullopt for an id the stream
 * should never contain. */
constexpr optional<u8> operand_size(event_t event)
{
    switch(event)
    {
    case event_t::none:
    case event_t::end:
        return 0;
    case event_t::animation_state:
    case event_t::aiming_speed:
        return 1;
    case event_t::control_flags:
    case event_t::weapon_index:
        return 2;
    case event_t::throttle:
        return 8;
    default:
        break;
    }
    if(is_vector_delta_i8(event))
        return 2;
    if(is_vector_delta_i16(event))
        return 4;
    return std::nullopt;
}

/* Yaw and pitch in 1/2000ths of a turn. Verified against the direction vectors
 * they sit beside: yaw * 2pi/2000 == atan2(facing.y, facing.x) to within one
 * unit of rounding across every retail recording. */
struct control_vector
{
    static constexpr f32 units_per_turn = 2000.f;
    static constexpr f32 to_radians     = 6.2831853f / units_per_turn;

    i16 yaw;
    i16 pitch;

    inline f32 yaw_radians() const
    {
        return yaw * to_radians;
    }

    inline f32 pitch_radians() const
    {
        return pitch * to_radians;
    }

    /* The direction the angles name, matching the vectors the stream header
     * stores alongside them. */
    inline Vecf3 direction() const
    {
        f32 const cp = std::cos(pitch_radians());
        return Vecf3(
            cp * std::cos(yaw_radians()),
            cp * std::sin(yaw_radians()),
            std::sin(pitch_radians()));
    }
};

static_assert(sizeof(control_vector) == 4);

/* Follows the unit control block in the stream; the events move it and the
 * engine rebuilds the direction vectors from it every tick. */
struct controller
{
    std::array<control_vector, vector_count> vectors;
};

static_assert(sizeof(controller) == 12);

/* The unit's control state on the first tick. The serialized form is a prefix
 * that grew over time, so its size comes from unit_control_data_version rather
 * than from sizeof() - see unit_control_size(). */
struct unit_control
{
    u8  animation_state;
    u8  aiming_speed;
    u16 control_flags; /* control_flags_t */
    i16 weapon_index;  /* -1 when the unit holds nothing */
    u8  unknown_1;     /* zero in every version 3 and 4 recording */
    u8  unknown_2;     /* carries animation_state again in versions 3 and 4;
                        * the seven version 0 recordings put something else in
                        * both of these, so their prefix is a guess */

    Vecf2 throttle; /* zero on the first tick of every retail recording */

    Vecf3 facing;
    Vecf3 aiming;
    Vecf3 looking;
};

static_assert(sizeof(unit_control) == 52);

/* Serialized size of the unit control block. Versions 3 and 4 append 6 and 8
 * bytes past the direction vectors: two i16 shared by every recording made in
 * the same scene, then index fields that are -1 throughout retail. Versions 1
 * and 2 appear in no retail map. */
constexpr u32 unit_control_size(u8 unit_control_data_version)
{
    switch(unit_control_data_version)
    {
    case 4:
        return sizeof(unit_control) + 8;
    case 3:
        return sizeof(unit_control) + 6;
    default:
        return sizeof(unit_control);
    }
}

/* Bytes of stream preceding the first event. */
constexpr u32 stream_header_size(u8 unit_control_data_version)
{
    return unit_control_size(unit_control_data_version) + sizeof(controller);
}

struct event_ref
{
    event_t        event;
    u16            delta;   /* ticks passing before the event applies */
    u8             vectors; /* vector_mask() of a vector_delta_* event */
    Span<u8 const> operands;
};

/* Decodes the event at cursor and advances past it. The end marker comes back
 * like any other event, carrying the ticks the recording runs on after the
 * last one that changes anything. nullopt means the stream ran short or named
 * an event it should not contain. */
inline optional<event_ref> next_event(Span<u8 const> stream, u32& cursor)
{
    if(cursor >= stream.size())
        return std::nullopt;

    u8 const header = stream[cursor];
    auto     event  = static_cast<event_t>(header >> 2);

    u16 delta       = 0;
    u32 header_size = 1;
    switch(static_cast<time_delta_t>(header & 0x3))
    {
    case time_delta_t::zero:
        break;
    case time_delta_t::one:
        delta = 1;
        break;
    case time_delta_t::byte:
        if(cursor + 1 >= stream.size())
            return std::nullopt;
        delta       = stream[cursor + 1];
        header_size = 2;
        break;
    case time_delta_t::word:
        if(cursor + 2 >= stream.size())
            return std::nullopt;
        delta = static_cast<u16>(
            stream[cursor + 1] | (stream[cursor + 2] << 8));
        header_size = 3;
        break;
    }

    auto operands = operand_size(event);
    if(!operands.has_value())
        return std::nullopt;
    if(cursor + header_size + operands.value() > stream.size())
        return std::nullopt;

    event_ref out = {
        .event    = event,
        .delta    = delta,
        .vectors  = vector_mask(event),
        .operands = stream.subspan(cursor + header_size, operands.value()),
    };
    cursor += header_size + operands.value();
    return out;
}

/* One tick of input, in the terms the rest of the engine already speaks. */
struct input_state
{
    Vecf2 movement; /* throttle; +x forward, +y right, -1 to 1 */

    std::array<control_vector, vector_count> view;

    u16 buttons; /* control_flags_t */
    u8  animation_state;
    u8  aiming_speed;
    i16 weapon_index; /* -1 when the unit holds nothing */

    inline bool held(control_flags_t flag) const
    {
        return (buttons & flag) != 0;
    }

    inline control_vector const& facing() const
    {
        return view[vector_facing];
    }

    inline control_vector const& aiming() const
    {
        return view[vector_aiming];
    }

    inline control_vector const& looking() const
    {
        return view[vector_looking];
    }
};

/* The header the stream opens with, and the events that follow it. */
struct stream_view
{
    Span<u8 const> data;
    u8             unit_control_version;

    inline unit_control const& control() const
    {
        return *C_RCAST<unit_control const*>(data.data());
    }

    inline controller const& initial_controller() const
    {
        return *C_RCAST<controller const*>(
            data.data() + unit_control_size(unit_control_version));
    }

    inline Span<u8 const> events() const
    {
        return data.subspan(stream_header_size(unit_control_version));
    }

    /* The state the recording starts in, before any event applies. */
    inline input_state initial_state() const
    {
        auto const& c = control();
        return input_state{
            .movement        = c.throttle,
            .view            = initial_controller().vectors,
            .buttons         = c.control_flags,
            .animation_state = c.animation_state,
            .aiming_speed    = c.aiming_speed,
            .weapon_index    = c.weapon_index,
        };
    }
};

/* Replays a stream a tick at a time: state() is the input the unit sees on the
 * current tick, advance() steps to the next. Ticks are 1-based, matching the
 * block's length_of_animation, so the header state is tick 1. */
struct playback
{
    explicit playback(stream_view const& stream) :
        m_events(stream.events()), m_state(stream.initial_state())
    {
        fetch();
        settle();
    }

    inline input_state const& state() const
    {
        return m_state;
    }

    inline u32 tick() const
    {
        return m_tick;
    }

    inline bool finished() const
    {
        if(!m_pending.has_value())
            return true;
        return m_pending.value().event == event_t::end && m_tick >= m_event_tick;
    }

    /* Steps one tick and applies every event that comes due on it. False once
     * the recording has run out, which happens on the tick the end marker
     * names - so tick() lands on length_of_animation. */
    inline bool advance()
    {
        if(finished())
            return false;

        m_tick++;
        settle();
        return true;
    }

private:
    inline void fetch()
    {
        m_pending = next_event(m_events, m_cursor);
        if(m_pending.has_value())
            m_event_tick += m_pending.value().delta;
    }

    /* Events are ordered, so everything due on the current tick applies before
     * the caller sees the state. */
    inline void settle()
    {
        while(m_pending.has_value() &&
              m_pending.value().event != event_t::end && m_event_tick <= m_tick)
        {
            apply(m_pending.value());
            fetch();
        }
    }

    template<typename T>
    static inline T read(Span<u8 const> operands, u32 offset = 0)
    {
        T out{};
        std::memcpy(&out, operands.data() + offset, sizeof(T));
        return from_le(out);
    }

    inline void apply(event_ref const& event)
    {
        switch(event.event)
        {
        case event_t::animation_state:
            m_state.animation_state = event.operands[0];
            return;
        case event_t::aiming_speed:
            m_state.aiming_speed = event.operands[0];
            return;
        case event_t::control_flags:
            m_state.buttons = read<u16>(event.operands);
            return;
        case event_t::weapon_index:
            m_state.weapon_index = read<i16>(event.operands);
            return;
        case event_t::throttle:
            m_state.movement = Vecf2(
                read<f32>(event.operands), read<f32>(event.operands, 4));
            return;
        default:
            break;
        }

        /* A single yaw/pitch delta, added to every view vector the mask names.
         * The vectors are absolute angles, so this integrates. */
        i16 yaw   = 0;
        i16 pitch = 0;
        if(is_vector_delta_i8(event.event))
        {
            yaw   = static_cast<i8>(event.operands[0]);
            pitch = static_cast<i8>(event.operands[1]);
        } else if(is_vector_delta_i16(event.event))
        {
            yaw   = read<i16>(event.operands);
            pitch = read<i16>(event.operands, 2);
        } else
            return;

        for(u8 i = 0; i < vector_count; i++)
            if(event.vectors & (1 << i))
            {
                m_state.view[i].yaw   = static_cast<i16>(m_state.view[i].yaw + yaw);
                m_state.view[i].pitch =
                    static_cast<i16>(m_state.view[i].pitch + pitch);
            }
    }

    Span<u8 const>      m_events;
    input_state         m_state;
    optional<event_ref> m_pending;
    u32                 m_cursor{0};
    u32                 m_tick{1};
    u32                 m_event_tick{1};
};

} // namespace blam::scn::recorded

namespace blam::scn {

/* One element of the scenario's recorded_animations block. */
struct recorded_animation
{
    bl_string name;

    u8 version;            /* event stream codec; 4 in every retail map */
    i8 raw_animation_data; /* zero in every retail recording, so the shape of
                            * a stream that sets it is unknown */
    u8 unit_control_data_version;
    u8 padding_1;

    i16 length_of_animation; /* ticks; one more than the stream's summed
                              * deltas, the first tick being the state the
                              * stream header opens with */
    u16 padding_2;
    u32 padding_3;

    antr::data_ref_t event_stream;

    inline result<recorded::stream_view, error_msg> stream(
        map_ptr const& magic) const
    {
        using namespace std::string_view_literals;

        auto data = event_stream.data(magic);
        if(data.has_error())
            return data.error();
        if(data.value().size() <
           recorded::stream_header_size(unit_control_data_version))
            return "truncated recorded animation stream"sv;
        return recorded::stream_view{
            .data                 = data.value(),
            .unit_control_version = unit_control_data_version,
        };
    }
};

static_assert(sizeof(recorded_animation) == 64);

} // namespace blam::scn
