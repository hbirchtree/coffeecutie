#pragma once

#include "types/tdef/integertypes.h"
#include "types/cdef/memsafe.h"

namespace Coffee{
namespace Store{

/* Save functionality!
 * All we are doing is dumping the in-memory structures to disk.
 * Quick to load, for damn sure!
 */

/*!
 * \brief Retrieve the total amount of memory available for storage.
 * Some systems (eg. Steam Cloud) will restrict the amount of storage available.
 * \return The total amount of memory available
 */
extern i64 AvailableSaveMemory();
/*!
 * \brief Available save slots in the system, if applicable
 * \return The total amount of save slots available
 */
extern u16 AvailableSaveSlots();

/*!
 * \brief Restore memory from storage, which may be in the cloud or in Google's thingy.
 * \param data_ptr Target data region (must be allocated, can be null to retrieve size preemptively)
 * \param data_size Target data size
 * \param slot If applicable, which data slot
 */
extern szptr RestoreMemory(Bytes &&data, u16 slot = 0);
/*!
 * \brief Save memory to storage
 * \param data_ptr Source data pointer
 * \param data_size Source data size
 * \param slot If applicable, which data slot
 */
extern szptr SaveMemory(Bytes const& data, u16 slot = 0);

}
}
