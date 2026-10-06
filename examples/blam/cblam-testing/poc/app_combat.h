#pragma once

#include <coffee/components/entity_container.h>

/* Registers the combat PoC as a BlamGraphics subsystem: a "Combat PoC"
 * ImGui window to host/join a session, seat 0's camera as the local player,
 * and its view drawn through the debug-line pass. Self-contained; drop the
 * call to discard it.
 *
 * POC_COMBAT_AUTOHOST=<bots> hosts as soon as a map is loaded, and
 * POC_COMBAT_AUTOFIRE=<ticks> makes the local player fire straight ahead
 * that often, for runs without a person at the controls. */
void alloc_poc_combat(compo::EntityContainer& e);
