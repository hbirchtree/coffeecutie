#pragma once

#include <peripherals/stl/functional_types.h>
#include <coffee/interfaces/online_service.h>

struct DiscordRichPresence;

namespace Coffee {
namespace Discord {

struct DiscordOptions
{
    DiscordOptions(CString const& appId):
        appId(appId),
        imgSize(0)
    {
    }
    DiscordOptions(CString const& appId, CString const& steamId):
        appId(appId),
        steamId(steamId),
        imgSize(128)
    {
    }
    DiscordOptions(CString const& appId, u32 imSize):
        appId(appId),
        imgSize(imSize)
    {
    }
    DiscordOptions(CString const& appId, CString const& steamId, u32 imgSize):
        appId(appId),
        steamId(steamId),
        imgSize(imgSize)
    {
    }

    CString appId;
    CString steamId;
    u32 imgSize;
};

struct PlayerInfo : non_copy
{
    CString userTag;
    CString username;
    CString userId;
    Url     avatarUrl;
};

struct discord_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int error_code) const;
};

using discord_error = domain_error_code<int, discord_error_category>;

struct DiscordGameDelegate : Online::GameDelegate, non_copy
{
    struct ExtraInfo
    {
        Url     smallImage;
        CString smallText;
        CString largeText;
    };

    DiscordRichPresence* m_presence;
    Builder              m_gameInfo;
    ExtraInfo            m_extraInfo;

    DiscordGameDelegate(
        DiscordOptions const& options, DiscordRichPresence* presence);

    virtual void put(Builder&& gameDesc);

    void put(ExtraInfo&& gameInfo);
};

struct DiscordPresenceDelegate : Online::PresenceDelegate, non_copy
{
    DiscordRichPresence* m_presence;
    Online::PartyDesc    m_desc;

    struct ExtraInfo
    {
        CString state;
    } extra;

    enum JoinReply
    {
        ReplyNo     = 0,
        ReplyYes    = 1,
        ReplyIgnore = 2,
    };

    DiscordPresenceDelegate(
        DiscordOptions const& options, DiscordRichPresence* presence);

    virtual ~DiscordPresenceDelegate()
    {
    }

    virtual void put(Online::PartyDesc&& party);

    virtual void putState(CString const& state);

    void disable();
};

struct DiscordDelegate : non_copy
{
    using JoinReply = DiscordPresenceDelegate::JoinReply;

    Function<void(PlayerInfo const&)>       ready;
    Function<void(discord_error const& ec)> disconnected;
    Function<void(discord_error const& ec)> error;

    Function<void(PlayerInfo const&)>            joinRequest;
    Function<void(PlayerInfo const&, JoinReply)> joinReply;

    Function<void(CString const&)> spectate;
    Function<void(CString const&)> joinGame;
};

extern ShPtr<Online::Service> CreateService(
    DiscordOptions&& options, ShPtr<DiscordDelegate> delegate);

} // namespace Discord
} // namespace Coffee
