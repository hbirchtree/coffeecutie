#pragma once

#include "../platform_detect.h"
#include "../../internal_state.h"

#ifdef COFFEE_UNIXPLAT
#include "cmd_interface.h"

namespace Coffee{
namespace UnixCmd{

struct UnixTerm : CmdInterface::BasicTerm
{
    static bool Interactive()
    {
#if defined(COFFEE_LINUX)
        return isatty(fileno(stdout)) && isatty(fileno(stderr));
#else
        //TODO: Find way of detecting interactive session
        return false;
#endif
    }

    static void ClearScreen()
    {
        fprintf(DefaultDebugOutputPipe,"\033[1;1H\033[2J");
    }
    static void AltScreen()
    {
        fprintf(DefaultDebugOutputPipe,"\033[?1049h\033[H");
        State::GetAlternateTerminal() = !State::GetAlternateTerminal();
    }
    static void ResetScreen()
    {
        if(!State::GetAlternateTerminal())
            return;
        fprintf(DefaultDebugOutputPipe,"\033[?1049l");
        State::GetAlternateTerminal() = !State::GetAlternateTerminal();
    }
    static CSize TerminalSize()
    {
        struct winsize w;
        ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
        return CSize(w.ws_col,w.ws_row);
    }
private:
    static bool alternate_buffer;
};

}

using Cmd = UnixCmd::UnixTerm;

}
#endif
