#pragma once

#include "../coffee.h"

namespace Coffee{
namespace Profiling{

/*!
 * \brief Print profiler data to terminal
 */
extern void PrintProfilerData();

/*!
 * \brief Export profiler data to a file
 * TODO: Allow output to arbitrary buffer
 * \param out
 * \param argc
 * \param argv
 */
extern void ExportProfilerData(CString &target);

extern void ExportStringToFile(CString const& data, cstring outfile);

/*!
 * \brief Called on exit to conclude profiling session
 * \param argc
 * \param argv
 * \param silent
 */
extern void ExitRoutine();

}
}
