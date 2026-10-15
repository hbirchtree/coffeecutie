#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <peripherals/stl/type_list.h>
#include <peripherals/typing/vectors/matrix_functions.h>

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
        const PhysicsData>,
    type_list_t<PhysicsBus, UIEventBus, RenderingParameters>,
    type_list_t<
        comp_app::ControllerInput,
        comp_app::MouseInput
    >>;

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

    Matf4 const view_matrix =
        glm::translate(glm::mat4_cast(cam.camera.rotation) * bsp_basis, -eye);

    cam.matrix       = GenPerspective(cam.camera);
    cam.matrix[2][2] = 0.f;
    cam.matrix       = cam.matrix * view_matrix;
    cam.rotation     = glm::mat4_cast(cam.camera.rotation) * bsp_basis;
}

void alloc_player_controller(compo::EntityContainer& e);
