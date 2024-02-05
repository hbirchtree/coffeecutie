#pragma once

#include <coffee/components/subsystem.h>
#include <map>
#include <url/url.h>

namespace comp_app {

class FileWatcher : public compo::SubsystemBase
{
    using Url = platform::url::Url;

  public:
    using type = FileWatcher;

    void start_frame(compo::ContainerProxy&, const compo::time_point&);

    std::future<Url> await(
        Url const&             url,
        compo::duration const& timeout = std::chrono::seconds(0));

  private:
    struct WatcherData
    {
        std::promise<Url>                promise;
        std::optional<compo::time_point> deadline{};
    };
    std::map<Url, WatcherData> m_pending;
};

} // namespace comp_app
