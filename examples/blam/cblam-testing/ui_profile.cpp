#include "ui_profile.h"

#include "ui_data.h"

#include <coffee/core/CDebug>

void alloc_profile_provider(compo::EntityContainer& e)
{
    using controls_t = PlayerProfile::controls_t;

    auto& profile = e.register_subsystem_inplace<PlayerProfile>();
    auto& data    = e.subsystem_cast<UIDataSource>();

    auto bind = [&data, &profile](
                    std::string_view widget, libc_types::u16 controls_t::* field) {
        data.bind_value(
            widget,
            [&profile, field] { return profile.controls.*field; },
            [&profile, field](libc_types::u16 value) {
                profile.controls.*field = value;
            });
    };
    bind("invert_joystick_spinner", &controls_t::invert_look);
    bind("controller_sensitivity_spinner", &controls_t::sensitivity);
    bind("controller_vibration_spinner", &controls_t::vibration);
    bind("flight_controls_spinner", &controls_t::invert_flight);
    bind("auto_center_spinner", &controls_t::auto_center);

    /* Keyboard text; an empty name keeps the keyboard up */
    auto set_name = [](std::u16string& target) {
        return [&target](UIFunctionCall const& call) {
            if(call.text.empty())
                return ui_result_t::failed;
            target = call.text;
            return ui_result_t::ok;
        };
    };
    constexpr libc_types::u16 mp_profile_change_name     = 41;
    constexpr libc_types::u16 player_profile_change_name = 66;
    data.on_function(player_profile_change_name, set_name(profile.name));
    data.on_function(mp_profile_change_name, set_name(profile.game_setting_name));

    /* Split screen lobby; the seat of a quadrant's event is its controller */
    auto& lobby = e.register_subsystem_inplace<LocalLobby>();
    constexpr libc_types::u16 clear_multiplayer_player_joins = 14;
    constexpr libc_types::u16 join_controller_to_mp_game     = 15;
    constexpr libc_types::u16 mp_profile_set_for_controller  = 37;
    data.on_function(clear_multiplayer_player_joins, [&lobby](UIFunctionCall const&) {
        lobby = {};
        return ui_result_t::ok;
    });
    data.on_function(join_controller_to_mp_game, [&lobby](UIFunctionCall const& call) {
        if(call.seat >= lobby.joined.size())
            return ui_result_t::failed;
        lobby.joined[call.seat] = true;
        Coffee::Logging::cDebug("UI: controller {} joined", call.seat);
        return ui_result_t::ok;
    });
    data.on_function(
        mp_profile_set_for_controller, [&lobby](UIFunctionCall const& call) {
            if(call.seat >= lobby.joined.size() || !lobby.joined[call.seat])
                return ui_result_t::failed;
            lobby.profile_chosen[call.seat] = true;
            Coffee::Logging::cDebug(
                "UI: controller {} chose a profile", call.seat);
            return ui_result_t::ok;
        });

    /* The description's "Current Profile:" line is completed by the engine */
    data.bind_text(
        "profile_edit_extended_desc_text", [&profile](std::u16string_view text) {
            constexpr std::u16string_view label = u"Current Profile:";
            std::u16string                out(text);
            if(auto at = out.find(label); at != std::u16string::npos)
                out.insert(at + label.size(), u" " + profile.name);
            return out;
        });
}
