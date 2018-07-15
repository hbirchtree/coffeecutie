#pragma once

#include "../../types/basetypes.h"

namespace Coffee {
namespace DebugFun {

struct DebugPrinterDef
{
    static CString& Colorize(CString&, Color, Color);
};

} // namespace DebugFun
} // namespace Coffee
