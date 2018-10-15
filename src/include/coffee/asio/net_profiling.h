#pragma once

namespace Coffee {
namespace Net {

/*!
 * \brief Send a Chrome trace over HTTP to address specified
 *  by COFFEE_REPORT_URL
 */
extern void ProfilingExport();

extern void RegisterProfiling();

} // namespace Net
} // namespace Coffee
