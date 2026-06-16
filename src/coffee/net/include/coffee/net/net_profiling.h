#pragma once

#include <peripherals/base.h>
#include <peripherals/libc/signals.h>

namespace Coffee {
namespace Net {

/*!
 * \brief Send a Chrome trace over HTTP to address specified
 *  by COFFEE_REPORT_URL
 */
extern void ProfilingExport();

NO_DISCARD extern int RegisterProfilingAtExit();

NO_DISCARD inline auto RegisterProfiling()
{
    if constexpr(!compile_info::platform::custom_exit)
        return libc::signal::scope_exit_handler<ProfilingExport>();
    else
        return RegisterProfilingAtExit();
}

} // namespace Net
} // namespace Coffee
