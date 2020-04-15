#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/stl_types.h>
#include <coffee/discord/discord_binding.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>
#include <peripherals/stl/thread_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;

i32 cmain(i32 argc, cstring_w* argv)
{
    using namespace platform::online;

    auto delegate   = MkShared<Discord::DiscordDelegate>();
    delegate->ready = [](Discord::PlayerInfo const& player) {
        cDebug("{0} {1}", player.avatarUrl, player.userTag);
    };
    delegate->error = [](Discord::discord_error const& ec) {
        cDebug("{0}: {1}", ec.message(), ec.error_message);
    };
    delegate->disconnected = [](Discord::discord_error const& ec) {
        cDebug("{0}: {1}", ec.message(), ec.error_message);
    };

    auto service = Discord::CreateService({"468164529617109002"}, delegate);

    service->getGame()->put(
        GameDelegate::Builder("Best Game", "Rickrolling", MkUrl("coffee_cup")));

    PartyDesc desc;
    desc.curPlayers  = 2;
    desc.maxPlayers  = 20;
    desc.partyId     = "1000";
    desc.join.secret = "derp";
    desc.join.secret.resize(129);

    service->getPresence()->put(std::move(desc));

    u32 count = 0;

    while(count < 1000000)
    {
        service->poll();
        CurrentThread::sleep_for(Chrono::milliseconds(100));
        count++;
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(cmain)
