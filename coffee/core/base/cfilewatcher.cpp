#include "cfilewatcher.h"

#include "coffee/core/plat/platform_detect.h"
#include "coffee/core/base/cdebug.h"

#if defined(COFFEE_LINUX)

#include <sys/ioctl.h>
#include <sys/inotify.h>
#include <unistd.h>

#endif

namespace Coffee{
namespace CResources{

/*!
 * \brief A wrapper for inotify on Linux and Windows-equivalent.
 */
struct CFilesystemWatcher
{
#if defined(COFFEE_LINUX)
    int fd;
#endif
    CFileWatchCallback callback;
    const void* data;
};

#if defined(COFFEE_LINUX)
int coffee_inotify_instance = -1;
std::map<int,CFilesystemWatcher*> coffee_inotify_watchers;
std::mutex coffee_inotify_access;
#endif

/*
 * These maps will look odd. The API should convey certain and
 *  accurate messages to the user about the state of files.
 * Because of this, the mappings are made this way.
 * IN_MODIFY is not sent in all cases, and IN_CLOSE_WRITE is
 *  more often used. We map these events to FileModify to make it work
 *  in a nicer way to the user. It might be deceptive to those familiar
 *  with the platform, but more intuitive as an abstraction.
 *
**/

constexpr _cbasic_static_map<CFileWatchParameters,int,13> infy_flags = {
    {CFileWatchParameters::FileAppear,IN_MOVED_TO},
    {CFileWatchParameters::FileDisappear,IN_MOVED_FROM},
    {CFileWatchParameters::FileCreate,IN_CREATE},
    {CFileWatchParameters::FileDelete,IN_DELETE},

    {CFileWatchParameters::FileOpen,IN_OPEN},

    {CFileWatchParameters::FileClose,IN_CLOSE},
    {CFileWatchParameters::FileClose,IN_CLOSE_WRITE},
    {CFileWatchParameters::FileClose,IN_CLOSE_NOWRITE},

    {CFileWatchParameters::FileModified,IN_MODIFY},
    {CFileWatchParameters::FileModified,IN_CLOSE_WRITE},

    {CFileWatchParameters::FileAccess,IN_ACCESS},

    {CFileWatchParameters::WatchDelete,IN_DELETE_SELF},

    {CFileWatchParameters::All,IN_ALL_EVENTS},
};
constexpr _cbasic_static_map<int,CFileWatchParameters,13> infy_flags_rev = {
    {IN_MOVED_TO,CFileWatchParameters::FileAppear},
    {IN_MOVED_FROM,CFileWatchParameters::FileDisappear},
    {IN_CREATE,CFileWatchParameters::FileCreate},
    {IN_DELETE,CFileWatchParameters::FileDelete,},


    {IN_OPEN,CFileWatchParameters::FileOpen},

    {IN_CLOSE,CFileWatchParameters::FileClose},
    {IN_CLOSE_WRITE,CFileWatchParameters::FileClose},
    {IN_CLOSE_NOWRITE,CFileWatchParameters::FileClose},

    {IN_MODIFY,CFileWatchParameters::FileModified},
    {IN_CLOSE_WRITE,CFileWatchParameters::FileModified},

    {IN_ACCESS,CFileWatchParameters::FileAccess},

    {IN_DELETE_SELF,CFileWatchParameters::WatchDelete},

    {IN_ALL_EVENTS,CFileWatchParameters::All},
};

inline CFileWatchParameters inotify_translate_flags(int flags)
{
    return coffee_get_flags(flags,infy_flags_rev);
}

int inotify_translate_flags(CFileWatchParameters flags)
{
    return coffee_get_flags(flags,infy_flags);
}

bool coffee_file_watch_init()
{
#if defined(COFFEE_LINUX)
    coffee_inotify_access.lock(); //Lock access
    if(coffee_inotify_instance == -1)
        coffee_inotify_instance = inotify_init();

    if(coffee_inotify_instance == -1)
    {
        cWarning("Failed to create inotify instance!");
        coffee_inotify_access.unlock(); //Unlock access
        return false;
    }
    coffee_inotify_access.unlock(); //Unlock access
    return true;
#endif
    return false;
}

CFilesystemWatcher *coffee_file_watch_add(
        cstring fname, const CFileWatchParameters& evnotify,
        CFileWatchCallback callback, const void* data)
{
#if defined(COFFEE_LINUX)
    if(!coffee_file_watch_init())
        return nullptr;

    coffee_inotify_access.lock(); //Lock access
    int fd = inotify_add_watch(
                coffee_inotify_instance,
                fname,
                inotify_translate_flags(evnotify));

    if(fd == -1)
    {
        cWarning("Failed to create file watch!");
        coffee_inotify_access.unlock(); //Unlock access
        return nullptr;
    }

    CFilesystemWatcher* file = new CFilesystemWatcher;

    file->fd = fd;
    file->callback = callback;
    file->data = data;

    coffee_inotify_watchers[fd] = file;

    coffee_inotify_access.unlock(); //Unlock access
    return file;
#else
    return nullptr;
#endif
}

void coffee_file_watch_process_events(size_t numEvents)
{
    uint32 queue_size = 0;
    ioctl(coffee_inotify_instance,FIONREAD,&queue_size);

    queue_size = CMath::min((size_t)queue_size,numEvents);

    if(queue_size<1)
        return;

    size_t bufsize = queue_size*(sizeof(inotify_event)+16);
    byte_t* buffer = new byte_t[bufsize];

    ssize_t len = read(coffee_inotify_instance,buffer,bufsize);

    coffee_inotify_access.lock(); //Lock access
    inotify_event* ev;
    for(ssize_t i=0;i<len;i+=sizeof(inotify_event)+ev->len)
    {
        ev = (inotify_event*)&buffer[i];
        CFilesystemWatcher* watch = coffee_inotify_watchers[ev->wd];

        if(!watch)
        {
            if(ev->len>0)
                cDebug("New/deleted file: {0}",ev->name);
            continue;
        }else{
            CFileWatchParameters parm =
                    inotify_translate_flags(ev->mask)|
                    ((ev->cookie)
                       ? CFileWatchParameters::FileRename
                       : CFileWatchParameters::None);

            watch->callback(watch,ev->name,watch->data,parm);
        }
    }
    coffee_inotify_access.unlock(); //Unlock access

    delete[] buffer;
}

void coffee_file_watch_set_data(CFilesystemWatcher *watch, const void *data)
{
    coffee_inotify_access.lock(); //Lock access
    watch->data = data;
    coffee_inotify_access.unlock(); //Unlock access
}

void coffee_file_watch_set_callback(CFilesystemWatcher *watch, CFileWatchCallback callback)
{
    coffee_inotify_access.lock(); //Lock access
    watch->callback = callback;
    coffee_inotify_access.unlock(); //Unlock access
}

bool coffee_file_watch_remove(CFilesystemWatcher *watch)
{
    coffee_inotify_access.lock(); //Lock access
    if(inotify_rm_watch(coffee_inotify_instance,watch->fd))
    {
        coffee_inotify_watchers.erase(watch->fd);
        delete watch;
        coffee_inotify_access.unlock(); //Lock access
        return true;
    }
    coffee_inotify_access.unlock(); //Unlock access
    return false;
}

void coffee_file_watch_deinit()
{
    coffee_inotify_access.lock();

    close(coffee_inotify_instance);
    coffee_inotify_instance = -1;

    coffee_inotify_access.unlock();
}

CFileWatchTask::CFileWatchTask():
    key_index(0),
    m_watcherAccess(),
    m_watcherSignal()
{
    cMsg("FW","Starting file watcher thread");
    std::function<void()> watcherFunction = [=]()
    {
        if(!coffee_file_watch_init())
        {
            m_watcherSignal.store(-1);
        }else
            m_watcherSignal.store(1);

        int signal = m_watcherSignal.load();
        while(signal > 0)
        {
            coffee_file_watch_process_events(1);

            signal = m_watcherSignal.load();
        }
        coffee_file_watch_deinit();
    };

    m_watcherHandle = CFunctional::CThreading::runAsync(watcherFunction);
}

CFileWatchTask::~CFileWatchTask()
{
    m_watcherSignal.store(-1);
    cMsg("FW","Waiting for file watcher thread to shut down...");
    m_watcherHandle.get();
}

uint64 CFileWatchTask::addTask(CString fname, const CFileWatchParameters &notify, CFileWatchCallback callback, const void *data)
{
    m_watcherAccess.lock();
    uint64 key = ++key_index;

    m_watchers[key] = coffee_file_watch_add(fname.c_str(),notify,callback,data);

    m_watcherAccess.unlock();
    return key;
}

bool CFileWatchTask::removeTask(const uint64 &key)
{
    m_watcherAccess.lock();

    CFilesystemWatcher* watch = m_watchers[key];
    bool result = watch&&coffee_file_watch_remove(watch);

    m_watcherAccess.unlock();

    return result;
}

}
}
