#include <discord/discord_binding.h>

#include <discord_register.h>
#include <discord_rpc.h>

#include <coffee/asio/net_resource.h>
#include <coffee/core/CFiles>
#include <coffee/core/internal_state.h>
#include <fmt_extensions/info.h>
#include <platforms/environment.h>

#include <coffee/core/CDebug>

#define DISCORD_EP "https://cdn.discordapp.com"
#define DISCORD_TAG "DiscordRPC: "

namespace discord {

using namespace platform;

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

struct DiscordService
    : online::Service
    , Coffee::State::GlobalState
{
    DiscordRichPresence m_cachedPresence;
    DiscordOptions      m_options;

    std::shared_ptr<online::GameDelegate>     game;
    std::shared_ptr<online::PresenceDelegate> presence;
    std::shared_ptr<DiscordDelegate>          m_delegate;

    DiscordService(std::shared_ptr<DiscordDelegate> delegate);

    void initialize(DiscordOptions const& options);

    virtual ~DiscordService();

    virtual std::shared_ptr<online::GameDelegate>     getGame();
    virtual std::shared_ptr<online::PresenceDelegate> getPresence();

    virtual std::shared_ptr<online::AchievementDelegate> getAchievements()
    {
        return {};
    }

    virtual std::shared_ptr<online::FriendDelegate> getFriends()
    {
        return {};
    }

    virtual void poll();

    virtual DiscordDelegate& delegate()
    {
        return *m_delegate;
    }
};

static PlayerInfo InfoFromDiscordUser(DiscordUser const* user, u32 imgSize)
{
    auto discriminator = stl_types::cast_string<u32>(user->discriminator);

    platform::url::Url avatarUrl = net::MkUrl(
        (strlen(user->avatar) > 0)
            ? fmt::format(
                  DISCORD_EP "/avatars/{0}/{1}.{2}?size={3}",
                  user->userId,
                  user->avatar,
                  "jpg",
                  imgSize)
            : fmt::format(
                  DISCORD_EP "/embed/avatars/{0}.{1}?size={2}",
                  discriminator % 5,
                  "png",
                  imgSize));

    PlayerInfo info;
    info.avatarUrl = avatarUrl;
    info.userTag  = fmt::format("{0}#{1}", user->username, user->discriminator);
    info.username = user->username;
    info.userId   = user->userId;

    return info;
}

static DiscordService& GetService()
{
    auto ptr = C_DCAST<DiscordService>(
        Coffee::State::PeekState("discordService").get());

    C_PTR_CHECK(ptr);

    return *ptr;
}

std::shared_ptr<online::Service> CreateService(
    DiscordOptions&& options, std::shared_ptr<DiscordDelegate> delegate)
{
    auto discordService = std::make_shared<DiscordService>(delegate);

    Coffee::State::SwapState("discordService", discordService);

    discordService->initialize(options);

    if(options.steamId.size())
        Discord_RegisterSteamGame(
            options.appId.c_str(), options.steamId.c_str());
    else
    {
        auto executable = platform::path::executable();
        Discord_Register(
            options.appId.c_str(), C_OCAST<const char*>(executable.value()));
    }

    return discordService;
}

DiscordService::DiscordService(std::shared_ptr<DiscordDelegate> delegate)
    : m_options("")
    , m_delegate(delegate)
{
}

void DiscordService::initialize(DiscordOptions const& options)
{
    std::string steamId;
    m_options = options;

    if(options.steamId.size())
        steamId = options.steamId.c_str();
    if(!options.appId.size())
        Throw(implementation_error("no application ID provided"));

    DiscordEventHandlers handlers = {};

    handlers.ready = [](DiscordUser const* user) {
        PlayerInfo info =
            InfoFromDiscordUser(user, GetService().m_options.imgSize);

        auto& delegate = GetService().delegate();

        if(delegate.ready)
            delegate.ready(std::move(info));
    };
    handlers.disconnected = [](int err, const char* message) {
        Coffee::cVerbose(8, DISCORD_TAG "Disconnecting: {0}", message);
        auto& delegate = GetService().delegate();
        if(delegate.disconnected)
            delegate.disconnected({
                .error_code = err,
                .message    = message,
            });
    };
    handlers.errored = [](int err, const char* message) {
        Coffee::cVerbose(8, DISCORD_TAG "Error occurred: {0}", message);
        auto& delegate = GetService().delegate();
        if(delegate.error)
            delegate.error({
                .error_code = err,
                .message    = message,
            });
    };
    handlers.joinGame = [](const char* secret) {
        auto& delegate = GetService().delegate();

        if(delegate.joinGame)
            delegate.joinGame(secret);
        else
            Coffee::cVerbose(
                8, DISCORD_TAG, "Join game request received, but no handler");
    };
    handlers.spectateGame = [](const char* secret) {
        auto& delegate = GetService().delegate();

        if(delegate.spectate)
            delegate.spectate(secret);
        else
            Coffee::cVerbose(
                8, DISCORD_TAG, "Spectate request received, but no handler");
    };
    handlers.joinRequest = [](DiscordUser const* request) {
        PlayerInfo info =
            InfoFromDiscordUser(request, GetService().m_options.imgSize);

        auto& delegate = GetService().delegate();

        if(delegate.joinRequest)
            GetService().delegate().joinRequest(std::move(info));
        else
            Coffee::cVerbose(
                8, DISCORD_TAG, "Join request received, but no handler");
    };

    m_delegate->joinReply = [&](PlayerInfo&&                       playerInfo,
                                DiscordPresenceDelegate::JoinReply reply) {
        Discord_Respond(playerInfo.userId.c_str(), reply);
    };

    Discord_Initialize(options.appId.c_str(), &handlers, 1, steamId.data());

    ClearPresence(m_cachedPresence);

    m_cachedPresence.startTimestamp =
        std::chrono::seconds(std::time(nullptr)).count();

    presence =
        std::make_shared<DiscordPresenceDelegate>(options, &m_cachedPresence);
    game = std::make_shared<DiscordGameDelegate>(options, &m_cachedPresence);
}

DiscordService::~DiscordService()
{
    Coffee::cVerbose(8, DISCORD_TAG "Unloading Discord");
    Discord_Shutdown();
}

std::shared_ptr<online::GameDelegate> DiscordService::getGame()
{
    return game;
}

std::shared_ptr<online::PresenceDelegate> DiscordService::getPresence()
{
    return presence;
}

void DiscordService::poll()
{
    //    Discord_UpdateConnection();
    Discord_RunCallbacks();
}

DiscordGameDelegate::DiscordGameDelegate(
    const DiscordOptions& /*options*/, DiscordRichPresence* presence)
    : m_presence(presence)
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
    const DiscordOptions& /*options*/, DiscordRichPresence* presence)
    : m_presence(presence)
{
    static_assert(DISCORD_REPLY_NO == ReplyNo, "bad enum");
    static_assert(DISCORD_REPLY_YES == ReplyYes, "bad enum");
    static_assert(DISCORD_REPLY_IGNORE == ReplyIgnore, "bad enum");
}

void DiscordPresenceDelegate::put(online::PartyDesc&& party)
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

void DiscordPresenceDelegate::putState(const std::string& state)
{
    extra.state = std::move(state);

    m_presence->state = extra.state.c_str();

    Discord_UpdatePresence(m_presence);
}

void DiscordPresenceDelegate::disable()
{
    Discord_ClearPresence();
}

} // namespace discord
