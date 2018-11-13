#pragma once

#include "cmd_interface.h"

namespace Coffee{
namespace CmdInterface{

struct DummyTerm : BasicTerm
{
    static void ClearScreen()
    {
    }
    static void AltScreen()
    {
    }
    static void ResetScreen()
    {
    }
    static CSize TerminalSize()
    {
        return CSize();
    }
};

}

using Cmd = CmdInterface::DummyTerm;

}