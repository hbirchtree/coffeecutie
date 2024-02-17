#pragma once

#include <peripherals/build/application.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/thread_types.h>
#include <peripherals/stl/types.h>

namespace Coffee::Store {

/* Save functionality!
 * All we are doing is dumping the in-memory structures to disk.
 * Quick to load, for damn sure!
 */
struct SaveApi
{
  public:
    using slot_count_t = libc_types::u16;

    virtual ~SaveApi();
    /*!
     * \brief Retrieve the total amount of memory available for storage.
     * Some systems (eg. Steam Cloud) will restrict the amount of storage
     * available. \return The total amount of memory available
     */
    virtual libc_types::u64 availableMemory() = 0;
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
    virtual std::future<size_t> restore(
        semantic::Bytes&& data, slot_count_t slot = 0) = 0;
    /*!
     * \brief Save memory to storage
     * \param data_ptr Source data pointer
     * \param data_size Source data size
     * \param slot If applicable, which data slot
     */
    virtual std::future<size_t> save(
        semantic::Bytes const& data, slot_count_t slot = 0) = 0;
};

struct FilesystemApi : SaveApi
{
    using AppData = platform::info::AppData;

    virtual ~FilesystemApi();

    libc_types::u64 availableMemory();
    slot_count_t    availableSlots();

    std::future<size_t> restore(semantic::Bytes&& data, slot_count_t slot = 0);
    std::future<size_t> save(
        semantic::Bytes const& data, slot_count_t slot = 0);

    AppData const& m_app;

    FilesystemApi();
    FilesystemApi(AppData const& app);

    friend std::shared_ptr<SaveApi> CreateDefaultSave();
    friend std::shared_ptr<SaveApi> CreateDefaultSave(AppData const&);
};

FORCEDINLINE std::shared_ptr<SaveApi> CreateDefaultSave()
{
    return std::make_shared<FilesystemApi>();
}

FORCEDINLINE std::shared_ptr<SaveApi> CreateDefaultSave(
    platform::info::AppData const& app)
{
    return std::make_shared<FilesystemApi>(app);
}

} // namespace Coffee::Store
