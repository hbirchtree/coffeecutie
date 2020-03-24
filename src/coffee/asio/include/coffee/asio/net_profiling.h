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

#if !defined(COFFEE_APPLE)
NO_DISCARD extern int RegisterProfiling();
#else
NO_DISCARD inline auto RegisterProfiling()
{
    return libc::signal::scope_exit_handler<ProfilingExport>();
}
#endif

} // namespace Net
} // namespace Coffee
