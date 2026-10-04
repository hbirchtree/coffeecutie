#include "offline_maps.h"

#include <peripherals/identify/system.h>

#include <algorithm>
#include <array>
#include <cstdlib>

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten/em_asm.h>
#include <platforms/emscripten/mmio.h>
#endif

namespace offline_maps {

#if defined(COFFEE_EMSCRIPTEN)

void register_storage()
{
    for(auto version : std::array{
            blam::version_t::pc,
            blam::version_t::custom_edition,
            blam::version_t::xbox,
            blam::version_t::mcc,
        })
        platform::file::emscripten::add_offline_prefix(
            std::string(prefix_for(version)) + "/");
}

std::vector<std::string> list(std::string_view directory)
{
    std::string dir(directory);
    if(!dir.ends_with('/'))
        dir.push_back('/');

    /* offline_maps_preload.js fills Module.blamOfflineFiles before main().
     * Module is only populated on the main thread. */
    // clang-format off
    char* joined = reinterpret_cast<char*>(MAIN_THREAD_EM_ASM_PTR({
        var dir = UTF8ToString($0);
        var keys = (Module.blamOfflineFiles || []).filter(function(key) {
            return key.startsWith(dir) &&
                   key.indexOf('/', dir.length) === -1 &&
                   key.indexOf('\n') === -1;
        });
        return stringToNewUTF8(keys.join('\n'));
    }, dir.c_str()));
    // clang-format on

    std::vector<std::string> out;
    std::string_view         rest(joined);
    while(!rest.empty())
    {
        auto end = std::min(rest.find('\n'), rest.size());
        out.emplace_back(rest.substr(dir.size(), end - dir.size()));
        rest.remove_prefix(std::min(end + 1, rest.size()));
    }
    std::free(joined);
    return out;
}

std::optional<std::string> default_map()
{
    if(prefix.empty())
        return std::nullopt;
    auto files = list(prefix);
    if(std::find(files.begin(), files.end(), "ui.map") == files.end())
        return std::nullopt;
    return std::string(prefix) + "/ui.map";
}

#else

void register_storage()
{
}

std::vector<std::string> list(std::string_view)
{
    return {};
}

std::optional<std::string> default_map()
{
    return std::nullopt;
}

#endif

} // namespace offline_maps
