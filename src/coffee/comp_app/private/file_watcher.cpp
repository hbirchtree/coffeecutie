#include <coffee/comp_app/file_watcher.h>

#include <coffee/core/debug/formatting.h>
#include <fmt_extensions/url_types.h>

namespace comp_app {

void FileWatcher::start_frame(compo::ContainerProxy&, const compo::time_point&)
{
    /* /config is mounted before anything runs (WasmFS on OPFS on web), so a
     * file is there or it is not; polling it only blocked frames behind
     * whatever the OPFS thread was writing */
    for(auto& [file, info] : m_pending)
        info.promise.set_value(file);
    m_pending.clear();
}

std::future<FileWatcher::Url> FileWatcher::await(
    const Url& url, compo::duration const& timeout)
{
    auto it = m_pending.emplace(
        url,
        WatcherData{
            .promise = std::promise<Url>(),
        });
    if(timeout > std::chrono::seconds(0))
        it.first->second.deadline = compo::clock::now() + timeout;
    auto out = it.first->second.promise.get_future();
    return out;
}

} // namespace comp_app
