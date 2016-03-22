#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_WINDOWS

#include "../windowman.h"

namespace Coffee{
namespace WindowManager{

struct WindowsWinManagerApi : WindowControlDef
{
};

}

using WM = WindowManager::WindowsWinManagerApi;

}
#endif