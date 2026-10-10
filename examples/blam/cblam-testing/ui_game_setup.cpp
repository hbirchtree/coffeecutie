#include "ui_game_setup.h"

#include "blam_files.h"
#include "data.h"
#include "network/networking.h"
#include "selected_version.h"
#include "ui_caching.h"
#include "ui_data.h"
#include "ui_profile.h"

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

void GameSetup::start_frame(ContainerProxy&, time_point const&)
{
    if(!m_pending)
        return;
    auto const map = *std::exchange(m_pending, std::nullopt);
    auto       name = blam::bl_string::from(map);
    if(!name)
        return;
    Coffee::Logging::cDebug("UI: starting {}", map);
    m_container.subsystem_cast<RenderingParameters>().render_ui = false;
    GameEvent          ev{GameEvent::MapLoadByName};
    MapLoadByNameEvent load{.map_name = *name};
    m_container.subsystem_cast<GameEventBus>().inject(ev, &load);
}

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
        func_t::net_game_speed_start, [&setup, mp_maps](UIFunctionCall const&) {
            if(setup.mp_map >= mp_maps->entries.size())
                return ui_result_t::failed;
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
}
