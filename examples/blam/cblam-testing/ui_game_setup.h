#pragma once

#include <coffee/components/entity_container.h>

#include <optional>
#include <string>
#include <vector>

/* What the shell UI picked for the next game: indices into the tables in
 * ui_game_setup.cpp, which follow the order of ui.map's strings and
 * pictures */
struct GameSetup : compo::SubsystemBase
{
    using type = GameSetup;

    enum class game_type_t : libc_types::u16
    {
        ctf,
        koth,
        slayer,
        oddball,
        race,
    };

    libc_types::u16 level{0};
    libc_types::u16 difficulty{1}; /* normal */
    libc_types::u16 mp_map{0};
    game_type_t     game_type{game_type_t::slayer};

    GameSetup(compo::EntityContainer& e)
        : m_container(e)
    {
    }

    /* Loads at the start of the next frame, outside the UI's handlers,
     * since a map load removes the menu's own entities */
    void start(std::string map)
    {
        m_pending = std::move(map);
    }

    /* Back to the menu because the server went away; message indexes
     * displayed_error_messages and shows once ui.map is up */
    void leave(std::optional<libc_types::u16> message, std::string detail);

    std::u16string const& error_text() const
    {
        return m_error_text;
    }

    void start_frame(ContainerProxy&, time_point const&) override;

    bool main_thread_only() const override
    {
        return true;
    }

    std::string_view subsystem_name() const override
    {
        return "GameSetup";
    }

  private:
    struct error_t
    {
        libc_types::u16 message;
        std::string     detail;
    };

    compo::EntityContainer&    m_container;
    std::optional<std::string> m_pending;
    std::optional<error_t>     m_error;
    std::u16string             m_error_text;
};

/* Registers GameSetup and the level, map, game type and difficulty hooks;
 * after alloc_profile_provider */
void alloc_game_setup_provider(compo::EntityContainer& e);
