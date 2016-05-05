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
extern void ExportProfilerData(cstring out, int32 argc = 0, cstring_w* argv = nullptr);

/*!
 * \brief Called on exit to conclude profiling session
 * \param argc
 * \param argv
 * \param silent
 */
extern void ExitRoutine(int32 argc, cstring_w* argv, bool silent = false);

}
}
