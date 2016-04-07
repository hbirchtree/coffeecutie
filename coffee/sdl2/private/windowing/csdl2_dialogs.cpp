#include <coffee/sdl2/windowing/csdl2_dialogs.h>
#include "../types/sdl2datatypes.h"

namespace Coffee{
namespace Display{

void SDL2Dialog::ErrorMessage(cstring title, cstring message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,title,message,NULL);
}

void SDL2Dialog::WarningMessage(cstring title, cstring message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,title,message,NULL);
}

void SDL2Dialog::InformationMessage(cstring title, cstring message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,title,message,NULL);
}

}
}