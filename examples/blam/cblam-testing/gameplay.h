#pragma once

#include <coffee/components/entity_container.h>

/* Gameplay acting on players' input intents: forge grabbing, getting in
 * and out of vehicles, driving them. Runs after physics, before the
 * graphics loop builds this frame's cameras. */
void alloc_gameplay(compo::EntityContainer& e);
