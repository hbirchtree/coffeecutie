#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/plat/timing/profiling.h>
#include <coffee/core/plat/environment/stacktrace_def.h>
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
unw_context_t* Environment::Linux::LinuxStacktracer::unwind_context = nullptr;
#endif

/* Storage for profiler data */
LinkList<Profiler::DataPoint>* Profiler::datapoints = nullptr;
#ifndef ANDROID_RELEASE_MODE
thread_local LinkList<CString>* Profiling::SimpleProfilerImpl::context_stack = nullptr;
#endif
Mutex* Profiler::data_access_mutex = nullptr;
Timestamp* Profiler::start_time = nullptr;
Profiler::ThreadListing* Profiler::threadnames = nullptr;
std::atomic_int *Profiler::global_init = nullptr;

#ifdef NDEBUG
bool Profiler::Enabled = false;
#else
bool Profiler::Enabled = true;
#endif

#if defined(COFFEE_USE_TERMINAL_CTL)
bool UnixCmd::UnixTerm::alternate_buffer = false;
#endif

Mutex OutputPrinterDef::PrinterLock;
Mutex DebuggingState::PrinterLock;

}
