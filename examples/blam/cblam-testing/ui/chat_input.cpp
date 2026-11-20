#include "ui/chat_input.h"

#include "components.h"
#include "game/game_mode.h"
#include "game_events.h"

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/types/input/event_types.h>
#include <utf8cpp/utf8.h>

#include <variant>
#include <vector>

using ChatInputManifest = compo::SubsystemManifest<
    type_list_t<PlayerInput, const PlayerInfo, const PlayerCamera>,
    type_list_t<ChatState, GameEventBus, const GameState>,
    type_list_t<comp_app::KeyboardInput>>;

struct ChatInput : compo::RestrictedSubsystem<ChatInput, ChatInputManifest>
{
    using type  = ChatInput;
    using Proxy = compo::proxy_of<ChatInputManifest>;
    using event_t =
        std::variant<Coffee::Input::CIKeyEvent, Coffee::Input::CIWriteEvent, Coffee::Input::CIWEditEvent>;

    ChatInput()
    {
        priority = 505;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        using namespace Coffee::Input;
        using mode_t = PlayerInput::input_mode_t;

        auto& state    = p.template subsystem<ChatState>();
        auto* keyboard = p.template service<comp_app::KeyboardInput>();
        bool const multiplayer =
            p.template subsystem<GameState>().kind == GameState::kind_t::multiplayer;

        PlayerInput*      input = nullptr;
        PlayerInfo const* info  = nullptr;
        for(auto ent : p.template select<PlayerInput, PlayerInfo, PlayerCamera>())
            if(auto [in, inf, cam] = ent.components(); !inf.is_remote() && cam.keyboard.enabled)
            {
                input = &in;
                info  = &inf;
                break;
            }

        auto const close = [&] {
            state.typing.reset();
            m_typing = false;
            if(input && input->input_mode == mode_t::chat)
                input->input_mode = mode_t::game;
            if(keyboard)
                keyboard->stopWriting();
        };
        /* Something else took the seat's input (a menu, a map change) */
        if(state.typing && (!input || !multiplayer || input->input_mode != mode_t::chat))
            close();

        bool opened = false, send = false;
        for(auto const& event : std::exchange(m_events, {}))
        {
            if(auto const* key = std::get_if<CIKeyEvent>(&event))
            {
                bool const pressed = (key->mod & CIKeyEvent::PressedModifier) != 0;
                bool const repeat  = (key->mod & CIKeyEvent::RepeatedModifier) != 0;
                if(!pressed)
                    continue;
                if(!state.typing)
                {
                    if(key->key == CK_t && !repeat && input && multiplayer &&
                       input->input_mode == mode_t::game)
                    {
                        state.typing      = ChatState::typing_t{.seat = info->seat_idx};
                        m_typing          = true;
                        opened            = true;
                        input->input_mode = mode_t::chat;
                        if(keyboard)
                            keyboard->startWriting();
                    }
                    continue;
                }
                /* Sent once this frame's text is in: keys come before it,
                 * and Enter may be what commits a composition */
                if((key->key == CK_EnterCR || key->key == CK_EnterNL || key->key == CK_KP_Enter) &&
                   !repeat)
                    send = true;
                else if(key->key == CK_Escape && !repeat)
                    close();
                else if(key->key == CK_BackSpace && state.typing->composing.empty())
                    pop_character(state.typing->text);
            } else if(auto const* write = std::get_if<CIWriteEvent>(&event))
            {
                /* The key that opened it may also arrive as text */
                if(state.typing && !opened &&
                   state.typing->text.size() + write->text.size() <= max_bytes)
                    state.typing->text += utf8::replace_invalid(write->text);
                if(state.typing)
                    state.typing->composing.clear();
            } else if(auto const* edit = std::get_if<CIWEditEvent>(&event))
            {
                if(state.typing)
                    state.typing->composing = utf8::replace_invalid(edit->text);
            }
        }
        if(send && state.typing)
        {
            auto text = state.typing->text + state.typing->composing;
            if(!text.empty())
            {
                GameEvent    ev{.type = GameEvent::ChatSay};
                ChatSayEvent say{.player_idx = info->player_idx, .text = std::move(text)};
                p.template subsystem<GameEventBus>().inject(ev, &say);
            }
            close();
        }
        /* The input method's candidates go by the line, as the HUD drew it */
        if(state.typing && keyboard && state.typing->area != m_area)
        {
            m_area           = state.typing->area;
            auto const& area = state.typing->area;
            keyboard->setTextArea({area[0], area[1]}, {area[2], area[3]}, 0);
        } else if(!state.typing)
            m_area = {};
    }

    /* The last whole character */
    static void pop_character(std::string& text)
    {
        if(text.empty())
            return;
        auto end = text.end();
        utf8::prior(end, text.begin());
        text.erase(end, text.end());
    }

    static constexpr size_t max_bytes = 200;

    std::vector<event_t>           m_events;
    std::array<libc_types::i32, 4> m_area{}; /*!< As last given to the input method */
    bool                 m_typing{false}; /*!< keys are chat's, not the game's */
};

void alloc_chat_input(compo::EntityContainer& e)
{
    using namespace Coffee::Input;

    e.register_subsystem_inplace<ChatState>();
    auto& chat = e.register_subsystem_inplace<ChatInput>();
    auto* bus  = e.service<comp_app::BasicEventBus<CIEvent>>();
    if(!bus)
        return;
    /* After ImGui, which keeps keys its own text fields take, and before
     * anything that acts on them in play */
    bus->addEventFunction<CIKeyEvent>(1, [&chat](CIEvent& ev, CIKeyEvent* key) {
        chat.m_events.emplace_back(*key);
        if(chat.m_typing)
            ev.type = CIEvent::NoneType;
    });
    bus->addEventFunction<CIWriteEvent>(1, [&chat](CIEvent&, CIWriteEvent* write) {
        chat.m_events.emplace_back(*write);
    });
    bus->addEventFunction<CIWEditEvent>(1, [&chat](CIEvent&, CIWEditEvent* edit) {
        chat.m_events.emplace_back(*edit);
    });
}
