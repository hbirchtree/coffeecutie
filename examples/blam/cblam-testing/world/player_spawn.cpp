#include "player_spawn.h"

#include "../blam_files.h"
#include "../selected_version.h"

void update_camera_aspect(compo::EntityContainer& e);

namespace world {

PlayerInfo* ensure_local_player(compo::EntityContainer& container)
{
    for([[maybe_unused]] auto const& p : container.select<PlayerInfo>())
        return nullptr;
    auto ref = container.create_entity(shared_recipes::player_recipe);
    for(size_t frame = 0; frame < Visibility::type::frame_count_v; frame++)
        container.get_at<Visibility>(ref.id(), frame)->skip_render_for = 0;
    auto& info      = ref.get<PlayerInfo>();
    info.player_idx = 0;
    info.seat_idx   = 0;
    auto& camera    = ref.get<PlayerCamera>();
    camera.keyboard.enabled = true;
    camera.controller.index = 0;
    return &info;
}

void reset_players_between_games(compo::EntityContainer& container)
{
    auto last = std::make_shared<MapGameEvent::game_t>(MapGameEvent::halo1);
    container.subsystem_cast<GameEventBus>().addEventFunction<MapGameEvent>(
        0, [&container, last](GameEvent&, MapGameEvent* map) {
            bool const back_to_halo1 = map->game == MapGameEvent::halo1 && *last != MapGameEvent::halo1;
            *last                    = map->game;
            if(!back_to_halo1)
                return;
            for(auto entity : container.select<PlayerInfo>())
            {
                auto& info = entity.get<PlayerInfo>();
                if(info.is_remote())
                    continue;
                info.mode.physics = false;
                info.biped        = {};
            }
        });
}

void place_local_cameras(
    compo::EntityContainer& container, Vecf3 const& position, f32 yaw, PlayerInfo::biped_shape_t const* unit)
{
    static const glm::mat3 bsp_basis_inv{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}};
    for(auto entity : container.select<PlayerCamera, PlayerInfo, PlayerInput>())
    {
        auto [cam, info, input] = entity.components();
        if(info.is_remote())
            continue;
        cam.controller.opts.sens.move = {.1f, .1f};
        cam.controller.opts.curve     = 200.f;
        cam.camera_opts.accel.alt     = 50.f;
        cam.camera_opts.world_basis   = bsp_basis_inv;
        cam.camera.position           = position;
        cam.camera.rotation           = glm::angleAxis(glm::pi<f32>() - yaw, Vecf3{0.f, 1.f, 0.f});
        if(unit)
            info.biped = *unit;
        info.mode.physics = unit != nullptr;
        input.input_mode  = PlayerInput::input_mode_t::game;
    }
    update_camera_aspect(container);
}

void arm_local_players(
    compo::EntityContainer& container, std::array<BipedEquipment::weapon_t, 2> const& loadout)
{
    u32 const generation = container.subsystem_cast<BlamFiles<halo_version>>().load_generation;
    for(auto entity : container.select<PlayerInfo, BipedEquipment>())
    {
        if(entity.get<PlayerInfo>().is_remote())
            continue;
        auto& equip           = entity.get<BipedEquipment>();
        equip                 = {};
        equip.weapons         = loadout;
        equip.load_generation = generation;
    }
}

} // namespace world
