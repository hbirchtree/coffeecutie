#ifndef COFFEE_CORE_BASE_FILEWATCHER_H
#define COFFEE_CORE_BASE_FILEWATCHER_H

#include <base/cthreading.h>
#include <coffee/core/CTypes>

namespace Coffee{
namespace CResources{

enum class CFileWatchParameters : uint16
{
    None = 0x0,

    FileAccess = 0x1,
    FileModified = 0x2,

    FileCreate = 0x4,
    FileDisappear = 0x8,
    FileAppear = 0x10,
    FileDelete = 0x20,
    FileRename = 0x40,

    FileOpen = 0x80,
    FileClose = 0x100,

    WatchDelete = 0x200,

    All = 0x400,
};

C_FLAGS(CFileWatchParameters,uint16);

struct CFilesystemWatcher;

using CFileWatchCallback = void(*)(CFilesystemWatcher const*,cstring,const void*,CFileWatchParameters const&);

extern bool coffee_file_watch_init();
/*!
 * \brief Creates a thread-local file watch
 * \param fname
 * \param evnotify
 * \return
 */
extern CFilesystemWatcher* coffee_file_watch_add(
        cstring fname, const CFileWatchParameters &evnotify,
        CFileWatchCallback callback, const void *data);
/*!
 * \brief Process thread-local file watch events. Will also free up removed watches.
 * \param numEvents
 */
extern void coffee_file_watch_process_events(size_t numEvents = 10);

extern void coffee_file_watch_set_callback(CFilesystemWatcher* watch, CFileWatchCallback callback);

extern void coffee_file_watch_set_data(CFilesystemWatcher* watch, const void* data);

extern bool coffee_file_watch_remove(CFilesystemWatcher* watch);

extern void coffee_file_watch_deinit();

class CFileWatchTask
{
public:
    CFileWatchTask();
    ~CFileWatchTask();

    uint64 addTask(CString fname,
                 const CFileWatchParameters& notify,
                 CFileWatchCallback callback,
                 const void* data);
    bool removeTask(uint64 const& key);

private:

    struct _cindirect_task
    {
        CString fname;
        CFileWatchParameters mask;
        CFileWatchCallback callback;
        const void* dataPtr;
        uint64 key;
    };

    std::map<uint64,CFilesystemWatcher*> m_watchers;
    uint64 key_index;
    std::vector<_cindirect_task> m_tasks;
    std::future<void> m_watcherHandle;
    std::mutex m_watcherAccess;
    std::atomic_char m_watcherSignal;
    std::vector<uint64> m_delete_queue;
};

}
}

#endif
