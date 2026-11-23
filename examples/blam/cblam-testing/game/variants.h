#pragma once

#include "ui/ui_game_setup.h"

#include <array>
#include <optional>
#include <string_view>

/* The built-in game variants, in the order of ui.map's
 * ui\default_multiplayer_game_setting_names; their descriptions follow the
 * ten custom ones in ui\shell\strings\game_variant_descriptions. Each is a
 * game type and the rules program globals that make it what it says */
namespace variants {

using game_type_t = GameSetup::game_type_t;

struct option_t
{
    std::string_view name;
    std::string_view value;
};

struct variant_t
{
    std::string_view        key; /*!< --game-variant */
    game_type_t             type;
    std::array<option_t, 3> options{};
    bool                    complete{true}; /*!< false: some of its rules aren't played yet */
};

constexpr libc_types::u16 first_description = 10;

/* Incomplete ones lack: lives, starting loadouts, weapon sets, invisibility,
 * motion tracker and health options, respawn time growth, team play outside
 * capture the flag */
constexpr std::array<variant_t, 26> all = {{
    {"slayer", game_type_t::slayer, {{{"score_limit", "15"}}}},
    {"slayer_pro", game_type_t::slayer, {{{"score_limit", "25"}}}, false},
    {"elimination", game_type_t::slayer, {}, false},
    {"phantoms", game_type_t::slayer, {}, false},
    {"endurance", game_type_t::slayer, {{{"score_limit", "10"}}}, false},
    {"rockets", game_type_t::slayer, {{{"score_limit", "25"}}}, false},
    {"snipers", game_type_t::slayer, {{{"score_limit", "15"}}}, false},
    {"oddball", game_type_t::oddball, {{{"score_limit", "120"}}}},
    {"reverse_tag", game_type_t::oddball, {{{"ball_type", "1"}, {"score_limit", "120"}}}},
    {"accumulate", game_type_t::oddball, {{{"score_limit", "300"}}}, false},
    {"juggernaut", game_type_t::oddball, {{{"ball_type", "2"}, {"kill_limit", "10"}}}, false},
    {"stalker", game_type_t::oddball, {{{"ball_type", "2"}, {"kill_limit", "10"}}}, false},
    {"king", game_type_t::koth, {{{"score_limit", "120"}}}},
    {"king_pro", game_type_t::koth, {{{"score_limit", "120"}}}, false},
    {"crazy_king", game_type_t::koth, {{{"score_limit", "120"}, {"moving_hill", "true"}}}},
    {"race", game_type_t::race, {{{"score_limit", "3"}}}},
    {"rally", game_type_t::race, {}, false},
    {"ctf", game_type_t::ctf, {{{"score_limit", "3"}}}},
    {"invasion", game_type_t::ctf, {{{"single_flag", "true"}, {"score_limit", "3"}}}, false},
    {"iron_ctf", game_type_t::ctf, {{{"score_limit", "3"}}}, false},
    {"ctf_pro", game_type_t::ctf, {{{"score_limit", "3"}, {"flag_at_home", "true"}}}, false},
    {"team_race", game_type_t::race, {{{"score_limit", "3"}}}, false},
    {"team_rally", game_type_t::race, {}, false},
    {"team_ball", game_type_t::oddball, {{{"score_limit", "120"}, {"respawn_delay", "10"}}}, false},
    {"team_king", game_type_t::koth, {{{"score_limit", "120"}, {"respawn_delay", "10"}}}, false},
    {"team_slayer", game_type_t::slayer, {{{"score_limit", "50"}}}, false},
}};

/* Sets the game up as the variant: its type and its rules' globals */
inline void apply(GameSetup& setup, libc_types::u16 index)
{
    if(index >= all.size())
        return;
    auto const& v = all[index];
    setup.variant   = index;
    setup.game_type = v.type;
    setup.variant_options.clear();
    for(auto const& option : v.options)
        if(!option.name.empty())
            setup.variant_options.emplace_back(std::string(option.name), std::string(option.value));
}

inline std::optional<libc_types::u16> find(std::string_view key)
{
    for(libc_types::u16 i = 0; i < all.size(); ++i)
        if(all[i].key == key)
            return i;
    return std::nullopt;
}

} // namespace variants
