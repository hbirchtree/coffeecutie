#include "ui/ui_profile.h"

#include "ui/ui_data.h"

#include <coffee/core/CDebug>
#include <coffee/core/files/cfiles.h>
#include <nlohmann/json.hpp>

using namespace Coffee::resource_literals;

/* As the community documents CE's player colour table */
std::array<PlayerProfile::color_t, 18> const PlayerProfile::colors{{
    {u"White", {1.f, 1.f, 1.f}},
    {u"Black", {.1f, .1f, .1f}},
    {u"Red", {.996f, 0.f, 0.f}},
    {u"Blue", {.008f, .004f, .89f}},
    {u"Gray", {.439f, .494f, .443f}},
    {u"Yellow", {1.f, 1.f, .004f}},
    {u"Green", {0.f, 1.f, .004f}},
    {u"Pink", {1.f, .337f, .725f}},
    {u"Purple", {.671f, .063f, .957f}},
    {u"Cyan", {.004f, 1.f, 1.f}},
    {u"Cobalt", {.392f, .576f, .929f}},
    {u"Orange", {1.f, .498f, 0.f}},
    {u"Teal", {.118f, .8f, .569f}},
    {u"Sage", {0.f, .392f, .004f}},
    {u"Brown", {.376f, .22f, .078f}},
    {u"Tan", {.776f, .612f, .424f}},
    {u"Maroon", {.616f, .043f, .055f}},
    {u"Salmon", {.961f, .6f, .62f}},
}};

namespace {

std::string to_utf8(std::u16string const& text)
{
    std::string out;
    for(char16_t c : text)
        if(c < 0x80)
            out.push_back(static_cast<char>(c));
        else if(c < 0x800)
        {
            out.push_back(static_cast<char>(0xC0 | (c >> 6)));
            out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        } else
        {
            out.push_back(static_cast<char>(0xE0 | (c >> 12)));
            out.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
            out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
        }
    return out;
}

std::u16string from_utf8(std::string const& text)
{
    std::u16string out;
    for(size_t i = 0; i < text.size();)
    {
        auto const c = static_cast<unsigned char>(text[i]);
        if(c < 0x80)
            out.push_back(c), i += 1;
        else if((c >> 5) == 6 && i + 1 < text.size())
            out.push_back(static_cast<char16_t>(((c & 0x1F) << 6) | (text[i + 1] & 0x3F))), i += 2;
        else if((c >> 4) == 14 && i + 2 < text.size())
            out.push_back(static_cast<char16_t>(((c & 0x0F) << 12) | ((text[i + 1] & 0x3F) << 6) | (text[i + 2] & 0x3F))),
                i += 3;
        else
            i += 1;
    }
    return out;
}

} // namespace

void PlayerProfile::load()
{
    auto file = "profiles.json"_config;
    if(!Coffee::FilePull(file))
        return;
    auto const doc = nlohmann::json::parse(file.data(), nullptr, false);
    if(!doc.is_object() || !doc.contains("profiles") || !doc["profiles"].is_array())
        return;
    std::vector<profile_t> loaded;
    for(auto const& entry : doc["profiles"])
    {
        profile_t profile;
        profile.name  = from_utf8(entry.value("name", std::string("Player")));
        profile.color = static_cast<libc_types::u16>(std::min<int>(entry.value("color", 0), colors.size() - 1));
        if(auto controls = entry.find("controls"); controls != entry.end() && controls->is_object())
        {
            profile.controls.invert_look   = controls->value("invert_look", profile.controls.invert_look);
            profile.controls.sensitivity   = controls->value("sensitivity", profile.controls.sensitivity);
            profile.controls.vibration     = controls->value("vibration", profile.controls.vibration);
            profile.controls.invert_flight = controls->value("invert_flight", profile.controls.invert_flight);
            profile.controls.auto_center   = controls->value("auto_center", profile.controls.auto_center);
        }
        loaded.push_back(std::move(profile));
    }
    if(!loaded.empty())
        profiles = std::move(loaded);
}

void PlayerProfile::save() const
{
    nlohmann::json doc{{"profiles", nlohmann::json::array()}};
    for(auto const& profile : profiles)
        doc["profiles"].push_back({
            {"name", to_utf8(profile.name)},
            {"color", profile.color},
            {"controls",
             {
                 {"invert_look", profile.controls.invert_look},
                 {"sensitivity", profile.controls.sensitivity},
                 {"vibration", profile.controls.vibration},
                 {"invert_flight", profile.controls.invert_flight},
                 {"auto_center", profile.controls.auto_center},
             }},
        });
    auto       text = doc.dump(2);
    auto       file = "profiles.json"_config;
    file.data_ro    = semantic::SpanOver(text);
    if(!Coffee::FileCommit(file))
        Coffee::Logging::cWarning("Failed to save profiles to {}", file.resource());
}

void alloc_profile_provider(compo::EntityContainer& e)
{
    using controls_t = PlayerProfile::controls_t;
    using func_t     = blam::ui_element::function_t;
    using input_t    = UIDataSource::data_function_t;
    using u16        = libc_types::u16;

    auto& profile = e.register_subsystem_inplace<PlayerProfile>();
    auto& data    = e.subsystem_cast<UIDataSource>();
    profile.load();

    /* The control spinners edit the profile being edited */
    auto bind = [&data, &profile](std::string_view widget, u16 controls_t::* field) {
        data.bind_value(
            widget,
            [&profile, field] { return profile.edited().controls.*field; },
            [&profile, field](u16 value) { profile.edited().controls.*field = value; });
    };
    bind("invert_joystick_spinner", &controls_t::invert_look);
    bind("controller_sensitivity_spinner", &controls_t::sensitivity);
    bind("controller_vibration_spinner", &controls_t::vibration);
    bind("flight_controls_spinner", &controls_t::invert_flight);
    bind("auto_center_spinner", &controls_t::auto_center);

    /* Keyboard text; an empty name keeps the keyboard up */
    data.on_function(func_t::player_profile_change_name, [&profile](UIFunctionCall const& call) {
        if(call.text.empty())
            return ui_result_t::failed;
        profile.edited().name = call.text;
        return ui_result_t::ok;
    });
    data.on_function(func_t::mp_profile_change_name, [&profile](UIFunctionCall const& call) {
        if(call.text.empty())
            return ui_result_t::failed;
        profile.game_setting_name = call.text;
        return ui_result_t::ok;
    });

    /* Picking one to edit; with nothing picked, a new one */
    auto edit_selected = [&profile](UIFunctionCall const& call) {
        if(call.selected && *call.selected < profile.profiles.size())
            profile.editing = *call.selected;
        else
        {
            auto const number = std::to_string(profile.profiles.size() + 1);
            profile.profiles.push_back({.name = u"Player " + std::u16string(number.begin(), number.end())});
            profile.editing = static_cast<u16>(profile.profiles.size() - 1);
        }
        return ui_result_t::ok;
    };
    data.on_function(func_t::create_edit_player_profile, edit_selected);
    data.on_function(func_t::player_profile_begin_editing, [&profile](UIFunctionCall const& call) {
        if(call.selected && *call.selected < profile.profiles.size())
            profile.editing = *call.selected;
        return ui_result_t::ok;
    });
    auto saved = [&profile](UIFunctionCall const&) {
        profile.save();
        return ui_result_t::ok;
    };
    for(auto fn : {
            func_t::player_profile_end_editing,
            func_t::player_profile_save_changes,
            func_t::plyr_prf_save_cntl_settings,
            func_t::plyr_prf_save_adv_cntl_set,
        })
        data.on_function(fn, saved);
    data.on_function(func_t::final_del_player_profile, [&profile](UIFunctionCall const&) {
        if(profile.profiles.size() > 1 && profile.editing < profile.profiles.size())
            profile.profiles.erase(profile.profiles.begin() + profile.editing);
        else
            profile.profiles = {PlayerProfile::profile_t{}};
        profile.editing = profile.selected = 0;
        profile.save();
        return ui_result_t::ok;
    });

    /* Profiles as a list, for the manager and the split screen lobby */
    auto profile_list = [&profile] {
        return UIDataSource::list_t{
            .count = [&profile] { return static_cast<u16>(profile.profiles.size()); },
            .get   = [&profile] { return profile.selected; },
            .set   = [&profile](u16 value) {
                profile.selected = std::min<u16>(value, static_cast<u16>(profile.profiles.size() - 1));
            },
            .text = [&profile](std::string_view widget, u16 item) {
                if(item >= profile.profiles.size() || widget.find("desc") != std::string_view::npos)
                    return std::u16string{};
                return profile.profiles[item].name;
            },
            .image = [&profile](u16 item) {
                return item < profile.profiles.size() ? profile.profiles[item].color : u16(0);
            },
        };
    };
    data.bind_list(input_t::player_profile_list_update_3wide, profile_list());
    data.bind_list(input_t::player_profile_small_menu_update, profile_list());

    /* The edited profile's colour */
    data.bind_list(
        input_t::color_picker_update,
        UIDataSource::list_t{
            .count = [] { return static_cast<u16>(PlayerProfile::colors.size()); },
            .get   = [&profile] { return profile.edited().color; },
            .set   = [&profile](u16 value) {
                profile.edited().color = std::min<u16>(value, PlayerProfile::colors.size() - 1);
            },
            .text = [](std::string_view widget, u16 item) {
                if(item >= PlayerProfile::colors.size() || widget.find("desc") != std::string_view::npos)
                    return std::u16string{};
                return std::u16string(PlayerProfile::colors[item].name);
            },
            .image = [](u16 item) { return item; },
        });
    data.on_function(func_t::color_picker_select_color, [&profile](UIFunctionCall const& call) {
        if(call.selected)
            profile.edited().color = std::min<u16>(*call.selected, PlayerProfile::colors.size() - 1);
        profile.save();
        return ui_result_t::ok;
    });

    /* Split screen lobby; the seat of a quadrant's event is its controller */
    auto& lobby = e.register_subsystem_inplace<LocalLobby>();
    data.on_function(func_t::clear_multiplayer_player_joins, [&lobby](UIFunctionCall const&) {
        lobby = {};
        return ui_result_t::ok;
    });
    data.on_function(func_t::join_controller_to_mp_game, [&lobby](UIFunctionCall const& call) {
        if(call.seat >= lobby.joined.size())
            return ui_result_t::failed;
        lobby.joined[call.seat] = true;
        Coffee::Logging::cDebug("UI: controller {} joined", call.seat);
        return ui_result_t::ok;
    });
    data.on_function(
        func_t::plyr_prof_set_for_game_1wide, [&lobby, &profile](UIFunctionCall const& call) {
            if(call.seat >= lobby.joined.size() || !lobby.joined[call.seat])
                return ui_result_t::failed;
            lobby.profile_chosen[call.seat] = true;
            lobby.profile[call.seat] =
                call.selected && *call.selected < profile.profiles.size() ? *call.selected : profile.selected;
            Coffee::Logging::cDebug("UI: controller {} chose profile {}", call.seat, lobby.profile[call.seat]);
            return ui_result_t::ok;
        });

    /* The description's "Current Profile:" line is completed by the engine */
    data.bind_text(
        "profile_edit_extended_desc_text", [&profile](std::u16string_view text) {
            constexpr std::u16string_view label = u"Current Profile:";
            std::u16string                out(text);
            if(auto at = out.find(label); at != std::u16string::npos)
                out.insert(at + label.size(), u" " + profile.edited().name);
            return out;
        });
    data.bind_text("current_profile_name", [&profile](std::u16string_view) { return profile.edited().name; });
}
