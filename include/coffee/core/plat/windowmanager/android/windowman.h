#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../windowman.h"

namespace Coffee{
namespace WindowManager{

struct AndroidWindowManApi : WindowControlDef
{
};

}

using WM = WindowManager::AndroidWindowManApi;

}
#endif