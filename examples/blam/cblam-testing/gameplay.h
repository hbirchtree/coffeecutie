#pragma once

#include <coffee/components/entity_container.h>

/* Gameplay acting on players' input intents: forge grabbing, getting in
 * and out of vehicles, riding along in them. Runs after physics. */
void alloc_gameplay(compo::EntityContainer& e);
