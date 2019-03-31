#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/chunk.h>
#include <peripherals/build/application.h>

namespace Coffee {
namespace Store {

/* Save functionality!
 * All we are doing is dumping the in-memory structures to disk.
 * Quick to load, for damn sure!
 */
struct SaveApi
{
  public:
    using slot_count_t = u16;

    virtual ~SaveApi();
    /*!
     * \brief Retrieve the total amount of memory available for storage.
     * Some systems (eg. Steam Cloud) will restrict the amount of storage
     * available. \return The total amount of memory available
     */
    virtual u64 availableMemory() = 0;
    /*!
     * \brief Available save slots in the system, if applicable
     * \return The total amount of save slots available
     */
    virtual slot_count_t availableSlots() = 0;

    /*!
     * \brief Restore memory from storage, which may be in the cloud or in
     * Google's thingy. \param data_ptr Target data region (must be allocated,
     * can be null to retrieve size preemptively) \param data_size Target data
     * size \param slot If applicable, which data slot
     */
    virtual szptr restore(Bytes&& data, slot_count_t slot = 0) = 0;
    /*!
     * \brief Save memory to storage
     * \param data_ptr Source data pointer
     * \param data_size Source data size
     * \param slot If applicable, which data slot
     */
    virtual szptr save(Bytes const& data, slot_count_t slot = 0) = 0;
};

struct FilesystemApi : SaveApi
{
    using AppData = platform::info::AppData;

    virtual ~FilesystemApi();

    u64          availableMemory();
    slot_count_t availableSlots();
    szptr        restore(Bytes&& data, slot_count_t slot = 0);
    szptr        save(Bytes const& data, slot_count_t slot = 0);

    AppData const& m_app;

    FilesystemApi();
    FilesystemApi(AppData const& app);

    friend ShPtr<SaveApi> CreateDefaultSave();
    friend ShPtr<SaveApi> CreateDefaultSave(AppData const&);
};

FORCEDINLINE ShPtr<SaveApi> CreateDefaultSave()
{
    return MkShared<FilesystemApi>();
}

FORCEDINLINE ShPtr<SaveApi> CreateDefaultSave(
    platform::info::AppData const& app)
{
    return MkShared<FilesystemApi>(app);
}

} // namespace Store
} // namespace Coffee
