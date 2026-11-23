#include "ui/ui_game_setup.h"

#include "loading/blam_files.h"
#include "data.h"
#include "network/networking.h"
#include "selected_version.h"
#include "ui/ui.h"
#include "ui/ui_caching.h"
#include "ui/ui_data.h"
#include "ui/ui_profile.h"

#include <utf8cpp/utf8.h>

#include <coffee/core/CDebug>
#include <magic_enum/magic_enum.hpp>

#include <array>

using libc_types::u16;
using platform::url::Path;

namespace {

/* Same order as ui\shell\main_menu\map_list and the sp_levels pictures */
constexpr std::array<std::string_view, 10> campaign_maps = {
    "a10", "a30", "a50", "b30", "b40", "c10", "c20", "c40", "d20", "d40"};

/* Same order as ui\shell\main_menu\mp_map_list and mp_map_grafix */
constexpr std::array<std::string_view, 13> multiplayer_maps = {
    "beavercreek",
    "sidewinder",
    "damnation",
    "ratrace",
    "prisoner",
    "hangemhigh",
    "chillout",
    "carousel",
    "boardingaction",
    "bloodgulch",
    "wizard",
    "putput",
    "longest",
};

/* Pictures follow game_type_grafix, names ui\multiplayer_game_text */
struct game_type_info_t
{
    GameSetup::game_type_t type;
    u16                    name;
};
constexpr std::array<game_type_info_t, 5> game_types = {{
    {GameSetup::game_type_t::ctf, 3},
    {GameSetup::game_type_t::koth, 6},
    {GameSetup::game_type_t::slayer, 4},
    {GameSetup::game_type_t::oddball, 5},
    {GameSetup::game_type_t::race, 7},
}};

/* One generated list: which table entries the map directory has, and the
 * strings for their names and descriptions */
struct list_source_t
{
    std::vector<u16>            entries;
    std::vector<std::u16string> names;
    std::vector<std::u16string> descriptions;

    std::u16string text(std::string_view widget, u16 item) const
    {
        if(item >= entries.size())
            return {};
        auto const& from = widget.ends_with("_name") ? names
                           : widget.ends_with("_data") ? descriptions
                                                       : names;
        auto const index = entries[item];
        return index < from.size() ? from[index] : std::u16string{};
    }
};

std::vector<std::u16string> strings_of(
    compo::EntityContainer& e, std::string_view tag_name)
{
    auto& ui = e.subsystem_cast<UIElementCache<halo_version>>();
    std::vector<std::u16string> out;
    for(blam::tag_t const& tag : ui.index)
    {
        if(!tag.matches(blam::tag_class_t::unicode_string) ||
           tag.to_name().to_string(ui.magic) != tag_name)
            continue;
        auto list = tag.template data<blam::ui::unicode_string_list>(ui.magic);
        if(!list.has_value())
            break;
        if(auto strings = list.value()->data.data(ui.magic); strings.has_value())
            for(auto const& ref : strings.value())
            {
                auto s = ref.str(ui.magic);
                out.emplace_back(
                    s.has_error() ? std::u16string{} : std::u16string(s.value()));
            }
        break;
    }
    return out;
}

/* Maps missing from the map directory are left out, unless none can be
 * found at all, e.g. where files can't be looked up */
template<size_t N>
std::vector<u16> available_maps(
    compo::EntityContainer& e, std::array<std::string_view, N> const& maps)
{
    auto const&      dir = e.subsystem_cast<BlamFiles<halo_version>>().map_directory;
    std::vector<u16> out;
    for(u16 i = 0; i < maps.size(); ++i)
    {
        auto const file = dir / Path(std::string(maps[i])).addExtension("map");
        if(platform::file::file_info(file).has_value())
            out.push_back(i);
    }
    if(out.empty())
        for(u16 i = 0; i < maps.size(); ++i)
            out.push_back(i);
    return out;
}

} // namespace

std::optional<std::string_view> next_campaign_map(std::string_view map)
{
    for(size_t i = 0; i + 1 < campaign_maps.size(); i++)
        if(campaign_maps[i] == map)
            return campaign_maps[i + 1];
    return std::nullopt;
}

namespace {

/* Lines as short as the stock error strings': the error box's text area
 * reaches under its frame on the right, so wrapping to it overflows */
std::u16string break_lines(std::u16string_view text)
{
    constexpr size_t line_length = 24;
    std::u16string   out;
    size_t           line = 0;
    while(!text.empty())
    {
        auto const end  = text.find(u' ');
        auto       word = text.substr(0, end);
        text.remove_prefix(end == std::u16string_view::npos ? text.size() : end + 1);
        while(!word.empty())
        {
            if(line > 0 && line + 1 + word.size() > line_length)
            {
                out += u'\n';
                line = 0;
            } else if(line > 0)
            {
                out += u' ';
                ++line;
            }
            auto const part = word.substr(0, line_length);
            out += part;
            line += part.size();
            word.remove_prefix(part.size());
        }
    }
    return out;
}

bool in_menu(compo::EntityContainer& e)
{
    auto const* map = e.subsystem_cast<BlamFiles<halo_version>>().container.map;
    return map && map->map_type == blam::maptype_t::ui &&
           !e.subsystem_cast<LoadingStatus>().loading;
}

} // namespace

void GameSetup::leave(std::string message)
{
    if(!message.empty())
        m_error = std::move(message);
    if(!in_menu(m_container))
        start("ui");
}

void GameSetup::start_frame(ContainerProxy&, time_point const&)
{
    if(m_pending)
    {
        auto const map = *std::exchange(m_pending, std::nullopt);
        auto       name = blam::bl_string::from(map);
        if(!name)
            return;
        Coffee::Logging::cDebug("UI: starting {}", map);
        m_container.subsystem_cast<RenderingParameters>().render_ui = false;
        GameEvent          ev{GameEvent::MapLoadByName};
        MapLoadByNameEvent load{.map_name = *name};
        m_container.subsystem_cast<GameEventBus>().inject(ev, &load);
        return;
    }
    if(!m_error || !in_menu(m_container))
        return;
    m_error_text = break_lines(utf8::utf8to16(utf8::replace_invalid(*m_error)));
    m_error.reset();
    UIEvent      ev{.type = UIEvent::open_widget};
    UIOpenWidget open{.widget = "ui\\shell\\error\\error_modal_fullscreen"};
    m_container.subsystem_cast<UIEventBus>().inject(ev, &open);
}

/* Strings of ui\multiplayer_game_text, as the pregame's text boxes index them */
namespace pregame_text {
constexpr libc_types::i32 blank              = 1;
constexpr libc_types::i32 ctf                = 3;
constexpr libc_types::i32 slayer             = 4;
constexpr libc_types::i32 oddball            = 5;
constexpr libc_types::i32 koth               = 6;
constexpr libc_types::i32 race               = 7;
constexpr libc_types::i32 team_game          = 12;
constexpr libc_types::i32 free_for_all       = 13;
constexpr libc_types::i32 captures           = 22;
constexpr libc_types::i32 minutes            = 23;
constexpr libc_types::i32 frags              = 24;
constexpr libc_types::i32 laps               = 25;
constexpr libc_types::i32 red_team           = 17;
constexpr libc_types::i32 choose_teams       = 26;
constexpr libc_types::i32 form_two_teams     = 27;
constexpr libc_types::i32 juggernaut         = 32;
constexpr libc_types::i32 waiting_for_player = 35;
} // namespace pregame_text

void alloc_game_setup_provider(compo::EntityContainer& e)
{
    using func_t  = blam::ui_element::function_t;
    using input_t = blam::ui_element::data_function_t;
    using list_t  = UIDataSource::list_t;

    auto& setup = e.register_subsystem_inplace<GameSetup>(std::ref(e));
    auto& data  = e.subsystem_cast<UIDataSource>();

    auto levels     = std::make_shared<list_source_t>();
    auto mp_maps    = std::make_shared<list_source_t>();
    auto game_names = std::make_shared<list_source_t>();

    auto ok = [](UIFunctionCall const&) { return ui_result_t::ok; };

    /* The list widgets' created handlers; the map directory or map may
     * have changed since last time */
    auto init_levels = [&e, levels](UIFunctionCall const&) {
        levels->entries      = available_maps(e, campaign_maps);
        levels->names        = strings_of(e, "ui\\shell\\main_menu\\map_list");
        levels->descriptions = strings_of(
            e, "ui\\shell\\main_menu\\solo_level_select\\map_data");
        return ui_result_t::ok;
    };
    data.on_function(func_t::initialize_sp_level_list_solo, init_levels);
    data.on_function(func_t::initialize_sp_level_list_coop, init_levels);
    data.on_function(func_t::dispose_sp_level_list, ok);
    data.on_function(func_t::solo_level_set_map, ok);

    data.on_function(func_t::mp_level_list_initialize, [&e, mp_maps](UIFunctionCall const&) {
        mp_maps->entries      = available_maps(e, multiplayer_maps);
        mp_maps->names        = strings_of(e, "ui\\shell\\main_menu\\mp_map_list");
        mp_maps->descriptions = strings_of(
            e,
            "ui\\shell\\main_menu\\multiplayer_type_select\\mp_map_select\\map_"
            "data");
        return ui_result_t::ok;
    });
    data.on_function(func_t::mp_level_list_dispose, ok);
    data.on_function(func_t::mp_level_select, ok);

    data.on_function(
        func_t::mp_profiles_list_initialize, [&e, game_names](UIFunctionCall const&) {
            game_names->entries.clear();
            for(u16 i = 0; i < game_types.size(); ++i)
                game_names->entries.push_back(game_types[i].name);
            game_names->names = strings_of(e, "ui\\multiplayer_game_text");
            game_names->descriptions.clear();
            return ui_result_t::ok;
        });
    data.on_function(func_t::mp_profiles_list_dispose, ok);
    data.on_function(func_t::mp_profile_set_for_game, ok);

    /* The difficulty list is a plain column list; its focus is the pick */
    data.on_function(func_t::difficulty_menu_init, ok);
    data.on_function(func_t::set_difficulty, [&setup](UIFunctionCall const& call) {
        if(call.selected)
            setup.difficulty = *call.selected;
        return ui_result_t::ok;
    });

    auto start_campaign = [&e, &setup, levels](UIFunctionCall const&) {
        /* Seats come from the split screen lobby, which this isn't */
        e.subsystem_cast<LocalLobby>() = {};
        if(setup.level >= levels->entries.size())
            return ui_result_t::failed;
        auto const map = campaign_maps[levels->entries[setup.level]];
        Coffee::Logging::cDebug(
            "UI: campaign {} on difficulty {}", map, setup.difficulty);
        setup.start(std::string(map));
        return ui_result_t::ok;
    };
    data.on_function(func_t::main_menu_switch_to_solo_game, start_campaign);
    data.on_function(func_t::start_new_game, start_campaign);

    data.on_function(
        func_t::net_game_speed_start, [&e, &setup, mp_maps](UIFunctionCall const&) {
            if(setup.mp_map >= mp_maps->entries.size())
                return ui_result_t::failed;
            /* A team game needs someone on each side */
            auto const& lobby = e.subsystem_cast<LocalLobby>();
            if(setup.game_type == GameSetup::game_type_t::ctf && lobby.seated_controllers().size() > 1 &&
               !lobby.both_teams())
                return ui_result_t::failed;
            setup.seat_team = lobby.team;
            auto const map = multiplayer_maps[mp_maps->entries[setup.mp_map]];
            Coffee::Logging::cDebug(
                "UI: multiplayer {} ({})",
                map,
                magic_enum::enum_name(setup.game_type));
            setup.start(std::string(map));
            return ui_result_t::ok;
        });

    /* QUIT / LEAVE GAME: disconnect, then load ui.map */
    auto leave_game = [&e, &setup](UIFunctionCall const&) {
        GameEvent             ev{GameEvent::ServerDisconnect};
        ServerDisconnectEvent disconnect{};
        e.subsystem_cast<GameEventBus>().inject(ev, &disconnect);
        setup.start("ui");
        return ui_result_t::ok;
    };
    data.on_function(func_t::pause_game_return_to_main_menu, leave_game);
    data.on_function(func_t::mp_game_player_quit, leave_game);

    /* Out of a network game: back to the menu, with a dialog saying why
     * unless the host simply left */
    e.subsystem_cast<GameEventBus>().addEventFunction<ServerDisconnectedEvent>(
        0, [&setup](GameEvent&, ServerDisconnectedEvent* gone) {
            using reason_t = ServerDisconnectedEvent::reason_t;
            std::string message = gone->message;
            if(message.empty())
                switch(gone->reason)
                {
                case reason_t::closed:
                    message = "The server closed the connection.";
                    break;
                case reason_t::connection_lost:
                    message = "Lost the connection to the server.";
                    break;
                case reason_t::join_failed:
                    message = "Couldn't join the game.";
                    break;
                case reason_t::hosting_failed:
                    message = "Couldn't host the game.";
                    break;
                case reason_t::host_left:
                    break;
                }
            setup.leave(std::move(message));
        });
    data.bind_text("error_text_box", [&setup](std::u16string_view text) {
        return setup.error_text().empty() ? std::u16string(text)
                                          : setup.error_text();
    });

    /* The selection lives in GameSetup, the lists only page through it */
    auto clamp_to = [](std::shared_ptr<list_source_t> const& src, u16 value) {
        return src->entries.empty()
                   ? u16(0)
                   : std::min<u16>(value, static_cast<u16>(src->entries.size() - 1));
    };
    data.bind_list(
        input_t::solo_level_select_update,
        list_t{
            .count = [levels] { return static_cast<u16>(levels->entries.size()); },
            .get   = [&setup] { return setup.level; },
            .set   = [&setup, levels, clamp_to](
                       u16 value) { setup.level = clamp_to(levels, value); },
            .text  = [levels](std::string_view widget, u16 item) {
                return levels->text(widget, item);
            },
            .image = [levels](u16 item) { return levels->entries.at(item); },
        });
    data.bind_list(
        input_t::mp_level_select_update,
        list_t{
            .count = [mp_maps] { return static_cast<u16>(mp_maps->entries.size()); },
            .get   = [&setup] { return setup.mp_map; },
            .set   = [&setup, mp_maps, clamp_to](
                       u16 value) { setup.mp_map = clamp_to(mp_maps, value); },
            .text  = [mp_maps](std::string_view widget, u16 item) {
                return mp_maps->text(widget, item);
            },
            .image = [mp_maps](u16 item) { return mp_maps->entries.at(item); },
        });
    /* Built-in game types only; saved variants would go first */
    data.bind_list(
        input_t::mp_profile_list_update,
        list_t{
            .count = [] { return static_cast<u16>(game_types.size()); },
            .get   = [&setup] {
                for(u16 i = 0; i < game_types.size(); ++i)
                    if(game_types[i].type == setup.game_type)
                        return i;
                return u16(0);
            },
            .set = [&setup](u16 value) {
                if(value < game_types.size())
                    setup.game_type = game_types[value].type;
            },
            .text = [game_names](std::string_view widget, u16 item) {
                /* No description string for the built-in ones */
                return widget.ends_with("_data") ? std::u16string{}
                                                 : game_names->text(widget, item);
            },
            .image = [](u16 item) { return item; },
        });

    /* The pregame screen's summary of the game about to start: its text
     * boxes pick from ui\multiplayer_game_text by their game data input */
    auto const& lobby    = e.subsystem_cast<LocalLobby>();
    auto const& profiles = e.subsystem_cast<PlayerProfile>();
    auto const  option   = [&setup](std::string_view name) -> std::optional<int> {
        for(auto const& [key, value] : setup.options)
            if(key == name)
                return std::atoi(value.c_str());
        return std::nullopt;
    };
    auto const juggernaut = [option] { return option("ball_type").value_or(0) == 2; };
    using t = GameSetup::game_type_t;
    data.bind_index(input_t::mp_set_textbox_game_ruleset, [&setup, juggernaut]() -> std::optional<i32> {
        switch(setup.game_type)
        {
        case t::ctf:
            return pregame_text::ctf;
        case t::slayer:
            return pregame_text::slayer;
        case t::oddball:
            return juggernaut() ? pregame_text::juggernaut : pregame_text::oddball;
        case t::koth:
            return pregame_text::koth;
        case t::race:
            return pregame_text::race;
        }
        return std::nullopt;
    });
    data.bind_index(input_t::mp_set_textbox_teams_noteams, [&setup]() -> std::optional<i32> {
        return setup.game_type == t::ctf ? pregame_text::team_game : pregame_text::free_for_all;
    });
    data.bind_index(input_t::mp_set_textbox_score_limit_type, [&setup, juggernaut]() -> std::optional<i32> {
        switch(setup.game_type)
        {
        case t::ctf:
            return pregame_text::captures;
        case t::koth:
            return pregame_text::minutes;
        case t::oddball:
            return juggernaut() ? pregame_text::frags : pregame_text::minutes;
        case t::race:
            return pregame_text::laps;
        default:
            return pregame_text::frags;
        }
    });
    data.bind_index(input_t::mp_game_directions, [&setup, &lobby]() -> std::optional<i32> {
        if(lobby.seated_controllers().size() < 2)
            return pregame_text::waiting_for_player;
        if(setup.game_type != t::ctf)
            return pregame_text::blank;
        return lobby.both_teams() ? pregame_text::choose_teams : pregame_text::form_two_teams;
    });
    /* The rules' own defaults, as each game type program declares them */
    data.bind_text("score_limit_value_text_box", [&setup, option, juggernaut](std::u16string_view) {
        int value = 0;
        switch(setup.game_type)
        {
        case t::ctf:
            value = option("score_limit").value_or(3);
            break;
        case t::koth:
            value = (option("score_limit").value_or(120) + 59) / 60;
            break;
        case t::oddball:
            value = juggernaut() ? option("kill_limit").value_or(15) : (option("score_limit").value_or(120) + 59) / 60;
            break;
        case t::race:
            value = option("score_limit").value_or(3);
            break;
        default:
            value = option("score_limit").value_or(25);
            break;
        }
        auto const text = std::to_string(value);
        return std::u16string(text.begin(), text.end());
    });
    data.bind_text("number_of_players_text_box", [&lobby](std::u16string_view) {
        auto const text = std::to_string(std::max<size_t>(lobby.seated_controllers().size(), 1));
        return std::u16string(text.begin(), text.end());
    });
    /* game_type_grafix: CTF, KOTH, Slayer, Oddball, Race */
    data.bind_index(input_t::mp_set_bitmap_for_ruleset, [&setup]() -> std::optional<i32> {
        switch(setup.game_type)
        {
        case t::ctf:
            return 0;
        case t::koth:
            return 1;
        case t::slayer:
            return 2;
        case t::oddball:
            return 3;
        case t::race:
            return 4;
        }
        return std::nullopt;
    });
    /* No countdown to start: its label stays hidden */
    data.bind_text("autostart_label", [](std::u16string_view) { return std::u16string{}; });
    /* The pregame's players: one row a joined controller, with its profile;
     * teams only in a team game */
    auto const joined = [&lobby](u16 row) -> std::optional<u32> {
        auto const seats = lobby.seated_controllers();
        return row < seats.size() ? std::optional<u32>(seats[row]) : std::nullopt;
    };
    auto const game_text   = std::make_shared<std::vector<std::u16string>>();
    auto const player_rows = [&e, &setup, &profiles, &lobby, joined, game_text] {
        return UIDataSource::list_t{
            .count = [&lobby] { return static_cast<u16>(std::max<size_t>(lobby.seated_controllers().size(), 1)); },
            .get   = [] { return u16(0); },
            .set   = [](u16) {},
            .text  = [&e, &setup, &profiles, &lobby, joined, game_text](std::string_view widget, u16 row) -> std::u16string {
                auto const seat = joined(row).value_or(0);
                if(widget == "local_player_name")
                    return profiles.at(lobby.profile_of(seat)).name;
                if(widget == "team_spinner")
                {
                    if(setup.game_type != t::ctf)
                        return u" ";
                    if(game_text->empty())
                        *game_text = strings_of(e, "ui\\multiplayer_game_text");
                    auto const at = static_cast<size_t>(pregame_text::red_team + (lobby.team[seat] & 1));
                    return at < game_text->size() ? (*game_text)[at] : std::u16string{};
                }
                return {};
            },
            /* controller_graphic: 0 is the empty port */
            .image = [](u16) { return u16(1); },
        };
    };
    data.bind_list(input_t::splitscreen_pregame_status_update, player_rows());
    /* System link shows this machine's players the same way */
    data.bind_list(input_t::network_pregame_status_update, player_rows());
    /* Each quadrant names the profile its controller picked */
    data.bind_seat_text("qtr_screen_profile_selected_name", [&lobby, &profiles](std::u16string_view, u32 seat) {
        return profiles.at(lobby.profile_of(seat)).name;
    });
    data.bind_seat_text("local_player_name", [&lobby, &profiles](std::u16string_view, u32 seat) {
        return profiles.at(lobby.profile_of(seat)).name;
    });
}
