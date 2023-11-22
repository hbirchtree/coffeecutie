#include <coffee/comp_app/file_watcher.h>

#include <coffee/core/debug/formatting.h>
#include <fmt_extensions/url_types.h>

namespace comp_app {

constexpr auto needs_await = compile_info::platform::is_emscripten;

void FileWatcher::start_frame(
    compo::ContainerProxy&, const compo::time_point& t)
{
    std::vector<Url> finished;
    for(auto& [file, info] : m_pending)
    {
        auto expired = !info.deadline.has_value()
                       || compo::clock::now() > info.deadline.value();
        auto exists = platform::file::exists(file);
        if(!exists.has_value() && needs_await && !expired)
            continue;
        info.promise.set_value(file);
        finished.push_back(file);
    }
    for(auto const& file : finished)
        m_pending.erase(file);
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
