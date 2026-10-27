#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <peripherals/stl/type_list.h>
#include <peripherals/typing/vectors/matrix_functions.h>

#include "game_mode.h"
#include "cinematic.h"
#include "components.h"
#include "data.h"
#include "physics.h"
#include "ui.h"

/* Samples each seat's devices and routes input to its menu, freecam,
 * body or vehicle. Also builds camera matrices. */
using PlayerControllerManifest = compo::SubsystemManifest<
    type_list_t<
        PlayerInput,
        PlayerCamera,
        PlayerInfo,
        NetworkInfo,
        Model,
        const Attachment,
        const PhysicsData,
        const BipedStatus,
        BipedEquipment>,
    type_list_t<
        PhysicsBus,
        UIEventBus,
        RenderingParameters,
        GameEventBus,
        const CinematicState,
        const GameState>,
    type_list_t<
        comp_app::ControllerInput,
        comp_app::MouseInput
    >>;

/*! First-person models at `eye`: models +X forward, +Z up; camera -Z
 *  forward, +Y up */
inline void place_first_person(
    PlayerCamera const& cam, Vecf3 const& eye, Attachment const& attachment, Model& model)
{
    static const Matf3 model_to_view{
        {0, 0, -1},
        {-1, 0, 0},
        {0, 1, 0},
    };
    /* They ride the view as kicked */
    Matf3 const view_to_world = glm::transpose(
        Matf3(glm::mat4_cast(glm::inverse(Quatf(cam.kick.turn)))) * Matf3(cam.rotation));
    Vecf3 const at    = eye + view_to_world * cam.kick.shift;
    Matf3 const world = view_to_world * model_to_view;
    model.position    = at;
    model.rotation    = glm::normalize(Quatf(world));
    model.transform   = glm::translate(Matf4(1), at) * Matf4(world) *
                      glm::translate(Matf4(1), cam.kick.weapon) * attachment.offset;
}

/*! A camera's projection and view, from `eye` and its rotation */
inline void build_camera_matrix(PlayerCamera& cam, Vecf3 const& eye)
{
    using namespace typing::vectors::scene;

    cam.camera.zVals = {100.f, 0.001f};

    /* Fold the vertex→BSP permutation into the rotation so that
     * cam.camera.position can be stored in plain vertex space.
     * Equivalent to: R * T(-bsp_pos) * bsp_basis. */
    static const Matf4 bsp_basis{
        {0, 0, 1, 0},
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 1},
    };

    /* The kick turns and moves the view about the eye, after the look */
    Matf4 const kick = glm::translate(Matf4(1), -cam.kick.shift) *
                       glm::mat4_cast(glm::inverse(Quatf(cam.kick.turn)));
    Matf4 const view_matrix =
        glm::translate(kick * glm::mat4_cast(cam.camera.rotation) * bsp_basis, -eye);

    cam.matrix       = GenPerspective(cam.camera);
    cam.matrix[2][2] = 0.f;
    cam.matrix       = cam.matrix * view_matrix;
    cam.rotation     = glm::mat4_cast(cam.camera.rotation) * bsp_basis;
}

void alloc_player_controller(compo::EntityContainer& e);
