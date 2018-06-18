#pragma once

#include "../coffee.h"

namespace Coffee{

struct Url;

namespace Profiling{


/*!
 * \brief Print profiler data to terminal
 */
COFFEE_APP_FUNC void PrintProfilerData();

/*!
 * \brief Export profiler data to a file
 * \param target
 */
COFFEE_APP_FUNC void ExportProfilerData(CString &target);

/*!
 * \brief Export profiler data to Chrome tracing format
 * \param target
 */
COFFEE_APP_FUNC void ExportChromeTracerData(CString& target);

/*!
 * \brief Take a string and put it into a file. That's all
 * \param data
 * \param outfile
 */
COFFEE_APP_FUNC void ExportStringToFile(
        CString const& data, Url const& outfile);

/*!
 * \brief Called on exit to conclude profiling session
 * \param argc
 * \param argv
 * \param silent
 */
COFFEE_APP_FUNC void ExitRoutine();

}
}
