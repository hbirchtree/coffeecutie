#pragma once

#include <coffee/core/CDialogs>

namespace Coffee{
namespace Display{

struct SDL2Dialog : public DialogInterface
{
    static void ErrorMessage(cstring title,cstring message);
    static void WarningMessage(cstring title,cstring message);
    static void InformationMessage(cstring title,cstring message);
};

}
}