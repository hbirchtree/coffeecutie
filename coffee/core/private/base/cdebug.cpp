#include <coffee/core/base/cdebug.h>

#include <coffee/core/plat/plat_core.h>

namespace Coffee{
namespace DebugFun{

Mutex OutputPrinterDef::PrinterLock;
Mutex DebuggingState::PrinterLock;

}
}
