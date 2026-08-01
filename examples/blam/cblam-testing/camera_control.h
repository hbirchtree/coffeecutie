#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>
#include <peripherals/stl/type_list.h>

#include "components.h"

using CameraControlManifest = compo::SubsystemManifest<
    type_list_t<
        const PlayerInfo,
        PlayerCamera,
        CameraLerp
    >,
    empty_list_t,
    empty_list_t>;

void alloc_camera_control(compo::EntityContainer& e);
