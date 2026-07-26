#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/identify/system.h>

#if defined(COFFEE_EMSCRIPTEN)
#include <csignal>
#include <emscripten.h>
#else
#include <csignal>
#include <stdlib.h>
#endif

#if defined(COFFEE_LINUX)
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#elif defined(COFFEE_APPLE)
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#endif

/*!
 * \brief Detect whether the process is currently being traced by a debugger.
 *  A software breakpoint (C_BREAK) is only meaningful when a debugger is
 *  attached to catch it. When no debugger is present, raising the trap signal
 *  would terminate the process (default SIGTRAP disposition is core dump), so
 *  the break is skipped entirely and execution continues normally.
 */
FORCEDINLINE bool C_DEBUGGER_ATTACHED()
{
#if defined(COFFEE_LINUX)
    int fd = ::open("/proc/self/status", O_RDONLY);
    if(fd < 0)
        return false;
    char    buf[4096];
    ssize_t num = ::read(fd, buf, sizeof(buf) - 1);
    ::close(fd);
    if(num <= 0)
        return false;
    buf[num]           = '\0';
    const char* tracer = ::strstr(buf, "TracerPid:");
    if(!tracer)
        return false;
    tracer += sizeof("TracerPid:") - 1;
    while(*tracer == ' ' || *tracer == '\t')
        ++tracer;
    /* A non-zero TracerPid means a debugger/tracer is attached. */
    return *tracer != '0';
#elif defined(COFFEE_APPLE)
    int               mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, ::getpid()};
    struct kinfo_proc info;
    size_t            size = sizeof(info);
    info.kp_proc.p_flag    = 0;
    if(::sysctl(mib, 4, &info, &size, nullptr, 0) != 0)
        return false;
    return (info.kp_proc.p_flag & P_TRACED) != 0;
#else
    return false;
#endif
}

FORCEDINLINE void C_BREAK()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    /* Only trap into an attached debugger. Without one this is a no-op, so
     * recoverable Throw()s do not deliver a fatal signal in headless runs. */
    if(C_DEBUGGER_ATTACHED())
        std::raise(SIGTRAP);
#elif defined(COFFEE_EMSCRIPTEN)
    emscripten_debugger();
#endif
}
