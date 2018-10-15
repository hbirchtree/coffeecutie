#pragma once

#include "../../types/tdef/stltypes.h"

namespace Coffee {
namespace DebugFun {

struct DebugPrinterDef
{
    static CString& Colorize(CString&, Color, Color);
};

} // namespace DebugFun
} // namespace Coffee
