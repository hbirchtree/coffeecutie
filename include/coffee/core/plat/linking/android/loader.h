#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID

#include "../libraries.h"

namespace Coffee{
namespace Library{

using FunctionLoader = FunctionLoad_def;
using ObjectLoader = ObjectLoader_def<FunctionLoader>;

}
}
#endif