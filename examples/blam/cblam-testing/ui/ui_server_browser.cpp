#include "ui/ui_server_browser.h"

#include "game_events.h"
#include "ui/ui_data.h"

#include <coffee/core/CDebug>
#include <coffee/core/task_queue/task.h>
#include <nlohmann/json.hpp>
#include <utf8cpp/utf8.h>

#if defined(FEATURE_ENABLE_Net)
#include <coffee/net/net_resource.h>
#endif

namespace {

using Coffee::Logging::cDebug;
using Coffee::Logging::cWarning;
using libc_types::u16;
using libc_types::u32;

struct server_t
{
    std::string id;
    std::string key; /*!< Ed25519 public key, base64 */
    std::string map;
    u32         players{0};
    u32         players_max{0};
    bool        locked{false};
};

std::u16string to_u16(std::string_view text)
{
    std::u16string out;
    utf8::unchecked::utf8to16(text.begin(), text.end(), std::back_inserter(out));
    return out;
}

/* The gateway's websocket address, as the HTTP one its list is served on */
std::optional<std::string> list_url(std::string const& gateway)
{
    for(auto [from, to] : {std::pair{"wss://", "https://"}, std::pair{"ws://", "http://"}})
        if(gateway.starts_with(from))
            return to + gateway.substr(std::string_view(from).size()) + "/server-list";
    if(gateway.starts_with("http://") || gateway.starts_with("https://"))
        return gateway + "/server-list";
    return std::nullopt;
}

std::vector<server_t> parse(std::string_view body)
{
    std::vector<server_t> out;
    auto const json = nlohmann::json::parse(body, nullptr, false);
    if(!json.is_object() || !json.contains("servers") || !json["servers"].is_array())
        return out;
    for(auto const& entry : json["servers"])
    {
        if(!entry.is_object())
            continue;
        server_t server{
            .id     = entry.value("name", std::string()),
            .key    = entry.value("key", std::string()),
            .locked = entry.value("private", false),
        };
        if(server.id.empty())
            continue;
        if(auto meta = entry.find("metadata"); meta != entry.end() && meta->is_object())
        {
            server.map         = meta->value("map", std::string());
            server.players     = meta->value("player_count", 0u);
            server.players_max = meta->value("player_count_max", 0u);
        }
        out.push_back(std::move(server));
    }
    return out;
}

} // namespace

/* The PC join screen's list, from the gateway's /server-list */
struct ServerBrowser : compo::SubsystemBase
{
    using type = ServerBrowser;

    std::string           gateway;
    std::vector<server_t> servers;
    u16                   selected{0};
    bool                  fetching{false};

#if defined(FEATURE_ENABLE_Net)
    rq::runtime_queue*                                         worker{};
    std::function<void(std::shared_ptr<std::vector<server_t>>)> on_fetched;
#endif

    void refresh()
    {
#if defined(FEATURE_ENABLE_Net)
        auto url = list_url(gateway);
        if(!url || fetching || !worker)
            return;
        fetching = true;
        [[maybe_unused]] auto queued = rq::runtime_queue::QueueImmediate(
            worker, rq::detail::duration(), [url = *url, done = on_fetched] {
                auto found = std::make_shared<std::vector<server_t>>();
                net::Resource list(net::create_curl_context(), net::MkUrl(url));
                if(auto error = list.fetch(); !error.has_value() && list.data().has_value())
                {
                    auto const data = list.data().value();
                    *found          = parse(std::string_view(reinterpret_cast<char const*>(data.view.data()), data.view.size()));
                } else
                    cWarning("Server list: no answer from {}", url);
                done(std::move(found));
            });
#endif
    }

    /* Joined through the gateway, pinned to the key it listed */
    void join(compo::EntityContainer& e, u16 row)
    {
        if(row >= servers.size())
            return;
        auto const& server = servers[row];
        std::string remote = gateway + "#" + server.id;
        if(!server.key.empty())
            remote += ";auth=ed25519:" + server.key;
        GameEvent          ev{GameEvent::ServerConnect};
        ServerConnectEvent connect{
            .type   = ServerConnectEvent::Server,
            .remote = std::move(remote),
        };
        e.subsystem_cast<GameEventBus>().inject(ev, &connect);
    }
};

void alloc_server_browser(compo::EntityContainer& e, std::string gateway)
{
    using func_t  = blam::ui_element::function_t;
    using input_t = blam::ui_element::data_function_t;

    auto& browser   = e.register_subsystem_inplace<ServerBrowser>();
    browser.gateway = std::move(gateway);
    while(browser.gateway.ends_with('/'))
        browser.gateway.pop_back();
#if defined(FEATURE_ENABLE_Net)
    if(!browser.gateway.empty())
    {
        browser.worker     = rq::runtime_queue::CreateNewThreadQueue("Server list").value();
        browser.on_fetched = rq::runtime_queue::BindToQueue(
            std::function<void(std::shared_ptr<std::vector<server_t>>)>(
                [&browser](std::shared_ptr<std::vector<server_t>> found) {
                    browser.fetching = false;
                    browser.servers  = std::move(*found);
                    if(browser.selected >= browser.servers.size())
                        browser.selected = 0;
                    cDebug("Server list: {} servers", browser.servers.size());
                }));
    }
#endif

    auto& data = e.subsystem_cast<UIDataSource>();
    data.on_function(func_t::gamespy_screen_init, [&browser](UIFunctionCall const&) {
        browser.refresh();
        return ui_result_t::ok;
    });
    data.on_function(func_t::gamespy_screen_dispose, [](UIFunctionCall const&) { return ui_result_t::ok; });
    /* Sorting by a column isn't done; the gateway's order stands */
    data.on_function(func_t::gamespy_select_header, [](UIFunctionCall const&) { return ui_result_t::ok; });
    data.on_function(func_t::gamespy_select_item, [&e, &browser](UIFunctionCall const& call) {
        if(browser.selected >= browser.servers.size())
            return ui_result_t::failed;
        browser.join(e, call.selected.value_or(browser.selected));
        return ui_result_t::ok;
    });
    /* The button bar's buttons by their string: 0 update, 1 refresh, 3 filters, 4 join */
    data.on_function(func_t::gamespy_select_button, [&e, &browser](UIFunctionCall const& call) {
        switch(call.widget ? call.widget->text_box.string_list_index : -1)
        {
        case 1:
            browser.refresh();
            return ui_result_t::ok;
        case 4:
            if(browser.selected >= browser.servers.size())
                return ui_result_t::failed;
            browser.join(e, browser.selected);
            return ui_result_t::ok;
        default:
            return ui_result_t::ok;
        }
    });

    data.bind_list(
        input_t::gamespy_screen_update,
        UIDataSource::list_t{
            .count = [&browser] { return static_cast<u16>(std::min<size_t>(browser.servers.size(), 0xffff)); },
            .get   = [&browser] { return browser.selected; },
            .set   = [&browser](u16 row) { browser.selected = row; },
            .text  = [&browser](std::string_view widget, u16 row) -> std::u16string {
                if(row >= browser.servers.size())
                    return {};
                auto const& server = browser.servers[row];
                if(widget == "server_item_server_name")
                    return to_u16(server.id);
                if(widget == "server_item_map")
                    return to_u16(server.map);
                if(widget == "server_item_players")
                    return to_u16(fmt::format("{}/{}", server.players, server.players_max));
                if(widget == "server_item_locked")
                    return server.locked ? u"*" : u"";
                return {};
            },
        });
    /* Its filters and error sub-screens, which nothing here opens yet */
    data.bind_shown("filters_screen", [] { return false; });
    data.bind_shown("gamespy_error_fullscreen", [] { return false; });
    /* Internet mode only, unsorted */
    data.bind_shown("header_lan", [] { return false; });
    data.bind_shown("header_sort_arrows", [] { return false; });
    data.bind_text("server_count_label", [&browser](std::u16string_view) {
        return to_u16(fmt::format("{}", browser.servers.size()));
    });
}
