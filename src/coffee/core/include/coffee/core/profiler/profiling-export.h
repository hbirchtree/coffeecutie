#pragma once

#include <coffee/core/coffee.h>

namespace platform {
namespace url {
struct Url;
}
} // namespace platform

namespace Coffee {
namespace Profiling {

/*!
 * \brief Print profiler data to terminal
 */
COFFEE_APP_FUNC void PrintProfilerData();

/*!
 * \brief Export profiler data to a file
 * \param target
 */
COFFEE_APP_FUNC void ExportProfilerData(std::string& target);

/*!
 * \brief Export profiler data to Chrome tracing format
 * \param target
 */
COFFEE_APP_FUNC void ExportChromeTracerData(std::string& target);

/*!
 * \brief Take a string and put it into a file. That's all
 * \param data
 * \param outfile
 */
COFFEE_APP_FUNC void ExportStringToFile(
    std::string const& data, platform::url::Url const& outfile);

/*!
 * \brief Called on exit to conclude profiling session
 * \param argc
 * \param argv
 * \param silent
 */
COFFEE_APP_FUNC void ExitRoutine();

} // namespace Profiling
} // namespace Coffee
