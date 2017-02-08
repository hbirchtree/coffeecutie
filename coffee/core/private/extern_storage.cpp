#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/plat/timing/profiling.h>
#include <coffee/core/CDebug>

#ifdef COFFEE_USE_TERMINAL_CTL
#include <coffee/core/plat/memory/cmd_unixterm.h>
#endif

namespace Coffee{
/* Don't mind this, just some string storage */
#ifdef COFFEE_LINUX
thread_local CString Environment::Linux::LinuxSysInfo::cached_cpuinfo_string;
#endif

#if defined(COFFEE_USE_UNWIND)
unw_context_t* Environment::Posix::PosixStacktracer::unwind_context = nullptr;
#endif

Profiler::ProfilerDataStore* Profiler::profiler_data_store = nullptr;
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
thread_local LinkList<CString>* Profiler::context_stack = nullptr;
#endif

#if defined(COFFEE_USE_TERMINAL_CTL)
bool UnixCmd::UnixTerm::alternate_buffer = false;
#endif

#ifndef COFFEE_LOWFAT
Mutex OutputPrinterDef::PrinterLock;
#endif

}
