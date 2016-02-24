#pragma once

#include "../platform_detect.h"

#include "cmd_interface.h"

namespace Coffee{
namespace UnixCmd{

using TermScreen = Env::TermScreen;

struct UnixTerm : CmdInterface::BasicTerm
{
    static bool Interactive()
    {
        //TODO: Find way of detecting interactive session
        return false;
    }

    static void ClearScreen()
    {
        fprintf(DefaultDebugOutputPipe,"\e[1;1H\e[2J");
    }
    static void AltScreen()
    {
        fprintf(DefaultDebugOutputPipe,"\033[?1049h\033[H");
        TermScreen::UsingAlternateBuffer = !TermScreen::UsingAlternateBuffer;
    }
    static void ResetScreen()
    {
        if(!TermScreen::UsingAlternateBuffer)
            return;
        fprintf(DefaultDebugOutputPipe,"\033[?1049l");
        TermScreen::UsingAlternateBuffer = !TermScreen::UsingAlternateBuffer;
    }
    static CSize TerminalSize()
    {
        struct winsize w;
        ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
        return CSize(w.ws_col,w.ws_row);
    }
};

}

using Cmd = UnixCmd::UnixTerm;

}