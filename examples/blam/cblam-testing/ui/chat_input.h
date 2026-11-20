#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>

#include <optional>
#include <string>

/* What the keyboard seat is typing into chat, for the HUD */
struct ChatState : compo::SubsystemBase
{
    using type = ChatState;

    struct typing_t
    {
        libc_types::u32 seat{0};
        std::string     text;      /*!< UTF-8, written so far */
        std::string     composing; /*!< UTF-8, still in the input method */
    };

    std::optional<typing_t> typing;
};

/* T opens chat for the keyboard's seat in multiplayer, Enter sends it,
 * Escape drops it; typed text, input method compositions included, goes
 * to ChatState and the sent line to the server as a ChatSayEvent */
void alloc_chat_input(compo::EntityContainer& e);
