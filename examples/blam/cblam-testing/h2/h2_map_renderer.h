#pragma once

#include <coffee/components/entity_container.h>

namespace h2 {

/* Registers the Halo 2 map renderer and its MapLoadEvent hook. Must run
 * before setup_load_eventhandlers() so a Halo 2 cache file is consumed here
 * instead of reaching the Halo 1 loader. */
void alloc_h2_renderer(compo::EntityContainer& e);

} // namespace h2
