#include <coffee/asio/net_resource.h>
#include <coffee/core/internal_state.h>
#include <coffee/discord/discord_binding.h>
#include <discord_register.h>
#include <discord_rpc.h>

#include <coffee/core/CDebug>

#define DISCORD_EP "https://cdn.discordapp.com"

namespace Coffee {
namespace Discord {

static constexpr cstring DiscordAvatarFmt =
    DISCORD_EP "/avatars/{0}/{1}.{2}?size={3}";
static constexpr cstring DiscordDefaultFmt =
    DISCORD_EP "/embed/avatars/{0}.{1}?size={2}";

STATICINLINE void ClearPresence(DiscordRichPresence& p)
{
    p.startTimestamp = p.endTimestamp = 0;

    p.state = p.details = nullptr;

    p.largeImageKey = p.largeImageText = nullptr;
    p.smallImageKey = p.smallImageText = nullptr;

    p.joinSecret = p.spectateSecret = nullptr;

    p.partyId   = nullptr;
    p.partySize = p.partyMax = 0;

    p.matchSecret = nullptr;
    p.instance    = 0;
}

struct DiscordService : Online::Service, State::GlobalState
{
    DiscordRichPresence m_cachedPresence;

    ShPtr<Online::GameDelegate>     game;
    ShPtr<Online::PresenceDelegate> presence;
    ShPtr<DiscordDelegate>          m_delegate;

    DiscordService(ShPtr<DiscordDelegate> delegate);

    void initialize(DiscordOptions const& options);

    virtual ~DiscordService();

    virtual ShPtr<Online::GameDelegate>        getGame();
    virtual ShPtr<Online::PresenceDelegate>    getPresence();
    virtual ShPtr<Online::AchievementDelegate> getAchievements()
    {
        return {};
    }
    virtual ShPtr<Online::FriendDelegate> getFriends()
    {
        return {};
    }

    virtual void poll();

    virtual DiscordDelegate& delegate()
    {
        return *m_delegate;
    }
};

static PlayerInfo InfoFromDiscordUser(DiscordUser const* user)
{
    auto discriminator = cast_string<u32>(user->discriminator);

    Url avatarUrl = Net::MkUrl(
        (strlen(user->avatar) > 0)
            ? fmt(DiscordAvatarFmt, user->userId, user->avatar, "jpg", 128)
            : fmt(DiscordDefaultFmt, discriminator % 5, "png", 128));

    PlayerInfo info;
    info.avatarUrl = avatarUrl;
    info.userTag   = fmt("{0}#{1}", user->username, user->discriminator);
    info.username  = user->username;

    return info;
}

static DiscordService& GetService()
{
    auto ptr =
        C_DCAST<DiscordService>(State::PeekState("discordService").get());

    C_PTR_CHECK(ptr);

    return *ptr;
}

ShPtr<Online::Service> CreateService(
    DiscordOptions&& options, ShPtr<DiscordDelegate> delegate)
{

    auto discordService = MkShared<DiscordService>(delegate);

    State::SwapState("discordService", discordService);

    discordService->initialize(options);

    if(options.steamId.size())
        Discord_RegisterSteamGame(
            options.appId.c_str(), options.steamId.c_str());
    else
    {
        auto executable = Env::ExecutableName();
        Discord_Register(options.appId.c_str(), executable.c_str());
    }

    return discordService;
}

DiscordService::DiscordService(ShPtr<DiscordDelegate> delegate) :
    m_delegate(delegate)
{
}

void DiscordService::initialize(DiscordOptions const& options)
{
    cstring steamId = nullptr;

    if(options.steamId.size())
        steamId = options.steamId.c_str();
    if(!options.appId.size())
        Throw(implementation_error("no application ID provided"));

    DiscordEventHandlers handlers = {};

    handlers.ready = [](DiscordUser const* user) {
        PlayerInfo info = InfoFromDiscordUser(user);

        GetService().delegate().ready(info);
    };
    handlers.disconnected = [](int err, cstring message) {
        discord_error ec;
        ec = err;
        ec = message;

        GetService().delegate().disconnected(ec);
    };
    handlers.errored = [](int err, cstring message) {
        discord_error ec;
        ec = err;
        ec = message;

        GetService().delegate().error(ec);
    };
    handlers.joinGame = [](cstring secret) {
        GetService().delegate().joinGame(secret);
    };
    handlers.spectateGame = [](cstring secret) {
        GetService().delegate().spectate(secret);
    };
    handlers.joinRequest = [](DiscordUser const* request) {
        PlayerInfo info = InfoFromDiscordUser(request);

        GetService().delegate().joinRequest(info);
    };

    m_delegate->joinReply = [&](PlayerInfo const&                  playerInfo,
                                DiscordPresenceDelegate::JoinReply reply) {
        Discord_Respond(playerInfo.userId.c_str(), reply);
    };

    Discord_Initialize(options.appId.c_str(), &handlers, 1, steamId);

    ClearPresence(m_cachedPresence);

    m_cachedPresence.startTimestamp =
        Chrono::seconds(std::time(nullptr)).count();

    presence = MkShared<DiscordPresenceDelegate>(options, &m_cachedPresence);
    game     = MkShared<DiscordGameDelegate>(options, &m_cachedPresence);
}

DiscordService::~DiscordService()
{
    Discord_Shutdown();
}

ShPtr<Online::GameDelegate> DiscordService::getGame()
{
    return game;
}

ShPtr<Online::PresenceDelegate> DiscordService::getPresence()
{
    return presence;
}

void DiscordService::poll()
{
    Discord_UpdateConnection();
    Discord_RunCallbacks();
}

DiscordGameDelegate::DiscordGameDelegate(
    const DiscordOptions& options, DiscordRichPresence* presence) :
    m_presence(presence)
{
}

void DiscordGameDelegate::put(Builder&& gameDesc)
{
    m_gameInfo = std::move(gameDesc);

    m_presence->details       = m_gameInfo.activity.c_str();
    m_presence->largeImageKey = m_gameInfo.gameImage.internUrl.c_str();

    Discord_UpdatePresence(m_presence);
}

void DiscordGameDelegate::put(DiscordGameDelegate::ExtraInfo&& gameInfo)
{
    m_extraInfo = std::move(gameInfo);

    m_presence->smallImageKey  = m_extraInfo.smallImage.internUrl.c_str();
    m_presence->smallImageText = m_extraInfo.smallText.c_str();
    m_presence->largeImageText = m_extraInfo.largeText.c_str();
}

DiscordPresenceDelegate::DiscordPresenceDelegate(
    const DiscordOptions& options, DiscordRichPresence* presence) :
    m_presence(presence)
{
    static_assert(DISCORD_REPLY_NO == ReplyNo, "bad enum");
    static_assert(DISCORD_REPLY_YES == ReplyYes, "bad enum");
    static_assert(DISCORD_REPLY_IGNORE == ReplyIgnore, "bad enum");
}

void DiscordPresenceDelegate::put(Online::PartyDesc&& party)
{
    m_desc = std::move(party);

    if(m_desc.partyId.size())
        m_presence->partyId = m_desc.partyId.c_str();
    m_presence->partySize = C_FCAST<i32>(m_desc.curPlayers);
    m_presence->partyMax  = C_FCAST<i32>(m_desc.maxPlayers);

    if(m_desc.spectate.secret.size())
        m_presence->spectateSecret = m_desc.spectate.secret.c_str();
    if(m_desc.join.secret.size())
        m_presence->joinSecret = m_desc.join.secret.c_str();

    Discord_UpdatePresence(m_presence);
}

void DiscordPresenceDelegate::putState(const CString& state)
{
    extra.state = std::move(state);

    m_presence->state = extra.state.c_str();

    Discord_UpdatePresence(m_presence);
}

void DiscordPresenceDelegate::disable()
{
    Discord_ClearPresence();
}

const char* discord_error_category::name() const noexcept
{
    return "discord_error_code";
}

std::string discord_error_category::message(int error_code) const
{
    return cast_pod(error_code);
}

} // namespace Discord
} // namespace Coffee
