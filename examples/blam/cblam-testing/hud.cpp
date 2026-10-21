#include "hud.h"
#include "game_mode.h"

#include "blam_files.h"
#include "cinematic.h"
#include "components.h"
#include "selected_version.h"
#include "ui.h"

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_globals.h>
#include <blam/volta/blam_ui.h>
#include <blam/volta/blam_unit.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <string>

using HudManifest = compo::SubsystemManifest<
    type_list_t<
        const PlayerInfo,
        const PlayerCamera,
        const NetworkInfo,
        const BipedEquipment,
        const BipedStatus,
        const PlayerInput>,
    type_list_t<
        HudScene,
        CinematicState,
        ScreenText,
        const GameState,
        const BlamFiles<halo_version>,
        const LoadingStatus>,
    type_list_t<comp_app::GraphicsFramebuffer>>;

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
        auto tag = bitmap.valid() ? m_index.find(bitmap.tag_id) : m_index.end();
        if(tag == m_index.end())
            return std::nullopt;
        auto header = (*tag).template data<blam::bitm::header_t>(*m_magic);
        if(!header.has_value())
            return std::nullopt;
        auto images    = header.value()->images.data(*m_magic);
        auto sequences = header.value()->sequences.data(*m_magic);
        if(!images.has_value() || images.value().empty())
            return std::nullopt;
        frame_t out{.sprite = {.bitmap = &bitmap}};
        u16 image = 0;
        if(sequences.has_value() && sequence >= 0 &&
           static_cast<size_t>(sequence) < sequences.value().size())
        {
            auto const& seq     = sequences.value()[sequence];
            auto        sprites = seq.sprites.data(*m_magic);
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
        Vecf2 const o = Vecf2(offset.x, offset.y) + (anchor == anchor_t::center ? Vecf2() : safe_inset);
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
               (high_res ? .5f : 1.f);
    }

    void push(
        anchor_t                                     anchor,
        unit_hud_interface::background_base_t const& base,
        blam::tagref_t const&                        bitmap,
        i16                                          sequence,
        Vecf4 const&                                 color,
        f32                                          fill  = -1.f,
        Vecf4 const&                                 empty = {})
    {
        auto frame = frame_of(bitmap, sequence, 0);
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

    void background(anchor_t anchor, unit_hud_interface::background_t const& bg)
    {
        if(blank(bg.colors.default_color))
            return;
        push(anchor, bg, bg.interface_bitmap, bg.sequence_index, color_of(bg.colors.default_color));
    }

    /* Lit up to `value` (0..1) along the meter bitmap's alpha ramp */
    void meter(anchor_t anchor, unit_hud_interface::meter_base_t const& m, f32 value)
    {
        f32 const low   = m.minimum_meter_value / 255.f;
        f32 const fill  = value > 0.f ? low + (1.f - low) * std::clamp(value, 0.f, 1.f) : 0.f;
        Vecf4 const lit = glm::mix(color_of(m.color_at_minimum), color_of(m.color_at_maximum), value);
        push(anchor, m, m.meter_bitmap, m.sequence_index, lit, fill, color_of(m.empty_color));
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

    void unit_hud(unit_hud_interface const& hud, BipedStatus const& status)
    {
        f32 const shield = status.shield;
        f32 const health = status.body;
        background(hud.anchor, hud.unit_hud_background);
        background(hud.anchor, hud.shield_panel_background);
        meter(hud.anchor, hud.shield_panel_meter, shield);
        background(hud.anchor, hud.health_panel_background);
        meter(hud.anchor, hud.health_panel_meter, health);
        /* The motion sensor keeps to the bottom left */
        background(anchor_t::bottom_left, hud.motion_sensor_background);
        background(anchor_t::bottom_left, hud.motion_sensor_foreground);
    }

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
        default:
            return 0.f;
        }
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

        if(auto statics = hud.screen_alignment.static_elements.data(*m_magic); statics.has_value())
            for(auto const& el : statics.value())
                if(on_this_map(el.can_use_on_map_type))
                    background(anchor, el.background);
        if(auto meters = hud.meter_elements.data(*m_magic); meters.has_value())
            for(auto const& el : meters.value())
                if(on_this_map(el.can_use_on_map_type))
                    meter(anchor, el.meter, value_of(el.state_attached_to, s, true));
        if(auto numbers = hud.number_elements.data(*m_magic); numbers.has_value())
            for(auto const& el : numbers.value())
            {
                if(!on_this_map(el.can_use_on_map_type))
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
                if(!on_this_map(ch.can_use_on_map_type) || !shown(ch.crosshair_type))
                    continue;
                auto overlays = ch.crosshair_overlays.data(*m_magic);
                if(!overlays.has_value())
                    continue;
                using flags_t = weapon_hud_interface::crosshair_overlay_t::flags_t;
                for(auto const& ov : overlays.value())
                    if((ov.flags & flags_t::show_only_when_zoomed) == flags_t::none &&
                       !blank(ov.colors.default_color))
                        push(anchor_t::center, ov, ch.crosshair_bitmap, ov.sequence_index,
                             color_of(ov.colors.default_color));
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

    static state_t state_of(BipedEquipment::weapon_t const& held, BipedEquipment::weapon_t const& slung, blam::map_ptr const& magic)
    {
        state_t s{
            .total            = static_cast<f32>(held.rounds_total),
            .loaded           = static_cast<f32>(held.rounds_loaded),
            .secondary_total  = static_cast<f32>(slung.rounds_total),
            .secondary_loaded = static_cast<f32>(slung.rounds_loaded),
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

    /* ---- Scoreboard ---- */

    static std::u16string wide(std::string_view text)
    {
        return std::u16string(text.begin(), text.end());
    }

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
        cinematic.fade.elapsed += dt;
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
        for(auto player : p.template select<PlayerInfo, PlayerCamera>())
        {
            auto [info, cam] = player.components();
            if(!info.is_remote() && cam.is_active())
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
           (asked || game.phase == GameState::phase_t::over))
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
            if(!info || !cam || !net || !equip || !status || !cinematic.show_hud ||
               cam->cinematic || !biped_in_play(*info, *cam, *net) || !info->mode.physics ||
               status->dead)
                continue;
            m_view  = &out;
            m_width = out.width;

            if(auto const* unit = m_unit_huds[seats.size() > 1 ? 1 : 0])
                if(auto hud = unit->template data<unit_hud_interface>(*m_magic);
                   hud.has_value())
                    unit_hud(*hud.value(), *status);
            if(auto const* held = equip->held(files.load_generation))
            {
                auto hud = m_index.find(held->weapon->hud_interface.tag_id);
                weapon_hud(
                    held->weapon->hud_interface.valid() && hud != m_index.end() ? &(*hud)
                                                                                : nullptr,
                    state_of(*held, equip->weapons[1], *m_magic));
            }
            /* The grenades it would throw */
            if(equip->grenade < equip->equipment.size())
            {
                auto const& carried = equip->equipment[equip->grenade];
                for(auto const& [equipment, hud] : m_grenades)
                    if(carried.equip == equipment)
                        grenade_hud(hud, carried.left);
            }
        }
        m_view = nullptr;
    }

    /* What this map's HUD is drawn from */
    void load(BlamFiles<halo_version> const& files)
    {
        m_magic       = &files.container.magic;
        m_index       = blam::tag_index_view<halo_version>(files.container);
        m_multiplayer = files.container.map->map_type == blam::maptype_t::multiplayer;
        m_unit_huds   = {};
        m_digits      = nullptr;
        m_grenades.clear();

        auto globals_tag = m_index.tag_of("globals\\globals");
        if(!globals_tag.has_value())
            return;
        auto globals = (*globals_tag)->template data<blam::globals::globals>(*m_magic);
        if(!globals.has_value())
            return;
        auto const& g = *globals.value();

        if(auto bitmaps = g.interface.data(*m_magic); bitmaps.has_value() && !bitmaps.value().empty())
            if(auto digits = m_index.find(bitmaps.value()[0].hud_digits_definition.tag_id);
               bitmaps.value()[0].hud_digits_definition.valid() && digits != m_index.end())
                if(auto d = (*digits).template data<blam::hud_number>(*m_magic); d.has_value())
                    m_digits = d.value();
        if(m_digits)
            if(auto tag = m_index.find(m_digits->digits_bitmap.tag_id); tag != m_index.end())
                if(auto header = (*tag).template data<blam::bitm::header_t>(*m_magic); header.has_value())
                    if(auto seqs = header.value()->sequences.data(*m_magic);
                       seqs.has_value() && !seqs.value().empty())
                        if(auto sprites = seqs.value()[0].sprites.data(*m_magic); sprites.has_value())
                            m_digit_sprites = sprites.value().size() >= 10;

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
    /* Bars of a letterbox, and how long they take to come in */
    static constexpr f32 letterbox_height  = .125f;
    static constexpr f32 letterbox_seconds = .5f;
    time_point           m_last{};
    static constexpr f32 low_ammo_fraction = .25f;
    /* 7.5% in from each edge of 640x480 */
    static inline Vecf2 const safe_inset{48.f, 36.f};

    blam::map_ptr const*                 m_magic{nullptr};
    blam::tag_index_view<halo_version>   m_index;
    bool                                 m_multiplayer{false};
    std::array<blam::tag_t const*, 2>    m_unit_huds{}; /*!< Full, split screen */
    blam::hud_number const*              m_digits{nullptr};
    bool                                 m_digit_sprites{false};
    /* Each grenade type's equipment, and the HUD that counts it */
    std::vector<std::pair<blam::scn::equipment const*, blam::tag_t const*>> m_grenades;

    HudScene::view_t* m_view{nullptr};
    f32               m_width{640.f};
};

void alloc_hud(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<HudScene>();
    e.register_subsystem_inplace<HudSystem>();
}
