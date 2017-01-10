#pragma once

#include "types/tdef/integertypes.h"

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
extern int64 AvailableSaveMemory();
/*!
 * \brief Available save slots in the system, if applicable
 * \return The total amount of save slots available
 */
extern uint16 AvailableSaveSlots();

/*!
 * \brief Restore memory from storage, which may be in the cloud or in Google's thingy.
 * \param data_ptr Target data region (must be allocated, can be null to retrieve size preemptively)
 * \param data_size Target data size
 * \param slot If applicable, which data slot
 */
extern szptr RestoreMemory(c_ptr data_ptr, szptr* data_size, uint16 slot = 0);
/*!
 * \brief Save memory to storage
 * \param data_ptr Source data pointer
 * \param data_size Source data size
 * \param slot If applicable, which data slot
 */
extern szptr SaveMemory(c_cptr data_ptr, szptr data_size, uint16 slot = 0);

}
}
