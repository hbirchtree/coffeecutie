#pragma once

#include <coffee/components/entity_container.h>

#include <string>

/* The PC join screen's servers, listed by the webrtc-gateway at `gateway`
 * (ws:// or wss://); none without one */
void alloc_server_browser(compo::EntityContainer& e, std::string gateway);
