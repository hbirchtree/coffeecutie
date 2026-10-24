#include "player_spawn.h"

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

void place_local_cameras(compo::EntityContainer& container, Vecf3 const& position, f32 yaw)
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
        /* No collision mesh to stand on: fly */
        info.mode.physics = false;
        input.input_mode  = PlayerInput::input_mode_t::game;
    }
    update_camera_aspect(container);
}

} // namespace world
