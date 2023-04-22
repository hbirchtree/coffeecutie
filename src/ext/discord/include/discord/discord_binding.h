#pragma once

#include <coffee/core/url.h>
#include <coffee/interfaces/online_service.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>

struct DiscordRichPresence;

namespace discord {

using libc_types::i32;
using libc_types::u32;

struct DiscordOptions
{
    DiscordOptions(std::string const& appId) : appId(appId), imgSize(0)
    {
    }
    DiscordOptions(std::string const& appId, std::string const& steamId) :
        appId(appId), steamId(steamId), imgSize(128)
    {
    }
    DiscordOptions(std::string const& appId, libc_types::u32 imSize) :
        appId(appId), imgSize(imSize)
    {
    }
    DiscordOptions(
        std::string const& appId, std::string const& steamId, u32 imgSize) :
        appId(appId),
        steamId(steamId), imgSize(imgSize)
    {
    }

    std::string appId;
    std::string steamId;
    u32         imgSize;
};

struct PlayerInfo
{
    std::string        userTag;
    std::string        username;
    std::string        userId;
    platform::url::Url avatarUrl;
};

struct DiscordGameDelegate : platform::online::GameDelegate
{
    struct ExtraInfo
    {
        platform::url::Url smallImage;
        std::string        smallText;
        std::string        largeText;
    };

    DiscordRichPresence* m_presence;
    Builder              m_gameInfo;
    ExtraInfo            m_extraInfo;

    DiscordGameDelegate(
        DiscordOptions const& options, DiscordRichPresence* presence);

    virtual void put(Builder&& gameDesc);

    void put(ExtraInfo&& gameInfo);
};

struct DiscordPresenceDelegate : platform::online::PresenceDelegate
{
    DiscordRichPresence*        m_presence;
    platform::online::PartyDesc m_desc;

    struct ExtraInfo
    {
        std::string state;
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

    virtual void put(platform::online::PartyDesc&& party);

    virtual void putState(std::string const& state);

    void disable();
};

struct discord_error
{
    int         error_code;
    std::string message;
};

struct DiscordDelegate
{
    using JoinReply = DiscordPresenceDelegate::JoinReply;

    std::function<void(PlayerInfo&&)>            ready;
    std::function<void(discord_error const& ec)> disconnected;
    std::function<void(discord_error const& ec)> error;

    std::function<void(PlayerInfo&&)>            joinRequest;
    std::function<void(PlayerInfo&&, JoinReply)> joinReply;

    std::function<void(std::string const&)> spectate;
    std::function<void(std::string const&)> joinGame;
};

extern std::shared_ptr<platform::online::Service> CreateService(
    DiscordOptions&& options, std::shared_ptr<DiscordDelegate> delegate);

} // namespace discord
