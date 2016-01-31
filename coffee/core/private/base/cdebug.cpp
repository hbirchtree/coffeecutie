#include <base/cdebug.h>

#include <plat/plat_core.h>

namespace Coffee{
namespace DebugFun{

Mutex OutputPrinterDef::PrinterLock;
Mutex DebuggingState::PrinterLock;

}
}
