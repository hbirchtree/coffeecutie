#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <peripherals/stl/type_list.h>

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
    type_list_t<comp_app::ControllerInput>>;

void alloc_player_controller(compo::EntityContainer& e);
