#include "ui/hud.h"
#include "game/game_mode.h"

#include "loading/blam_files.h"
#include "scripting/cinematic.h"
#include "components.h"
#include "world/devices.h"
#include "selected_version.h"
#include "ui/ui.h"
#include "ui/ui_data.h"
#include "combat/weapons.h"

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_strings.h>
#include <blam/volta/blam_ui.h>
#include <blam/volta/blam_unit.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <array>
#include <span>
#include <string>
#include "audio/sounds.h"

using HudManifest = compo::SubsystemManifest<
    type_list_t<
        const PlayerInfo,
        const PlayerCamera,
        const NetworkInfo,
        const BipedEquipment,
        const BipedStatus,
        const PlayerInput,
        const Actor,
        const DeviceState,
        const Model>,
    type_list_t<
        HudScene,
        CinematicState,
        ScreenText,
        const GameState,
        const BlamFiles<halo_version>,
        const LoadingStatus>,
    type_list_t<comp_app::GraphicsFramebuffer, comp_app::EventBus<SoundEvent>>>;

namespace {

using blam::grenade_hud_interface;
using blam::unit_hud_interface;
using blam::weapon_hud_interface;

using anchor_t   = unit_hud_interface::anchor_t;
using sprite_t   = HudScene::sprite_t;
using attached_t = weapon_hud_interface::attached_to_t;

/* The values HUD elements are attached to */
struct state_t
{
    f32 total{0.f};  /*!< rounds carried, past the magazine */
    f32 loaded{0.f}; /*!< rounds in the magazine */
    f32 total_max{1.f};
    f32 loaded_max{1.f};
    f32 secondary_total{0.f};
    f32 secondary_loaded{0.f};
    f32 heat{0.f};
    f32 battery{1.f}; /*!< What is left of it */
    u8  zoom{0};      /*!< Zoom level, 0 = none */
};

/* Drawn premultiplied: alpha is how much of what is behind an element it
 * hides, so at zero it only adds on */
Vecf4 color_of(blam::argb8_t c)
{
    return Vecf4(c.r, c.g, c.b, c.a) / 255.f;
}

bool blank(blam::argb8_t c)
{
    return c.r == 0 && c.g == 0 && c.b == 0 && c.a == 0;
}

/* MCC lays its HUD out on a 1280x960 canvas (tag offsets twice ours),
 * every bitmap at twice that again, unflagged */
template<typename V>
constexpr libc_types::f32 hud_canvas_scale(V)
{
    return 1.f;
}
constexpr libc_types::f32 hud_canvas_scale(blam::mcc_version_t)
{
    return .5f;
}
template<typename V>
constexpr libc_types::f32 hud_bitmap_scale(V)
{
    return 1.f;
}
constexpr libc_types::f32 hud_bitmap_scale(blam::mcc_version_t)
{
    return .25f;
}

} // namespace

struct HudSystem : compo::RestrictedSubsystem<HudSystem, HudManifest>
{
    using type  = HudSystem;
    using Proxy = compo::proxy_of<HudManifest>;

    HudSystem()
    {
        /* After weapons and equipment have settled for the frame */
        this->priority = 910;
    }

    /* A sequence's frame: its sprite, or a whole image, and its size in
     * texels */
    struct frame_t
    {
        sprite_t sprite;
        Vecf2    size;
    };

    std::optional<frame_t> frame_of(blam::tagref_t const& bitmap, i16 sequence, i16 frame)
    {
        if(!bitmap.valid())
            return std::nullopt;
        /* Custom Edition keeps most HUD bitmaps in bitmaps.map */
        auto found = m_index.template resource<blam::bitm::header_t>(bitmap.tag_id);
        if(!found)
            return std::nullopt;
        auto const& [tag, header_ptr, magic] = *found;
        auto images    = header_ptr->images.data(magic);
        auto sequences = header_ptr->sequences.data(magic);
        if(!images.has_value() || images.value().empty())
            return std::nullopt;
        frame_t out{.sprite = {.bitmap = &bitmap}};
        u16 image = 0;
        if(sequences.has_value() && sequence >= 0 &&
           static_cast<size_t>(sequence) < sequences.value().size())
        {
            auto const& seq     = sequences.value()[sequence];
            auto        sprites = seq.sprites.data(magic);
            if(sprites.has_value() && !sprites.value().empty())
            {
                auto const& sprite =
                    sprites.value()[static_cast<size_t>(frame) % sprites.value().size()];
                image          = sprite.bitmap_index;
                out.sprite.uv  = Vecf4(sprite.left, sprite.top, sprite.right, sprite.bottom);
            } else if(seq.bitmap_count > 0)
                image = static_cast<u16>(
                    seq.first_bitmap + static_cast<u32>(frame) % seq.bitmap_count);
        }
        if(image >= images.value().size())
            return std::nullopt;
        auto const& size = images.value()[image].isize;
        out.sprite.image = image;
        out.size         = Vecf2(out.sprite.uv.z - out.sprite.uv.x, out.sprite.uv.w - out.sprite.uv.y) *
                   Vecf2(size.x, size.y);
        return out;
    }

    /* Where an element of `size` goes for its anchor and offset; corners
     * keep to the title-safe frame */
    Vecf2 place(anchor_t anchor, blam::vec2i16 offset, Vecf2 size) const
    {
        Vecf2 const o = Vecf2(offset.x, offset.y) * offset_scale + (anchor == anchor_t::center ? Vecf2() : safe_inset);
        switch(anchor)
        {
        case anchor_t::top_right:
            return {m_width - o.x - size.x, o.y};
        case anchor_t::bottom_left:
            return {o.x, HudScene::height - o.y - size.y};
        case anchor_t::bottom_right:
            return {m_width - o.x - size.x, HudScene::height - o.y - size.y};
        case anchor_t::center:
            return Vecf2(m_width, HudScene::height) * .5f + o - size * .5f;
        default:
            return o;
        }
    }

    static constexpr f32 offset_scale = hud_canvas_scale(halo_version_v);
    static constexpr f32 sprite_scale = hud_bitmap_scale(halo_version_v);

    /* Bitmaps drawn for high resolutions are twice the size they show at */
    static Vecf2 scale_of(unit_hud_interface::background_base_t const& base)
    {
        using flags_t = unit_hud_interface::background_base_t::scaling_flags_t;
        bool const high_res =
            (base.scaling_flags & flags_t::use_high_res_scale) != flags_t::none &&
            (base.scaling_flags & flags_t::dont_scale_size) == flags_t::none;
        return Vecf2(
                   base.width_scale > 0.f ? base.width_scale : 1.f,
                   base.height_scale > 0.f ? base.height_scale : 1.f) *
               (high_res ? .5f : 1.f) * sprite_scale;
    }

    void push(
        anchor_t                                     anchor,
        unit_hud_interface::background_base_t const& base,
        blam::tagref_t const&                        bitmap,
        i16                                          sequence,
        Vecf4 const&                                 color,
        f32                                          fill  = -1.f,
        Vecf4 const&                                 empty = {},
        i16                                          sprite = 0)
    {
        auto frame = frame_of(bitmap, sequence, sprite);
        if(!frame)
            return;
        Vecf2 const size = frame->size * scale_of(base);
        Vecf2 const at   = place(anchor, base.anchor_offset, size);
        frame->sprite.min   = at;
        frame->sprite.max   = at + size;
        frame->sprite.color = color;
        frame->sprite.fill  = fill;
        frame->sprite.empty = empty;
        m_view->sprites.push_back(frame->sprite);
    }

    void background(anchor_t anchor, unit_hud_interface::background_t const& bg, bool blink = false)
    {
        if(blank(bg.colors.default_color))
            return;
        bool const on = lit(bg.colors, blink, m_blink_time);
        push(anchor, bg, bg.interface_bitmap, bg.sequence_index,
             color_of(on ? bg.colors.flashing_color : bg.colors.default_color));
    }

    /* The alpha a texel must stay under to light, as the shader reads it:
     * the value in the meter's units through the tag's multiplier and
     * bias, never under the tag's minimum. An ammo meter counts rounds; a
     * fraction is scaled by value_scale, or without one to the range the
     * multiplier and bias leave (health: 8 cells of 30 past a bias of 15) */
    static f32 fill_of(unit_hud_interface::meter_base_t const& m, f32 value, bool fraction)
    {
        f32 const mult = std::max<f32>(m.alpha_multiplier, 1.f);
        if(fraction)
            value = std::clamp(value, 0.f, 1.f) *
                    (m.value_scale > 0 ? m.value_scale : (255.f - m.alpha_bias) / mult);
        f32 const alpha = std::max<f32>(m.minimum_meter_value, value * mult + m.alpha_bias);
        return std::min(alpha, 255.f) / 255.f;
    }

    /* Lit up to `value` along the meter bitmap's alpha ramp, in a colour
     * of its own if given */
    void meter(
        anchor_t                                anchor,
        unit_hud_interface::meter_base_t const& m,
        f32                                     value,
        bool                                    fraction = true,
        std::optional<Vecf4>                    color    = std::nullopt)
    {
        f32 const t   = fraction ? std::clamp(value, 0.f, 1.f) : 1.f;
        Vecf4     lit = color ? *color
                              : glm::mix(color_of(m.color_at_minimum), color_of(m.color_at_maximum), t);
        /* The tags leave these colours' alpha at 0: a lit bar is solid */
        if(lit.a <= 0.f)
            lit.a = 1.f;
        push(anchor, m, m.meter_bitmap, m.sequence_index, lit, fill_of(m, value, fraction),
             color_of(m.empty_color));
    }

    /* What a bar has just lost, lit in its flash colour under the bar
     * itself, fading out */
    void damage_flash(
        anchor_t                                anchor,
        unit_hud_interface::meter_base_t const& m,
        f32                                     peak,
        f32                                     strength)
    {
        if(strength <= 0.f)
            return;
        Vecf4 const flash = color_of(m.flash_color);
        push(anchor, m, m.meter_bitmap, m.sequence_index,
             Vecf4(Vecf3(flash) * strength, strength), fill_of(m, peak, true), Vecf4(0.f));
    }

    /* Digits in the hud# font, right-aligned in their field */
    void number(
        anchor_t anchor, grenade_hud_interface::numbers_t const& n, i32 value)
    {
        if(!m_digits || blank(n.colors.default_color))
            return;
        using flags_t = grenade_hud_interface::numbers_t::flags_t;
        std::string text = std::to_string(std::max(value, 0));
        u32 const   width = std::max<u32>(n.maximum_number_digits, 1);
        if(text.size() > width)
            text = std::string(width, '9');
        if((n.flags & flags_t::show_leading_zeros) != flags_t::none)
            text.insert(0, width - text.size(), '0');
        auto glyph = frame_of(m_digits->digits_bitmap, 0, 0);
        if(!glyph)
            return;
        /* Digits sit side by side on the sheet, one sprite or strip cell each */
        Vecf2 const cell(m_digits->bitmap_digit_width, glyph->size.y);
        Vecf2 const scale = scale_of(n);
        f32 const   step  = m_digits->screen_digit_width * scale.x;
        /* The offset places the last digit, the rest run back from it */
        Vecf2 const at = place(anchor, n.anchor_offset, Vecf2(step, cell.y * scale.y)) -
                         Vecf2(step * (width - 1), 0.f) +
                         Vecf2(m_digits->x_offset, m_digits->y_offset) * scale;
        for(size_t i = 0; i < text.size(); i++)
        {
            auto digit = digit_frame(text[i] - '0', *glyph, cell);
            Vecf2 const min = at + Vecf2(step * (width - text.size() + i), 0.f);
            digit.min       = min;
            digit.max       = min + cell * scale;
            digit.color     = color_of(n.colors.default_color);
            m_view->sprites.push_back(digit);
        }
    }

    sprite_t digit_frame(i32 digit, frame_t const& first, Vecf2 cell)
    {
        if(m_digit_sprites)
            if(auto frame = frame_of(m_digits->digits_bitmap, 0, static_cast<i16>(digit)))
                return frame->sprite;
        sprite_t out  = first.sprite;
        f32 const du  = (out.uv.z - out.uv.x) * cell.x / std::max(first.size.x, 1.f);
        out.uv.x     += du * digit;
        out.uv.z      = out.uv.x + du;
        return out;
    }

    /* ---- Unit ---- */

    /* Per seat: what its bars held last frame and what is still lit */
    struct warning_t
    {
        f32 shield{1.f};
        f32 body{1.f};
        f32 shield_peak{1.f}; /*!< What the bar had before the blows */
        f32 body_peak{1.f};
        f32 shield_flash{0.f}; /*!< Seconds the loss stays lit */
        f32 body_flash{0.f};
        u32 latched{0}; /*!< sound_t::latched_to_t bits in effect */
    };

    /* The HUD's warning sounds latch to what the shield and health enter;
     * the shield hit plays for every blow */
    void warning_sounds(
        Proxy&                    p,
        warning_t&                watch,
        unit_hud_interface const& hud,
        BipedStatus const&        status,
        bool                      shield_hit)
    {
        using latch_t = unit_hud_interface::sound_t::latched_to_t;
        auto sounds   = hud.hud_warning_sounds.data(*m_magic);
        if(!sounds.has_value() || sounds.value().empty())
            return;
        u32 now = 0;
        if(status.shield > watch.shield + 1e-4f && watch.shield < 1.f)
            now |= static_cast<u32>(latch_t::shield_recharging);
        if(status.shield <= 0.f)
            now |= static_cast<u32>(latch_t::shield_empty) |
                   static_cast<u32>(latch_t::shield_low);
        if(status.body < hud.min_color_health_fraction_cutoff)
            now |= static_cast<u32>(latch_t::health_low);
        if(status.body <= 0.f)
            now |= static_cast<u32>(latch_t::health_empty);
        u32 entered   = now & ~watch.latched;
        watch.latched = now;
        if(shield_hit)
            entered |= static_cast<u32>(latch_t::shield_damaged);
        if(!entered)
            return;
        auto* bus = p.template service<comp_app::EventBus<SoundEvent>>();
        for(auto const& entry : sounds.value())
        {
            if(!(static_cast<u32>(entry.latched_to) & entered) || !entry.sound.valid())
                continue;
            SoundEvent     ev{.type = SoundEvent::play_sound};
            PlaySoundEvent play{
                .sound    = &static_cast<blam::tagref_t const&>(entry.sound),
                .relative = true,
            };
            bus->inject(ev, &play);
        }
    }

    /* An over shield fills the shield meter again in the overcharge colours,
     * over a full one; the stock HUDs leave those blank, so the meter's
     * full and flash colours stand in */
    void overcharge(unit_hud_interface const& hud, f32 value)
    {
        auto const& m        = hud.shield_panel_meter;
        bool const  authored = !blank(hud.overcharge_minimum_color) ||
                              !blank(hud.overcharge_maximum_color);
        Vecf4 const low  = color_of(authored ? hud.overcharge_minimum_color : m.color_at_maximum);
        Vecf4 const high = color_of(authored ? hud.overcharge_maximum_color : m.flash_color);
        Vecf4 const below = color_of(
            blank(hud.overcharge_empty_color) ? m.color_at_maximum : hud.overcharge_empty_color);
        value = std::clamp(value, 0.f, 1.f);
        push(hud.anchor, m, m.meter_bitmap, m.sequence_index, glm::mix(low, high, value),
             fill_of(m, value, true), below);
    }

    void unit_hud(
        Proxy& p, u64 id, unit_hud_interface const& hud, BipedStatus const& status)
    {
        f32 const shield = status.shield;
        f32 const health = status.body;
        auto&     watch  = m_warnings[id];
        /* A blow holds what it took lit for a moment; an over shield
         * running down is not one */
        bool const shield_hit = shield < watch.shield - 1e-4f && status.recent_shield > 0.f;
        bool const body_hit   = health < watch.body - 1e-4f;
        if(shield_hit)
        {
            watch.shield_peak  = std::max(watch.shield_peak, std::min(watch.shield, 1.f));
            watch.shield_flash = damage_flash_seconds;
        }
        if(body_hit)
        {
            watch.body_peak  = std::max(watch.body_peak, watch.body);
            watch.body_flash = damage_flash_seconds;
        }
        watch.shield_flash = std::max(watch.shield_flash - m_dt, 0.f);
        watch.body_flash   = std::max(watch.body_flash - m_dt, 0.f);
        if(watch.shield_flash <= 0.f)
            watch.shield_peak = shield;
        if(watch.body_flash <= 0.f)
            watch.body_peak = health;
        warning_sounds(p, watch, hud, status, shield_hit);
        watch.shield = shield;
        watch.body   = health;

        auto const& shown = p.template subsystem<CinematicState>().hud_parts;
        auto const& blink = p.template subsystem<CinematicState>().hud_blink;
        if(shown.health || shown.shield)
            background(hud.anchor, hud.unit_hud_background);
        if(shown.shield)
        {
            background(hud.anchor, hud.shield_panel_background, blink.shield);
            if(shield > 1.f)
                overcharge(hud, (shield - 1.f) / (BipedStatus::over_shield - 1.f));
            else
            {
                damage_flash(hud.anchor, hud.shield_panel_meter, watch.shield_peak,
                             watch.shield_flash / damage_flash_seconds);
                meter(hud.anchor, hud.shield_panel_meter, shield);
            }
        }
        if(shown.health)
            health_panel(hud, watch, health, blink.health);
        if(!shown.motion_sensor)
            return;
        /* The motion sensor keeps to the bottom left */
        background(anchor_t::bottom_left, hud.motion_sensor_background, blink.motion_sensor);
        background(anchor_t::bottom_left, hud.motion_sensor_foreground, blink.motion_sensor);
        motion_sensor(p, id, hud);
    }

    void health_panel(unit_hud_interface const& hud, warning_t const& watch, f32 health, bool blink = false)
    {
        background(hud.anchor, hud.health_panel_background, blink);
        damage_flash(hud.anchor, hud.health_panel_meter, watch.body_peak,
                     watch.body_flash / damage_flash_seconds);
        /* Health shows in three bands, not a blend: the max colour, the
         * medium one, then the min colour */
        using mflags_t = unit_hud_interface::meter_base_t::meter_flags_t;
        auto const& hm = hud.health_panel_meter;
        std::optional<Vecf4> band;
        if((hm.flags & mflags_t::use_min_max_for_state_changes) != mflags_t::none)
            band = health > hud.max_color_health_fraction_cutoff ? color_of(hm.color_at_maximum)
                   : health > hud.min_color_health_fraction_cutoff
                       ? color_of(hud.medium_health_left_color)
                       : color_of(hm.color_at_minimum);
        meter(hud.anchor, hm, health, true, band);
    }

    /* Blips for what moves within the sensor's range, the view's forward
     * up: foes red, friends yellow, a vehicle's larger */
    void motion_sensor(Proxy& p, u64 self, unit_hud_interface const& hud)
    {
        auto const* info = p.template get<PlayerInfo>(self);
        auto const* cam  = p.template get<PlayerCamera>(self);
        if(!info || !cam || !m_hudg)
            return;
        auto const& fg    = hud.motion_sensor_foreground;
        auto        frame = frame_of(fg.interface_bitmap, fg.sequence_index, 0);
        if(!frame)
            return;
        auto blip = frame_of(m_blip, 0, 0);
        if(!blip)
            return;
        Vecf2 const size   = frame->size * scale_of(fg);
        Vecf2 const centre = place(anchor_t::bottom_left, fg.anchor_offset, size) + size * .5f;
        f32 const   radius = std::min(size.x, size.y) * .5f * sensor_ring;
        f32 const   range  = m_hudg->motion_sensor_range > 0.f ? m_hudg->motion_sensor_range : 20.f;

        Vecf3 const view    = glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f};
        Vecf2 const forward = glm::dot(Vecf2(view), Vecf2(view)) > 1e-6f ? glm::normalize(Vecf2(view))
                                                                           : Vecf2{1.f, 0.f};
        Vecf2 const right{forward.y, -forward.x};
        auto const& game   = p.template subsystem<GameState>();
        auto const  my_team = [&]() -> i16 {
            if(game.kind != GameState::kind_t::multiplayer || !game.teams)
                return -1;
            for(auto const& pl : game.players)
                if(pl.player_idx == info->player_idx)
                    return pl.team;
            return -1;
        }();
        for(auto ent : p.template select<Model>())
        {
            if(ent.id() == self || ent.id() == info->riding.vehicle)
                continue;
            auto const& model = ent.template get<Model>();
            auto const* tag   = model.origin_object;
            if(!tag || !model.tick_posed)
                continue;
            bool const vehicle = tag->matches(blam::tag_class_t::vehi);
            auto const* status = p.template get<BipedStatus>(ent.id());
            auto const* other  = p.template get<PlayerInfo>(ent.id());
            if(!vehicle && (!status || status->dead))
                continue;
            if(other && other->riding.vehicle)
                continue;
            /* Standing still, or crouched along, it is not seen */
            f32 const speed = glm::distance(model.position, model.tick_position) * tick_rate;
            if(speed < sensor_speed)
                continue;
            Vecf2 const rel = Vecf2(model.position - cam->camera.position);
            f32 const   d   = glm::length(rel);
            if(d > range)
                continue;
            bool foe = true;
            if(other)
            {
                if(my_team >= 0)
                    for(auto const& pl : game.players)
                        if(pl.player_idx == other->player_idx)
                            foe = pl.team != my_team;
                if(game.kind != GameState::kind_t::multiplayer)
                    foe = false;
            } else if(auto const* actor = p.template get<Actor>(ent.id()))
                foe = hostile(actor->team);
            else if(!vehicle)
                continue;
            Vecf2 const at = centre + Vecf2(glm::dot(rel, right), -glm::dot(rel, forward)) * (radius / range);
            f32 const   half = (vehicle ? blip_vehicle : blip_unit) * .5f;
            auto        dot  = *blip;
            dot.sprite.min   = at - Vecf2(half);
            dot.sprite.max   = at + Vecf2(half);
            dot.sprite.color = foe ? blip_foe : blip_friend;
            m_view->sprites.push_back(dot.sprite);
        }
    }

    /* The scripts' nav points this player is shown: the waypoint arrow over
     * the spot when it is in view, else at the screen's edge turned toward
     * it */
    void nav_points(Proxy& p, PlayerInfo const& info, PlayerCamera const& cam)
    {
        using team_t  = blam::scn::unit::team_t;
        using flags_t = blam::hud_globals::waypoint_arrow_t::flags_t;
        auto const& game = p.template subsystem<GameState>();
        auto const& navs = game.nav_points;
        if(navs.empty() || !m_hudg)
            return;
        auto arrows = m_hudg->waypoint_arrows.data(*m_magic);
        if(!arrows.has_value())
            return;
        Vecf4 const inset = m_hudg->waypoint_offsets; /* top, bottom, left, right */
        Vecf2 const lo{inset.z, inset.x};
        Vecf2 const hi{m_width - inset.w, HudScene::height - inset.y};
        for(auto const& nav : navs)
        {
            /* Players are all of the player team to the scripts */
            if(nav.player >= 0 ? nav.player != static_cast<i32>(info.player_idx)
                               : nav.team != static_cast<i16>(team_t::player))
                continue;
            if(nav.mp_team >= 0)
                if(auto const* mine = game.player(info.player_idx); !mine || mine->team != nav.mp_team)
                    continue;
            auto const* arrow = [&]() -> blam::hud_globals::waypoint_arrow_t const* {
                for(auto const& a : arrows.value())
                    if(a.name.str() == nav.arrow)
                        return &a;
                return arrows.value().empty() ? nullptr : &arrows.value()[0];
            }();
            if(!arrow)
                continue;
            Vecf3 at = nav.position;
            if(nav.object || nav.unit >= 0 || nav.net)
            {
                auto const* model = p.template get<Model>(marked(p, nav));
                if(!model)
                    continue;
                at = model->position;
            }
            at.z += nav.offset;
            Vecf4 const clip   = cam.matrix * Vecf4(at, 1.f);
            bool const  behind = clip.w <= 1e-4f;
            Vecf2 const ndc    = Vecf2(clip) / std::max(std::abs(clip.w), 1e-4f);
            Vecf2       spot{(ndc.x * .5f + .5f) * m_width, (.5f - ndc.y * .5f) * HudScene::height};
            bool const  inside = !behind && glm::all(glm::greaterThanEqual(spot, lo)) &&
                                glm::all(glm::lessThanEqual(spot, hi));
            auto frame = frame_of(
                m_hudg->waypoint_arrow_bitmap, inside ? arrow->on_screen_sequence : arrow->off_screen_sequence, 0);
            if(!frame)
                continue;
            f32 turn = 0.f;
            if(!inside)
            {
                /* Out along the way to it, to the inset edge */
                Vecf2 const middle = (lo + hi) * .5f;
                Vecf2       dir    = Vecf2(ndc.x, -ndc.y);
                if(glm::dot(dir, dir) < 1e-8f)
                    dir = {0.f, 1.f};
                Vecf2 const half = (hi - lo) * .5f;
                f32 const   t    = std::min(
                    std::abs(dir.x) > 1e-6f ? half.x / std::abs(dir.x) : 1e9f,
                    std::abs(dir.y) > 1e-6f ? half.y / std::abs(dir.y) : 1e9f);
                spot = middle + dir * t;
                if((arrow->flags & flags_t::dont_rotate_when_pointing_offscreen) == flags_t::none)
                    turn = std::atan2(dir.x, -dir.y);
            }
            auto        sprite = frame->sprite;
            Vecf4 const color  = color_of(arrow->color);
            sprite.min         = spot - frame->size * (offset_scale * .5f);
            sprite.max         = spot + frame->size * (offset_scale * .5f);
            /* Lit in full colour; the opacity only lets the world show through */
            sprite.color       = Vecf4(Vecf3(color), arrow->opacity);
            sprite.rotation    = turn;
            m_view->sprites.push_back(sprite);
        }
    }

    /* What a nav point marks: an object, a player's body, or an object
     * the server named that may only now have arrived */
    u64 marked(Proxy& p, GameState::nav_point_t const& nav)
    {
        if(nav.object)
            return nav.object;
        for(auto ent : p.template select<PlayerInfo>())
            if(nav.unit >= 0 && ent.template get<PlayerInfo>().player_idx == static_cast<u32>(nav.unit))
                return ent.id();
        if(nav.net)
            for(auto ent : p.template select<NetworkInfo>())
                if(ent.template get<NetworkInfo>().instance_id == nav.net)
                    return ent.id();
        return 0;
    }

    /* Players and humans stand together; everyone else is against them */
    static bool hostile(u16 team)
    {
        using team_t = blam::scn::unit::team_t;
        auto const t = static_cast<team_t>(team);
        return t != team_t::player && t != team_t::human;
    }

    static constexpr f32   tick_rate    = 30.f;
    static constexpr f32   sensor_ring  = .9f;  /* of the dial, used for blips */
    static constexpr f32   sensor_speed = .5f;  /* wu/s, slower goes unseen */
    static constexpr f32   blip_unit    = 5.f;  /* HUD px */
    static constexpr f32   blip_vehicle = 8.f;
    static inline Vecf4 const blip_foe{1.f, .15f, .1f, 1.f};
    static inline Vecf4 const blip_friend{1.f, .9f, .2f, 1.f};

    /* ---- Weapon ---- */

    f32 value_of(attached_t to, state_t const& s, bool fraction) const
    {
        switch(to)
        {
        case attached_t::total_ammo:
            return fraction ? s.total / s.total_max : s.total;
        case attached_t::loaded_ammo:
            return fraction ? s.loaded / s.loaded_max : s.loaded;
        case attached_t::secondary_weapon_total_ammo:
            return s.secondary_total;
        case attached_t::secondary_weapon_loaded_ammo:
            return s.secondary_loaded;
        case attached_t::heat:
            return fraction ? s.heat : s.heat * 100.f;
        case attached_t::age:
            return fraction ? s.battery : std::round(s.battery * 100.f);
        default:
            return 0.f;
        }
    }

    /* Rounds are shown as they are, heat and age as a fraction */
    static bool counts(attached_t to)
    {
        return to == attached_t::total_ammo || to == attached_t::loaded_ammo ||
               to == attached_t::secondary_weapon_total_ammo ||
               to == attached_t::secondary_weapon_loaded_ammo;
    }

    bool on_this_map(weapon_hud_interface::use_on_map_type_t type) const
    {
        using map_t = weapon_hud_interface::use_on_map_type_t;
        return type == map_t::any || (type == map_t::multiplayer) == m_multiplayer;
    }

    /* Its elements, then its child's, down the chain */
    void weapon_hud(blam::tag_t const* tag, state_t const& s, u32 depth = 0)
    {
        using crosshair_t = weapon_hud_interface::crosshair_t;
        if(!tag || depth > max_child_depth)
            return;
        auto data = tag->template data<weapon_hud_interface>(*m_magic);
        if(!data.has_value())
            return;
        auto const&    hud    = *data.value();
        anchor_t const anchor = hud.screen_alignment.anchor;
        if(depth == 0)
            scope_mask(hud, s.zoom > 0);

        if(auto statics = hud.screen_alignment.static_elements.data(*m_magic); statics.has_value())
            for(auto const& el : statics.value())
                if(on_this_map(el.can_use_on_map_type))
                    background(anchor, el.background);
        if(auto meters = hud.meter_elements.data(*m_magic); meters.has_value())
            for(auto const& el : meters.value())
                if(on_this_map(el.can_use_on_map_type))
                {
                    bool const fraction = !counts(el.state_attached_to);
                    meter(anchor, el.meter, value_of(el.state_attached_to, s, fraction), fraction);
                }
        if(auto numbers = hud.number_elements.data(*m_magic); numbers.has_value())
            for(auto const& el : numbers.value())
            {
                if(!on_this_map(el.can_use_on_map_type))
                    continue;
                using nflags_t = grenade_hud_interface::numbers_t::flags_t;
                if((el.number.flags & nflags_t::only_show_when_zoomed) != nflags_t::none &&
                   s.zoom == 0)
                    continue;
                using wflags_t = decltype(el.number.weapon_flags);
                f32 value      = value_of(el.state_attached_to, s, false);
                if((el.number.weapon_flags & wflags_t::divide_number_by_clip_size) !=
                   wflags_t::none)
                    value = std::ceil(value / std::max(s.loaded_max, 1.f));
                number(anchor, el.number, static_cast<i32>(value));
            }

        /* Reticles for what the weapon needs */
        bool const empty      = s.loaded <= 0.f;
        bool const none_spare = s.total <= 0.f;
        auto const shown = [&](crosshair_t::crosshair_type_t type) {
            using type_t = crosshair_t::crosshair_type_t;
            switch(type)
            {
            case type_t::aim:
                return true;
            case type_t::zoom:
                return s.zoom > 0;
            case type_t::should_reload:
                return empty && !none_spare;
            case type_t::low_ammo_and_none_left_to_reload:
                return none_spare && s.loaded < s.loaded_max * low_ammo_fraction;
            case type_t::flash_when_firing_and_no_ammo:
                return empty && none_spare;
            default:
                return false;
            }
        };
        if(auto crosshairs = hud.crosshairs.data(*m_magic); crosshairs.has_value())
            for(auto const& ch : crosshairs.value())
            {
                if(!m_crosshair || !on_this_map(ch.can_use_on_map_type) || !shown(ch.crosshair_type))
                    continue;
                auto overlays = ch.crosshair_overlays.data(*m_magic);
                if(!overlays.has_value())
                    continue;
                using flags_t = weapon_hud_interface::crosshair_overlay_t::flags_t;
                auto const hidden = s.zoom > 0 ? flags_t::dont_show_when_zoomed
                                                : flags_t::show_only_when_zoomed;
                for(auto const& ov : overlays.value())
                {
                    if((ov.flags & hidden) != flags_t::none || blank(ov.colors.default_color))
                        continue;
                    /* A zoom readout's sequence has a sprite per zoom level */
                    i16 const level = ch.crosshair_type == crosshair_t::crosshair_type_t::zoom &&
                                              (ov.flags & flags_t::one_zoom_level) == flags_t::none
                                          ? static_cast<i16>(std::max(s.zoom - 1, 0))
                                          : i16(0);
                    push(anchor_t::center, ov, ch.crosshair_bitmap, ov.sequence_index,
                         color_of(ov.colors.default_color), -1.f, {}, level);
                }
            }

        if(auto elements = hud.overlay_elements.data(*m_magic); elements.has_value())
            for(auto const& el : elements.value())
            {
                auto overlays = el.overlays.data(*m_magic);
                if(!on_this_map(el.can_use_on_map_type) || !overlays.has_value())
                    continue;
                using type_t = weapon_hud_interface::overlay_t::type_t;
                for(auto const& ov : overlays.value())
                    if((ov.type & (type_t::show_always | type_t::show_on_default)) !=
                           type_t::none &&
                       !blank(ov.colors.default_color))
                        push(anchor, ov, el.overlay_bitmap, ov.sequence_index,
                             color_of(ov.colors.default_color));
            }

        auto child = hud.child_hud.valid() ? m_index.find(hud.child_hud.tag_id) : m_index.end();
        weapon_hud(child != m_index.end() ? &(*child) : nullptr, s, depth + 1);
    }

    static state_t state_of(
        BipedEquipment::weapon_t const& held,
        BipedEquipment::weapon_t const& slung,
        blam::map_ptr const&            magic,
        u8                              zoom = 0)
    {
        state_t s{
            .total            = static_cast<f32>(held.rounds_total),
            .loaded           = static_cast<f32>(held.rounds_loaded),
            .secondary_total  = static_cast<f32>(slung.rounds_total),
            .secondary_loaded = static_cast<f32>(slung.rounds_loaded),
            .heat             = std::min(held.heat, 1.f),
            .battery          = 1.f - held.age,
            .zoom             = zoom,
        };
        auto triggers  = held.weapon->triggers.data(magic);
        auto magazines = held.weapon->magazines.data(magic);
        if(triggers.has_value() && !triggers.value().empty() && magazines.has_value())
            if(auto const i = triggers.value()[0].magazine;
               i >= 0 && static_cast<size_t>(i) < magazines.value().size())
            {
                auto const& magazine = magazines.value()[i];
                s.loaded_max = std::max<f32>(magazine.rounds_loaded_maximum, 1.f);
                s.total_max  = std::max<f32>(magazine.rounds_reserved_maximum, 1.f);
            }
        return s;
    }

    /* ---- Vehicles ---- */

    struct riding_t
    {
        bool                     armed{true}; /*!< Its own weapon in hand */
        blam::tag_t const*       hud{nullptr}; /*!< The seat's, over the biped's */
        BipedEquipment::weapon_t mounted{};    /*!< A gunner's, full */
    };

    /* A rider's seat: its HUD, and the vehicle's gun for its gunner */
    riding_t rider_of(Proxy& p, PlayerInfo const& info)
    {
        using flags_t       = blam::scn::unit::seat_t::seat_flags_t;
        auto const* vehicle = info.riding.vehicle ? p.template get<Model>(info.riding.vehicle) : nullptr;
        if(!vehicle || !vehicle->origin_object)
            return {};
        auto unit = vehicle->origin_object->template data<blam::scn::unit>(*m_magic);
        if(!unit.has_value())
            return {};
        auto seats = unit.value()[0].seats.data(*m_magic);
        if(!seats.has_value() || info.riding.seat < 0 ||
           static_cast<size_t>(info.riding.seat) >= seats.value().size())
            return {};
        auto const& seat = seats.value()[info.riding.seat];
        riding_t    out{.armed = (seat.flags & flags_t::allows_weapons) != flags_t::none};
        if(auto huds = seat.hud_interfaces.data(*m_magic); huds.has_value() && !huds.value().empty())
            if(auto tag = m_index.find(huds.value()[0].hud.tag_id);
               huds.value()[0].hud.valid() && tag != m_index.end())
                out.hud = &(*tag);
        auto weapons = unit.value()[0].weapons.data(*m_magic);
        if((seat.flags & flags_t::gunner) == flags_t::none || !weapons.has_value() ||
           weapons.value().empty())
            return out;
        auto tag = m_index.find(weapons.value()[0].weapon.tag_id);
        if(!weapons.value()[0].weapon.valid() || tag == m_index.end())
            return out;
        auto weapon = (*tag).template data<blam::scn::weapon>(*m_magic);
        if(!weapon.has_value())
            return out;
        out.mounted = {.weapon = weapon.value(), .tag = &(*tag)};
        auto triggers  = weapon.value()->triggers.data(*m_magic);
        auto magazines = weapon.value()->magazines.data(*m_magic);
        if(triggers.has_value() && !triggers.value().empty() && magazines.has_value())
            if(auto const i = triggers.value()[0].magazine;
               i >= 0 && static_cast<size_t>(i) < magazines.value().size())
            {
                out.mounted.rounds_loaded = static_cast<u16>(
                    std::max<i16>(magazines.value()[i].rounds_loaded_maximum, 0));
                out.mounted.rounds_total = static_cast<u16>(
                    std::max<i16>(magazines.value()[i].rounds_reserved_maximum, 0));
            }
        return out;
    }

    /* ---- Grenades ---- */

    void grenade_hud(blam::tag_t const* tag, i32 count)
    {
        if(!tag)
            return;
        auto data = tag->template data<grenade_hud_interface>(*m_magic);
        if(!data.has_value())
            return;
        auto const& hud = *data.value();
        background(hud.anchor, hud.grenade_hud_background);
        background(hud.anchor, hud.total_grenades_background);
        number(hud.anchor, hud.total_grenades_numbers, count);
        auto overlays = hud.total_grenades_overlays.overlays.data(*m_magic);
        if(!overlays.has_value())
            return;
        using type_t = grenade_hud_interface::overlay_t::type_t;
        type_t const now = count > 0 ? type_t::show_on_default : type_t::show_on_empty;
        for(auto const& ov : overlays.value())
            if((ov.type & (now | type_t::show_always)) != type_t::none &&
               !blank(ov.colors.default_color))
                push(hud.anchor, ov, hud.total_grenades_overlays.overlay_bitmap,
                     ov.sequence_index, color_of(ov.colors.default_color));
    }

    /* A cutscene title, in the 640x480 space the tag places it in, centred
     * on the window */
    void title(Proxy& p, CinematicState::title_t& t, f32 dt, Vecf2 window)
    {
        t.elapsed += dt;
        f32 const   scale = window.y / HudScene::height;
        f32 const   left  = window.x * .5f - 320.f * scale;
        f32 const   x     = glm::mix(t.bounds.y, t.bounds.w, t.align);
        Vecf4       color = t.color;
        color.a *= t.opacity();
        p.template subsystem<ScreenText>().lines.push_back({
            .text     = t.text,
            .baseline = {left + x * scale, (t.bounds.x + t.bounds.z) * .5f * scale},
            .color    = color,
            .align    = t.align,
        });
    }

    /* ---- Messages ---- */

    using run_t = HudScene::text_t::run_t;

    /* Icons past the hudg's buttons are game actions, then values the
     * message is given */
    static constexpr i32 first_action = 18;
    static constexpr i32 first_value  = 32;
    /* Each action's button on the Xbox's default layout; -1 for unknown */
    static constexpr auto action_buttons = std::to_array<libc_types::i8>({
        2,  /* action: X */
        6,  /* throw grenade: left trigger */
        7,  /* fire: right trigger */
        5,  /* flashlight: white */
        0,  /* jump: A */
        1,  /* melee: B */
        3,  /* switch weapons: Y */
        -1, -1,
        14, /* crouch: left thumb */
        -1, -1,
        16, /* move: left stick */
        17, /* look: right stick */
    });

    /* The fields of a hudg button icon or a messaging_information */
    struct icon_t
    {
        i16           sequence_index;
        i16           width_offset;
        blam::vec2i16 offset;
        blam::argb8_t color;
        u8            flags;
    };

    /* A sprite of the hudg's message icons, placed and spaced as the icon
     * says */
    std::optional<run_t> icon_run(icon_t const& icon, Vecf4 const& color)
    {
        using flags_t = blam::hud_globals::button_icon_t::flags_t;
        auto const bitmap = m_hudg ? m_index.find(m_hudg->icon_bitmap.tag_id) : m_index.end();
        if(bitmap == m_index.end())
            return std::nullopt;
        auto header = (*bitmap).template data<blam::bitm::header_t>(*m_magic);
        if(!header.has_value())
            return std::nullopt;
        /* Some shipped tags point past the sheet */
        auto seqs = header.value()->sequences.data(*m_magic);
        if(!seqs.has_value() || icon.sequence_index < 0 ||
           static_cast<size_t>(icon.sequence_index) >= seqs.value().size())
            return std::nullopt;
        auto frame = frame_of(m_hudg->icon_bitmap, icon.sequence_index, 0);
        if(!frame)
            return std::nullopt;
        auto const  has    = [&icon](flags_t f) { return (icon.flags & static_cast<u8>(f)) != 0; };
        Vecf2 const offset = Vecf2(icon.offset.x, icon.offset.y) * offset_scale;
        frame->size        *= sprite_scale;
        frame->sprite.min   = offset;
        frame->sprite.max   = offset + frame->size;
        frame->sprite.color = has(flags_t::override_default_color) ? color_of(icon.color) : color;
        return run_t{
            .icon    = frame->sprite,
            .advance = has(flags_t::width_offset_is_absolute_icon_width)
                           ? icon.width_offset * offset_scale
                           : frame->size.x + icon.width_offset * offset_scale,
        };
    }

    /* A button icon of the hudg, or its stand-in text */
    std::optional<run_t> button_run(i32 button)
    {
        using flags_t = blam::hud_globals::button_icon_t::flags_t;
        if(!m_hudg)
            return std::nullopt;
        auto buttons = m_hudg->button_icons.data(*m_magic);
        if(!buttons.has_value() || button < 0 || static_cast<size_t>(button) >= buttons.value().size())
            return std::nullopt;
        auto const& b = buttons.value()[button];
        if((static_cast<u8>(b.flags) & static_cast<u8>(flags_t::use_text_from_string_list)) == 0)
            return icon_run({b.sequence_index, b.width_offset, b.offset, b.color, static_cast<u8>(b.flags)}, icon_color());
        auto const list = located_of<blam::ui::unicode_string_list>(m_hudg->alternate_icon_text);
        if(!list)
            return std::nullopt;
        auto subs = list->first->data.data(list->second);
        if(!subs.has_value() || b.text_index < 0 || static_cast<size_t>(b.text_index) >= subs.value().size())
            return std::nullopt;
        auto text = subs.value()[b.text_index].str(list->second);
        if(!text.has_value())
            return std::nullopt;
        return run_t{.text = std::u16string(text.value())};
    }

    Vecf4 icon_color() const
    {
        /* Stored a, r, g, b */
        return m_hudg ? Vecf4(m_hudg->icon_color.y, m_hudg->icon_color.z, m_hudg->icon_color.w, m_hudg->icon_color.x)
                      : Vecf4(1.f);
    }

    /* A message as runs of text and icons; `values` stand in for the icons
     * past the actions */
    std::vector<run_t> message_of(
        blam::hud_message_text const* hmt,
        blam::map_ptr const&          magic,
        std::string_view              name,
        std::span<run_t const>        values = {})
    {
        std::vector<run_t> out;
        if(!hmt)
            return out;
        auto messages = hmt->messages.data(magic);
        auto elements = hmt->elements.data(magic);
        auto text     = hmt->text(magic);
        if(!messages.has_value() || !elements.has_value() || !text.has_value())
            return out;
        auto const message = std::find_if(messages.value().begin(), messages.value().end(), [name](auto const& m) {
            return m.name.str() == name;
        });
        if(message == messages.value().end())
            return out;
        using type_t = blam::hud_message_text::element_t::type_t;
        auto append  = [&out](run_t run) {
            if(out.empty() || out.back().icon)
                out.emplace_back();
            out.back().text += run.text;
            out.back().icon    = run.icon;
            out.back().advance = run.advance;
        };
        size_t at = static_cast<size_t>(std::max<i16>(message->text_start, 0));
        for(i32 i = 0; i < message->element_count; i++)
        {
            size_t const e = static_cast<size_t>(message->element_start) + i;
            if(e >= elements.value().size())
                break;
            auto const& element = elements.value()[e];
            if(element.type == type_t::text)
            {
                auto const     chars = text.value().subspan(std::min(at, text.value().size()));
                std::u16string run(chars.begin(), chars.begin() + std::min<size_t>(std::max<libc_types::i8>(element.data, 0), chars.size()));
                run.erase(std::find(run.begin(), run.end(), u'\0'), run.end());
                /* "|n" breaks the line */
                for(size_t n; (n = run.find(u"|n")) != std::u16string::npos;)
                    run.replace(n, 2, u"\n");
                append({.text = run});
                at += static_cast<size_t>(std::max<libc_types::i8>(element.data, 0));
                continue;
            }
            i32 const icon = element.data;
            std::optional<run_t> run;
            if(icon >= first_value)
            {
                if(static_cast<size_t>(icon - first_value) < values.size())
                    run = values[icon - first_value];
            } else if(icon >= first_action)
            {
                if(static_cast<size_t>(icon - first_action) < action_buttons.size())
                    run = button_run(action_buttons[icon - first_action]);
            } else
                run = button_run(icon);
            if(run)
                append(*run);
        }
        return out;
    }

    /* Flashing for its first few beats of each period, as hudg's help text
     * colors say */
    static bool lit(unit_hud_interface::colors_t const& c, bool flash, f32 t)
    {
        using flags_t = unit_hud_interface::colors_t::flash_flags_t;
        bool on       = false;
        if(flash && c.flash_period > 0.f)
        {
            f32 const in   = std::fmod(t, c.flash_period);
            f32 const beat = c.flash_length + c.flash_delay;
            i32 const n    = beat > 0.f ? static_cast<i32>(in / beat) : 0;
            on             = n < c.num_flashes && in - n * beat < c.flash_length;
        }
        if((c.flash_flags & flags_t::reverse_default_flashing_colors) != flags_t::none)
            on = !on;
        return on;
    }

    static Vecf4 flashing(unit_hud_interface::colors_t const& c, bool flash, f32 t)
    {
        Vecf4 const color = color_of(lit(c, flash, t) ? c.flashing_color : c.default_color);
        /* Glyphs are drawn over the world, not added to it */
        return Vecf4(Vecf3(color), 1.f);
    }

    /* The weapon a press of use would swap the one in hand for */
    blam::scn::weapon const* offered(Proxy& p, PlayerInfo const& info)
    {
        auto const* model = info.offered.weapon ? p.template get<Model>(info.offered.weapon) : nullptr;
        if(!model || !model->origin_object)
            return nullptr;
        auto weapon = model->origin_object->template data<blam::scn::weapon>(*m_magic);
        return weapon.has_value() ? weapon.value() : nullptr;
    }

    /* The scripts' help text, and what the player could pick up */
    void messages(Proxy& p, CinematicState const& cinematic, PlayerInfo const& info, PlayerCamera const& cam, BipedEquipment const& equip)
    {
        if(!m_hudg)
            return;
        Vecf2 const at = place(m_hudg->anchor, m_hudg->placement.anchor_offset, Vecf2());
        countdown(cinematic.timer);
        if(cinematic.help.shown && !cinematic.help.message.empty())
            if(auto runs = message_of(m_messages, *m_magic, cinematic.help.message); !runs.empty())
                m_view->texts.push_back({
                    .runs  = std::move(runs),
                    .at    = at,
                    .color = flashing(m_hudg->help_text_colors, cinematic.help.flash, cinematic.help.elapsed),
                });
        /* A seat in reach is what use boards, so its prompt comes first */
        if(auto runs = seat_prompt(p, info); !runs.empty())
        {
            Vecf4 const text = m_hudg->text_color;
            m_view->texts.push_back({
                .runs  = std::move(runs),
                .at    = at,
                .color = Vecf4(text.y, text.z, text.w, 1.f),
                .below = !m_view->texts.empty(),
            });
            return;
        }
        if(auto runs = device_prompt(p, info, cam); !runs.empty())
        {
            Vecf4 const text = m_hudg->text_color;
            m_view->texts.push_back({
                .runs  = std::move(runs),
                .at    = at,
                .color = Vecf4(text.y, text.z, text.w, 1.f),
                .below = !m_view->texts.empty(),
            });
            return;
        }
        auto const* weapon = offered(p, info);
        auto hud = weapon && weapon->hud_interface.valid() ? m_index.find(weapon->hud_interface.tag_id) : m_index.end();
        if(hud == m_index.end())
            return;
        auto wphi = (*hud).template data<weapon_hud_interface>(*m_magic);
        if(!wphi.has_value())
            return;
        auto const& info_ = wphi.value()->messaging_information;
        auto const  icon  = icon_run(
            {info_.sequence_index,
             info_.width_offset,
             info_.offset_from_reference_corner,
             info_.override_icon_color,
             static_cast<u8>(info_.flags)},
            icon_color());
        if(!icon)
            return;
        Vecf4 const text = m_hudg->text_color;
        if(auto runs = message_of(m_global_messages, m_global_magic, "swap_weapon", std::span(&*icon, 1)); !runs.empty())
            m_view->texts.push_back({
                .runs  = std::move(runs),
                .at    = at,
                .color = Vecf4(text.y, text.z, text.w, 1.f),
                .below = !m_view->texts.empty(),
            });
    }

    /* A string of the hudg's icon text list, as a run */
    std::optional<run_t> icon_text(i32 index)
    {
        if(!m_hudg)
            return std::nullopt;
        auto const list = located_of<blam::ui::unicode_string_list>(m_hudg->alternate_icon_text);
        if(!list)
            return std::nullopt;
        auto subs = list->first->data.data(list->second);
        if(!subs.has_value() || index < 0 || static_cast<size_t>(index) >= subs.value().size())
            return std::nullopt;
        auto text = subs.value()[index].str(list->second);
        if(!text.has_value())
            return std::nullopt;
        return run_t{.text = std::u16string(text.value())};
    }

    /* "Press X to <word>" for a control in reach whose object names one in
     * the hudg's icon text, else "Press X to touch device" */
    std::vector<run_t> device_prompt(Proxy& p, PlayerInfo const& info, PlayerCamera const& cam)
    {
        if(info.riding.vehicle != 0)
            return {};
        auto const aimed = aimed_control(p, cam);
        if(!aimed.usable)
            return {};
        auto const* control = p.template get<DeviceState>(aimed.id)->control;
        if(i16 const word = control->export_.hud_msg.index; word > 0)
            if(auto const run = icon_text(word))
                return message_of(m_global_messages, m_global_magic, "custom_device", std::span(&*run, 1));
        return message_of(m_global_messages, m_global_magic, "touch_device");
    }

    /* The scripts' countdown as m:ss, in the hudg's text colour until it
     * runs into its warning time */
    void countdown(CinematicState::timer_t const& timer)
    {
        if(!timer.shown)
            return;
        auto const seconds = (std::max<libc_types::i64>(timer.ticks, 0) + 29) / 30;
        auto const text    = fmt::format("{}:{:02}", seconds / 60, seconds % 60);
        Vecf4 const normal = m_hudg->text_color;
        bool const  warn   = timer.ticks <= timer.warning;
        m_view->texts.push_back({
            .runs  = {{.text = std::u16string(text.begin(), text.end())}},
            .at    = place(static_cast<anchor_t>(std::min<u16>(timer.corner, 4)),
                        blam::vec2i16{timer.x, timer.y},
                        Vecf2(countdown_width, countdown_height)),
            .color = warn ? Vecf4(1.f, .25f, .2f, 1.f) : Vecf4(normal.y, normal.z, normal.w, 1.f),
        });
    }
    static constexpr f32 countdown_width  = 36.f; /* px, "0:00" in the HUD font */
    static constexpr f32 countdown_height = 14.f;

    /* A screen effect's mask over the whole view, under the rest of the
     * HUD: the sniper scope's vignette */
    void scope_mask(weapon_hud_interface const& hud, bool zoomed)
    {
        using flags_t = weapon_hud_interface::screen_effect_t::flags_t;
        auto effects  = hud.screen_effects.data(*m_magic);
        if(!effects.has_value())
            return;
        for(auto const& fx : effects.value())
        {
            if((static_cast<u16>(fx.mask.flags) & static_cast<u16>(flags_t::only_when_zoomed)) && !zoomed)
                continue;
            auto const& bitmap = m_split && fx.mask.mask_splitscreen.valid() ? fx.mask.mask_splitscreen
                                                                             : fx.mask.mask_fullscreen;
            auto frame = frame_of(bitmap, 0, 0);
            if(!frame)
                continue;
            frame->sprite.min   = Vecf2(0.f);
            frame->sprite.max   = Vecf2(m_width, HudScene::height);
            /* The mask's alpha is how much of the view it blacks out */
            frame->sprite.color = Vecf4(0.f, 0.f, 0.f, 1.f);
            m_view->sprites.insert(m_view->sprites.begin(), frame->sprite);
        }
    }

    /* The objective the scripts set, as text */
    std::u16string objective_text(std::string_view name)
    {
        std::u16string out;
        if(m_magic)
            for(auto const& run : message_of(m_messages, *m_magic, name))
                out += run.text;
        return out;
    }

    /* "Press X to enter <seat> seat of <vehicle>": the seat's and the
     * vehicle's words are indices into the hudg's icon text */
    std::vector<run_t> seat_prompt(Proxy& p, PlayerInfo const& info)
    {
        auto const* vehicle = info.offered.vehicle ? p.template get<Model>(info.offered.vehicle) : nullptr;
        if(!vehicle || !vehicle->origin_object || info.offered.seat < 0)
            return {};
        auto unit = vehicle->origin_object->template data<blam::scn::unit>(*m_magic);
        if(!unit.has_value())
            return {};
        auto seats = unit.value()[0].seats.data(*m_magic);
        if(!seats.has_value() || static_cast<size_t>(info.offered.seat) >= seats.value().size())
            return {};
        auto const role = icon_text(seats.value()[info.offered.seat].hud_text_message_index);
        auto const name = icon_text(unit.value()[0].export_.hud_msg.index);
        if(!role || !name)
            return {};
        std::array<run_t, 2> const values{*role, *name};
        return message_of(m_global_messages, m_global_magic, "enter_vehicle", values);
    }

    /* ---- Scoreboard ---- */

    static std::u16string wide(std::string_view text)
    {
        return std::u16string(text.begin(), text.end());
    }

    /* ---- Kill messages ---- */

    /* PC maps' ui\multiplayer_game_text, by place; Xbox maps carry none and
     * get the same words */
    enum game_text_t : size_t
    {
        text_guardians     = 76,
        text_vehicle       = 77,
        text_killed_by     = 78,
        text_betrayed_by   = 79,
        text_suicide       = 81,
        text_you_betrayed  = 82,
        text_you_killed    = 88,
    };

    std::u16string game_text(game_text_t index, std::string_view fallback) const
    {
        return index < m_game_text.size() && !m_game_text[index].empty() ? m_game_text[index] : wide(fallback);
    }

    /* %s filled in order */
    static std::u16string fill(std::u16string const& format, std::initializer_list<std::u16string> names)
    {
        std::u16string out;
        auto           name = names.begin();
        for(size_t i = 0; i < format.size(); i++)
            if(format[i] == u'%' && i + 1 < format.size() && format[i + 1] == u's' && name != names.end())
            {
                out += *name++;
                i++;
            } else
                out += format[i];
        return out;
    }

    /* The deaths of the last few seconds, as this player is told them: their
     * own kills as theirs, betrayals as such, the rest by what did it */
    void kill_feed(Proxy& p, PlayerInfo const& viewer)
    {
        if(m_feed.empty() || !m_hudg)
            return;
        auto const& game    = p.template subsystem<GameState>();
        auto const  name_of = [&p](u32 player_idx) {
            for(auto player : p.template select<PlayerInfo>())
                if(auto const& info = player.template get<PlayerInfo>();
                   info.player_idx == player_idx && !info.name.empty())
                    return wide(info.name);
            return wide(fmt::format("Player {}", player_idx + 1));
        };
        auto const teammates = [&game](u32 a, u32 b) {
            auto const *x = game.player(a), *y = game.player(b);
            return game.teams && x && y && x->team >= 0 && x->team == y->team;
        };
        std::u16string text;
        for(auto const& [kill, _] : m_feed)
        {
            bool const by_player = kill.killer != ObjectHitEvent::no_player;
            std::u16string line;
            if(by_player && kill.killer == kill.victim)
                line = fill(game_text(text_suicide, "%s committed suicide"), {name_of(kill.victim)});
            else if(by_player && kill.killer == viewer.player_idx)
                line = teammates(kill.killer, kill.victim)
                           ? fill(game_text(text_you_betrayed, "You betrayed %s"), {name_of(kill.victim)})
                           : fill(game_text(text_you_killed, "You killed %s"), {name_of(kill.victim)});
            else if(by_player)
                line = fill(
                    teammates(kill.killer, kill.victim) ? game_text(text_betrayed_by, "%s was betrayed by %s")
                                                        : game_text(text_killed_by, "%s was killed by %s"),
                    {name_of(kill.victim), name_of(kill.killer)});
            else if(kill.cause == UnitKilledEvent::vehicle)
                line = fill(game_text(text_vehicle, "%s was killed by a vehicle"), {name_of(kill.victim)});
            else
                line = fill(game_text(text_guardians, "%s was killed by the guardians"), {name_of(kill.victim)});
            if(!text.empty())
                text += u'\n';
            text += line;
        }
        Vecf4 const color = m_hudg->text_color;
        m_view->texts.push_back({
            .runs  = {{.text = std::move(text)}},
            .at    = {safe_inset.x, HudScene::height - safe_inset.y - feed_bottom - feed_line * m_feed.size()},
            .color = Vecf4(color.y, color.z, color.w, 1.f),
        });
    }

    struct feed_t
    {
        KillFeedEvent kill;
        f32           age{0.f};
    };
    static constexpr f32    feed_seconds = 6.f;   /* a message stays, my pick */
    static constexpr size_t feed_lines   = 4;
    static constexpr f32    feed_line    = 14.f;  /* HUD px */
    static constexpr f32    feed_bottom  = 110.f; /* HUD px above the bottom, clear of the motion sensor */

    /* Everyone's score, best first */
    void scoreboard(Proxy& p, GameState const& game, Vecf2 window)
    {
        f32 const scale  = window.y / HudScene::height;
        f32 const centre = window.x * .5f;
        auto&     lines  = p.template subsystem<ScreenText>().lines;
        f32       y      = scoreboard_top;
        /* A shadow under each line keeps it legible over the world */
        auto line = [&](std::string const& text, f32 x, Vecf4 color, f32 align) {
            Vecf2 const at{centre + x * scale, y * scale};
            lines.push_back({
                .text     = wide(text),
                .baseline = at + Vecf2(scale),
                .color    = shadow,
                .align    = align,
            });
            lines.push_back({.text = wide(text), .baseline = at, .color = color, .align = align});
        };
        auto name_of = [&p](u32 player_idx) {
            for(auto player : p.template select<PlayerInfo>())
                if(auto const& info = player.template get<PlayerInfo>();
                   info.player_idx == player_idx && !info.name.empty())
                    return std::string(info.name);
            return fmt::format("Player {}", player_idx + 1);
        };
        static constexpr std::array<std::string_view, 2> team_names = {"Red", "Blue"};

        line(std::string(GameState::title(game.game_type, game.teams)), 0.f, heading, .5f);
        y += scoreboard_row;
        if(game.phase == GameState::phase_t::over)
        {
            std::string const who =
                game.winner < 0 ? std::string("Nobody")
                : game.teams    ? std::string(team_names[game.winner % 2]) + " team"
                                : name_of(static_cast<u32>(game.winner));
            line(who + " wins", 0.f, highlight, .5f);
            y += scoreboard_row;
        } else if(game.time_left >= 0.f)
        {
            i32 const left = static_cast<i32>(game.time_left);
            line(fmt::format("{}:{:02} left", left / 60, left % 60), 0.f, plain, .5f);
            y += scoreboard_row;
        }
        if(game.teams)
        {
            line(fmt::format("Red {}  -  Blue {}", game.team_scores[0], game.team_scores[1]),
                 0.f, plain, .5f);
            y += scoreboard_row;
        }
        y += scoreboard_row * .5f;
        line("Player", -scoreboard_width, heading, 0.f);
        line("Score", score_column, heading, 1.f);
        line("Kills", kills_column, heading, 1.f);
        line("Deaths", deaths_column, heading, 1.f);
        y += scoreboard_row;

        auto ranked = game.players;
        std::sort(ranked.begin(), ranked.end(), [](auto const& a, auto const& b) {
            return a.score != b.score ? a.score > b.score : a.kills > b.kills;
        });
        for(auto const& player : ranked)
        {
            Vecf4 const color = player.team == 0   ? red
                                : player.team == 1 ? blue
                                                   : plain;
            line(name_of(player.player_idx), -scoreboard_width, color, 0.f);
            line(std::to_string(player.score), score_column, color, 1.f);
            line(std::to_string(player.kills), kills_column, color, 1.f);
            line(std::to_string(player.deaths), deaths_column, color, 1.f);
            y += scoreboard_row;
        }
    }

    /* In 640x480 HUD units */
    static constexpr f32 scoreboard_top   = 110.f;
    static constexpr f32 scoreboard_row   = 22.f;
    static constexpr f32 scoreboard_width = 210.f; /* names, left of centre */
    static constexpr f32 score_column     = 60.f;  /* right edges */
    static constexpr f32 kills_column     = 135.f;
    static constexpr f32 deaths_column    = 220.f;
    static inline Vecf4 const shadow{0.f, 0.f, 0.f, .8f};
    static inline Vecf4 const heading{.7f, .85f, 1.f, 1.f};
    static inline Vecf4 const highlight{1.f, .9f, .4f, 1.f};
    static inline Vecf4 const plain{1.f, 1.f, 1.f, 1.f};
    static inline Vecf4 const red{1.f, .45f, .4f, 1.f};
    static inline Vecf4 const blue{.45f, .65f, 1.f, 1.f};

    /* ---- Frame ---- */

    void start_restricted(Proxy& p, time_point const& t)
    {
        auto& scene = p.template subsystem<HudScene>();
        scene.views.clear();

        /* Cutscenes cover it all */
        auto& cinematic = p.template subsystem<CinematicState>();
        f32 const dt    = m_last != time_point{} ? std::chrono::duration<f32>(t - m_last).count() : 0.f;
        m_last          = t;
        m_dt            = dt;
        m_blink_time += dt;
        if(m_feed_queue)
            m_feed_queue->poll();
        for(auto& line : m_feed)
            line.age += dt;
        std::erase_if(m_feed, [](feed_t const& line) { return line.age > feed_seconds; });
        while(m_feed.size() > feed_lines)
            m_feed.erase(m_feed.begin());
        cinematic.fade.elapsed += dt;
        cinematic.help.elapsed += dt;
        scene.fade = Vecf4(cinematic.fade.color, cinematic.fade.opacity());
        scene.letterbox = glm::clamp(
            scene.letterbox + (cinematic.letterbox ? dt : -dt) / letterbox_seconds * letterbox_height,
            0.f,
            letterbox_height);
        if(auto* fb = p.template service<comp_app::GraphicsFramebuffer>(); fb && cinematic.title)
            title(p, *cinematic.title, dt, Vecf2(fb->size().w, fb->size().h));
        if(cinematic.title && cinematic.title->opacity() <= 0.f &&
           cinematic.title->elapsed > cinematic.title->fade_in)
            cinematic.title.reset();
        auto const& files   = p.template subsystem<BlamFiles<halo_version>>();
        auto const& loading = p.template subsystem<LoadingStatus>();
        if(loading.loaded_map != LoadingStatus::loaded ||
           files.container.map->map_type == blam::maptype_t::ui)
            return;
        if(scene.generation != files.load_generation)
            load(files);
        scene.generation = files.load_generation;

        auto* fb = p.template service<comp_app::GraphicsFramebuffer>();
        if(!fb)
            return;

        std::vector<std::pair<u32, u64>> seats;
        auto const cutscene = cutscene_seat(p, cinematic.playing);
        for(auto player : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = player.components();
            if(!info.is_remote() && cam.is_active() &&
               (!cutscene || info.seat_idx == *cutscene))
                seats.emplace_back(info.seat_idx, player.id());
        }
        std::sort(seats.begin(), seats.end());
        /* Same split as the world's viewports */
        Vecf2 const screen(fb->size().w, fb->size().h);
        auto const& game = p.template subsystem<GameState>();
        bool const  asked = std::any_of(seats.begin(), seats.end(), [&p](auto const& seat) {
            auto const* input = p.template get<PlayerInput>(seat.second);
            return input && input->scores;
        });
        if(game.kind == GameState::kind_t::multiplayer &&
           (asked || game.shows_results()))
            scoreboard(p, game, screen);
        Vecf2 const view = seats.size() <= 1   ? screen
                           : seats.size() == 2 ? Vecf2(screen.x, screen.y / 2)
                                               : screen / 2.f;

        for(auto const& [seat, id] : seats)
        {
            auto& out = scene.views.emplace_back(HudScene::view_t{
                .seat  = seat,
                .width = HudScene::height * view.x / std::max(view.y, 1.f),
            });
            auto const* info   = p.template get<PlayerInfo>(id);
            auto const* cam    = p.template get<PlayerCamera>(id);
            auto const* net    = p.template get<NetworkInfo>(id);
            auto const* equip  = p.template get<BipedEquipment>(id);
            auto const* status = p.template get<BipedStatus>(id);
            /* Full spectrum vision, fading out over its last second */
            if(info && info->powerups.vision > 0.f)
                out.tint = vision_tint * std::min(info->powerups.vision, 1.f);
            /* Deaths are told while dead too */
            if(info && cam && cinematic.show_hud && !cam->cinematic)
            {
                m_view  = &out;
                m_width = out.width;
                kill_feed(p, *info);
            }
            if(!info || !cam || !net || !equip || !status || !cinematic.show_hud ||
               cam->cinematic || !biped_shown(*info, *cam, *net) || !info->mode.physics ||
               status->dead)
                continue;
            m_view  = &out;
            m_width = out.width;

            size_t const split  = seats.size() > 1 ? 1 : 0;
            m_split             = split != 0;
            auto const   riding = rider_of(p, *info);
            if(auto const* unit = riding.hud ? riding.hud : m_unit_huds[split])
                if(auto hud = unit->template data<unit_hud_interface>(*m_magic);
                   hud.has_value())
                    unit_hud(p, id, *hud.value(), *status);
            m_crosshair = p.template subsystem<CinematicState>().hud_parts.crosshair;
            if(riding.mounted.weapon)
            {
                auto hud = m_index.find(riding.mounted.weapon->hud_interface.tag_id);
                weapon_hud(
                    riding.mounted.weapon->hud_interface.valid() && hud != m_index.end() ? &(*hud)
                                                                                        : nullptr,
                    state_of(riding.mounted, {}, *m_magic));
            } else if(auto const* held = riding.armed ? equip->held(files.load_generation) : nullptr)
            {
                auto hud = m_index.find(held->weapon->hud_interface.tag_id);
                weapon_hud(
                    held->weapon->hud_interface.valid() && hud != m_index.end() ? &(*hud)
                                                                                : nullptr,
                    state_of(*held, equip->weapons[1], *m_magic, equip->zoom));
            }
            /* The grenades it would throw */
            if(equip->grenade < equip->equipment.size())
            {
                auto const& carried = equip->equipment[equip->grenade];
                for(auto const& [equipment, hud] : m_grenades)
                    if(carried.equip == equipment)
                        grenade_hud(hud, carried.left);
            }
            messages(p, cinematic, *info, *cam, *equip);
            nav_points(p, *info, *cam);
        }
        m_view = nullptr;
    }

    /* A tag's data, if it is in the map and of T's class */
    template<typename T>
    T const* data_of(blam::tagref_t const& ref) const
    {
        auto tag = ref.valid() ? m_index.find(ref.tag_id) : m_index.end();
        if(tag == m_index.end() || !(*tag).matches(T::tag_class))
            return nullptr;
        auto data = (*tag).template data<T>(*m_magic);
        return data.has_value() ? data.value() : nullptr;
    }

    /* A tag's data with the magic its pointers need: the map's, or the
     * loc.map blob's for a tag a Custom Edition map only indexes */
    template<typename T>
    std::optional<std::pair<T const*, blam::map_ptr>> located_of(blam::tagref_t const& ref) const
    {
        auto tag = ref.valid() ? m_index.find(ref.tag_id) : m_index.end();
        if(tag == m_index.end() || !(*tag).matches(T::tag_class))
            return std::nullopt;
        if(auto loc = m_index.template localized<T>(*tag); loc)
            return loc;
        auto data = (*tag).template data<T>(*m_magic);
        if(!data.has_value())
            return std::nullopt;
        return std::make_pair(data.value(), *m_magic);
    }

    /* What this map's HUD is drawn from */
    void load(BlamFiles<halo_version> const& files)
    {
        m_magic       = &files.container.magic;
        m_index       = blam::tag_index_view<halo_version>(files.container);
        if(files.loc_file)
            m_index.add_atlas(
                blam::atlas_type_t::localization,
                blam::map_ptr(static_cast<semantic::BytesConst>(*files.loc_file)));
        if(files.bitmap_file)
            m_index.add_atlas(
                blam::atlas_type_t::bitmaps,
                blam::map_ptr(static_cast<semantic::BytesConst>(*files.bitmap_file)));
        m_multiplayer = files.container.map->map_type == blam::maptype_t::multiplayer;
        m_feed.clear();
        m_game_text.clear();
        if(auto text = m_index.tag_of("ui\\multiplayer_game_text"); text.has_value())
            if(auto list = (*text)->template data<blam::ui::unicode_string_list>(*m_magic); list.has_value())
                if(auto strings = list.value()->data.data(*m_magic); strings.has_value())
                    for(auto const& ref : strings.value())
                    {
                        auto s = ref.str(*m_magic);
                        m_game_text.emplace_back(s.has_value() ? std::u16string(s.value()) : std::u16string{});
                    }
        m_unit_huds   = {};
        m_digits      = nullptr;
        m_grenades.clear();
        m_hudg            = nullptr;
        m_messages        = nullptr;
        m_global_messages = nullptr;
        if(auto const* scenario = files.container.scenario().value_or(nullptr))
            m_messages = data_of<blam::hud_message_text>(scenario->ui_text.hud_text);

        auto globals_tag = m_index.tag_of("globals\\globals");
        if(!globals_tag.has_value())
            return;
        auto globals = (*globals_tag)->template data<blam::globals::globals>(*m_magic);
        if(!globals.has_value())
            return;
        auto const& g = *globals.value();

        if(auto bitmaps = g.interface.data(*m_magic); bitmaps.has_value() && !bitmaps.value().empty())
            m_blip = bitmaps.value()[0].motion_sensor_blip_bitmap;
        if(auto bitmaps = g.interface.data(*m_magic); bitmaps.has_value() && !bitmaps.value().empty())
            if((m_hudg = data_of<blam::hud_globals>(bitmaps.value()[0].hud_globals)))
                if(auto hmt = located_of<blam::hud_message_text>(m_hudg->hud_messages); hmt)
                    std::tie(m_global_messages, m_global_magic) = *hmt;
if(auto bitmaps = g.interface.data(*m_magic); bitmaps.has_value() && !bitmaps.value().empty())
            if(auto digits = m_index.find(bitmaps.value()[0].hud_digits_definition.tag_id);
               bitmaps.value()[0].hud_digits_definition.valid() && digits != m_index.end())
                if(auto d = (*digits).template data<blam::hud_number>(*m_magic); d.has_value())
                    m_digits = d.value();
        if(m_digits)
            if(auto found = m_index.template resource<blam::bitm::header_t>(m_digits->digits_bitmap.tag_id))
            {
                auto const& [tag, header, magic] = *found;
                if(auto seqs = header->sequences.data(magic); seqs.has_value() && !seqs.value().empty())
                    if(auto sprites = seqs.value()[0].sprites.data(magic); sprites.has_value())
                        m_digit_sprites = sprites.value().size() >= 10;
            }

        if(auto grenades = g.grenades.data(*m_magic); grenades.has_value())
            for(auto const& grenade : grenades.value())
            {
                auto eqip = m_index.find(grenade.equipment.tag_id);
                auto hud  = m_index.find(grenade.hud_interface.tag_id);
                if(!grenade.equipment.valid() || eqip == m_index.end())
                    continue;
                auto data = (*eqip).template data<blam::scn::equipment>(*m_magic);
                m_grenades.emplace_back(
                    data.has_value() ? data.value() : nullptr,
                    grenade.hud_interface.valid() && hud != m_index.end() ? &(*hud) : nullptr);
            }

        /* The player's unit: the multiplayer one on multiplayer maps */
        blam::tagref_t unit_ref{};
        if(auto mp = g.multiplayer.data(*m_magic);
           m_multiplayer && mp.has_value() && !mp.value().empty())
            unit_ref = mp.value()[0].unit;
        else if(auto sp = g.player.data(*m_magic); sp.has_value() && !sp.value().empty())
            unit_ref = sp.value()[0].unit;
        auto biped = m_index.find(unit_ref.tag_id);
        if(!unit_ref.valid() || biped == m_index.end())
            return;
        auto unit = (*biped).template data<blam::scn::unit>(*m_magic);
        if(!unit.has_value())
            return;
        auto huds = unit.value()->hud_interfaces.data(*m_magic);
        if(!huds.has_value() || huds.value().empty())
            return;
        /* The first is drawn full screen, the second in split screen */
        for(size_t i = 0; i < m_unit_huds.size(); i++)
        {
            auto const& hud = huds.value()[std::min(i, huds.value().size() - 1)].hud;
            if(auto tag = m_index.find(hud.tag_id); hud.valid() && tag != m_index.end())
                m_unit_huds[i] = &(*tag);
        }
    }

    static constexpr u32 max_child_depth  = 4;
    static constexpr f32 damage_flash_seconds = .5f;
    /* Bars of a letterbox, and how long they take to come in */
    static constexpr f32 letterbox_height  = .125f;
    static constexpr f32 letterbox_seconds = .5f;
    time_point           m_last{};
    f32                  m_dt{0.f};
    f32                  m_blink_time{0.f}; /*!< Clock the hud_blink_* flashes beat to */
    std::shared_ptr<GameEventBus::queue_type<KillFeedEvent>> m_feed_queue;
    std::vector<feed_t>                                      m_feed;
    std::vector<std::u16string>                              m_game_text;
    static constexpr f32 low_ammo_fraction = .25f;
    /* 7.5% in from each edge of 640x480 */
    static inline Vecf2 const safe_inset{48.f, 36.f};

    blam::map_ptr const*                 m_magic{nullptr};
    bool                                 m_crosshair{true}; /*!< hud_show_crosshair */
    bool                                 m_split{false};    /*!< Views share the window */
    blam::tag_index_view<halo_version>   m_index;
    bool                                 m_multiplayer{false};
    std::array<blam::tag_t const*, 2>    m_unit_huds{}; /*!< Full, split screen */
    /* Dims the view and lifts it green */
    static inline Vecf4 const vision_tint{.04f, .22f, .06f, .4f};
    std::map<u64, warning_t> m_warnings; /*!< Per seat */
    blam::hud_number const*              m_digits{nullptr};
    blam::hud_globals const*             m_hudg{nullptr};
    blam::tagref_t                       m_blip{}; /*!< matg's motion sensor blip */
    blam::hud_message_text const*        m_messages{nullptr};        /*!< The scenario's */
    blam::hud_message_text const*        m_global_messages{nullptr}; /*!< The hudg's */
    blam::map_ptr                        m_global_magic{};           /*!< What it resolves with */
    bool                                 m_digit_sprites{false};
    /* Each grenade type's equipment, and the HUD that counts it */
    std::vector<std::pair<blam::scn::equipment const*, blam::tag_t const*>> m_grenades;

    HudScene::view_t* m_view{nullptr};
    f32               m_width{640.f};
};

void alloc_hud(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<HudScene>();
    auto& hud         = e.register_subsystem_inplace<HudSystem>();
    hud.m_feed_queue = e.subsystem_cast<GameEventBus>().addQueuedEventFunction<KillFeedEvent>(
        0, [&hud](GameEvent&, KillFeedEvent* feed) { hud.m_feed.push_back({.kill = *feed}); });
}

void alloc_objective_provider(compo::EntityContainer& e)
{
    auto& hud       = e.subsystem_cast<HudSystem>();
    auto& cinematic = e.subsystem_cast<CinematicState>();
    e.subsystem_cast<UIDataSource>().bind_text(
        "mission_objective_text", [&hud, &cinematic](std::u16string_view text) {
            if(cinematic.objective.empty())
                return std::u16string(text);
            return hud.objective_text(cinematic.objective);
        });
}
