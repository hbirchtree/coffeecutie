#include "coffee/core/types/input/event_types.h"
#include <coffee/terminal_comp/terminal_components.h>
#include <coffee/core/types/input/keymap.h>
#include <peripherals/libc/output_ops.h>
#include <peripherals/libc/signals.h>

#include <unistd.h>
#include <poll.h>

namespace terminal {

using namespace Coffee::Input;

namespace {
struct TerminalState
{
    struct termios original;
    bool           saved = false;
};

static TerminalState& get_state()
{
    static TerminalState state;
    return state;
}

static void restore_terminal_state()
{
    auto& state = get_state();
    if(state.saved)
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &state.original);
        state.saved = false;
    }
}

static void handle_signal(int sig)
{
    restore_terminal_state();
    ::signal(sig, SIG_DFL);
    ::raise(sig);
}
} // namespace

void TerminalInput::load(entity_container&, comp_app::app_error& ec)
{
    if(!libc::io::terminal::interactive())
    {
        ec = comp_app::AppError::SystemError;
        return;
    }

    auto& state = get_state();

    if(!state.saved)
    {
        if(tcgetattr(STDIN_FILENO, &state.original) < 0)
        {
            ec = comp_app::AppError::SystemError;
            return;
        }
        state.saved = true;
        std::atexit(restore_terminal_state);

        using libc::signal::sig;
        //libc::signal::install(sig::interrupt, handle_signal);
        //libc::signal::install(sig::terminate, handle_signal);
    }

    m_original_termios = state.original;

    struct termios raw = m_original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0)
    {
        ec = comp_app::AppError::SystemError;
        return;
    }
}

void TerminalInput::unload(entity_container&, comp_app::app_error&)
{
    restore_terminal_state();
}

void TerminalInput::start_restricted(proxy_type& p, time_point const&)
{
    auto inputBus = p.service<comp_app::BasicEventBus<CIEvent>>();

    if(!inputBus)
    {
        fmt::print("No input bus!!\n");
        return;
    }

    while(!m_pressed.empty())
    {
        auto [key, mod] = m_pressed.front();
        m_pressed.pop();
        CIKeyEvent rel;
        rel.key = static_cast<decltype(rel.key)>(key);
        rel.mod = static_cast<decltype(rel.mod)>(mod);
        m_register[rel.key] = rel.mod;
        CIEvent relEv;
        relEv.type = CIEvent::Keyboard;
        inputBus->process(relEv, &rel);
    }

    struct pollfd fds;
    fds.fd = STDIN_FILENO;
    fds.events = POLLIN;

    while(poll(&fds, 1, 0) > 0)
    {
        unsigned char c;
        if(read(STDIN_FILENO, &c, 1) != 1)
            break;

        CIKeyEvent ev;
        ev.mod = CIKeyEvent::PressedModifier;
        ev.key = CK_Null;

        if(c == 0x1B) // ESC
        {
            unsigned char seq[2];
            struct pollfd esc_poll;
            esc_poll.fd = STDIN_FILENO;
            esc_poll.events = POLLIN;

            if(poll(&esc_poll, 1, 10) > 0 && read(STDIN_FILENO, &seq[0], 1) == 1)
            {
                if(seq[0] == '[')
                {
                    if(poll(&esc_poll, 1, 10) > 0 && read(STDIN_FILENO, &seq[1], 1) == 1)
                    {
                        switch(seq[1])
                        {
                        case 'A': ev.key = CK_Up; break;
                        case 'B': ev.key = CK_Down; break;
                        case 'C': ev.key = CK_Right; break;
                        case 'D': ev.key = CK_Left; break;
                        default: break;
                        }
                    }
                }
            } else {
                ev.key = CK_Escape;
            }
        } else if(c == 0x7F) {
            ev.key = CK_BackSpace;
        } else if(c == '\r' || c == '\n') {
            ev.key = CK_EnterNL;
        } else if(c == '\t') {
            ev.key = CK_HTab;
        } else if(c >= 0x01 && c <= 0x1A) {
            // Ctrl+A to Ctrl+Z
            ev.key = CK_a + (c - 1);
            ev.mod |= CIKeyEvent::LCtrlModifier;
        } else {
            if(c >= 'A' && c <= 'Z')
            {
                ev.key = CK_A + (c - 'A');
                ev.mod |= CIKeyEvent::LShiftModifier;
            } else {
                ev.key = c;
            }
        }

        if(ev.key != CK_Null)
        {
            fmt::print("Emitting key from TerminalInput: {}", ev.key);
            m_register[ev.key] = ev.mod;
            CIEvent inputEv;
            inputEv.type = CIEvent::Keyboard;
            inputBus->process(inputEv, &ev);

            auto release_mod = static_cast<libc_types::u32>(ev.mod & ~CIKeyEvent::PressedModifier);
            m_pressed.push({static_cast<libc_types::u16>(ev.key), release_mod});
        }
    }
}

}
