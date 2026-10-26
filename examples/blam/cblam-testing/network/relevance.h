#pragma once

#include <coffee/components/entity_container.h>

/* On the server, rates how much each networked object and player matters to
 * each remote player, as NetworkInfo::relevance: near or in view is sent
 * every tick, far and behind as rarely as a second. Networking sends by it */
void alloc_network_relevance(compo::EntityContainer& e);
