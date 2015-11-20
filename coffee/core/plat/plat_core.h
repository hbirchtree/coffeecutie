#ifndef COFFEE_PLAT_CORE
#define COFFEE_PLAT_CORE

#include "platform_detect.h"
#include "coffee/core/coffee_macros.h"
#include "coffee/core/CTypes"

namespace Coffee{

/*!
 * \brief Initializes core dumping for the process, useful for information on crashes
 */
extern void coffee_enable_core_dump();

namespace CResources{
namespace CFiles{

/*!
 * \brief Create directories
 * \param dname The directory name
 * \param createParent Whether it should work recursively
 * \return True on success, false on failure. Failure means that the creation of the directory cannot be guaranteed in any way. Any file operation should cease at that point.
 */
extern bool coffee_file_mkdir(cstring dname, bool createParent = false);

} //CFiles

/*!
 * \brief Acquire file size
 * \param file
 * \return
 */
extern szptr coffee_file_get_size(cstring file);

} //CResources

namespace CMemoryManagement{

/*!
 * \brief Map file to memory for reading (and reading only)
 * \param filename
 * \param offset
 * \param size
 * \param errorCode
 * \return
 */
extern void* coffee_memory_map_file(
        cstring filename,
        szptr offset,
        szptr size,
        int* errorCode);

/*!
 * \brief Unmap a previously mapped file, may use platform-specific code to release the handle behind the scenes.
 * \param ptr
 * \param size
 * \return
 */
extern bool coffee_memory_unmap_file(void* ptr, szptr size);

} //CMemoryManagement

namespace CFunctional{
namespace CDebugHelpers{

/*!
 * \brief Acquire the process callstack
 * \param length
 * \param stackreduce
 * \return
 */
extern cstring_w* coffee_callstack(szptr* length, uint32 stackreduce = 0);

/*!
 * \brief Acquire a clock string for use in debug output
 * \return
 */
extern cstring_w coffee_clock_string();

/*!
 * \brief Free a clock string after use
 * \param arg
 */
extern void coffee_clock_free(cstring_w arg);

} //CDebugHelpers

/*!
 * \brief Millisecond timer for use in the application
 */
class _plat_timer_ll_milli;

/*!
 * \brief Microsecond timer for use in profiling
 */
class _plat_timer_ll_micro;

/*!
 * \brief Microsecond timer suitable for accurate, end-user timing
 */
class _plat_timer_scalar_micro;

} //CFunctional
} //Coffee

//Function and class definitions are given by the platform-specific headers

#if defined(COFFEE_UNIXPLAT)
#include "coffee_unix/unix_core.h"
#elif defined(COFFEE_WINDOWS)
#include "coffee_windows/win_core.h"
#elif defined(__APPLE__) && defined(__MACH__)

#else

#endif

#endif
