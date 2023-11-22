#pragma once

#include <coffee/components/entity_container.h>

#include "data.h"
#include "selected_version.h"

void alloc_ui_system(compo::EntityContainer& e);

void load_ui_items(
    compo::EntityContainer& e, MapChangedEvent<halo_version>& data);
