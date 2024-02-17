#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>
#include <url/url.h>

namespace platform {
namespace online {

using namespace ::libc_types;
using namespace ::stl_types;
using namespace ::platform::url;

enum class FriendStatus
{
    Undefined = 0,
    Online    = 1,
    Offline   = 2,
    NoDisturb = 3,
    Away      = 4,
};

class User
{
  public:
    virtual ~User();
    virtual u64         identifier()  = 0;
    virtual std::string displayName() = 0;
    virtual Url         image()       = 0;
};

class Achievement
{
  public:
    virtual ~Achievement();
    virtual u64         identifier()  = 0;
    virtual std::string displayName() = 0;
    virtual Url         image()       = 0;
};

class Friend
{
  public:
    virtual ~Friend();
    virtual std::shared_ptr<User> getUnderlyingUser() = 0;
    virtual FriendStatus          state()             = 0;
};

class Party
{
  public:
    using UserContainer = void;

    virtual ~Party();

    virtual UserContainer getParticipants() = 0;

    virtual u64 userLimit() = 0;
    virtual u64 count()     = 0;

    virtual std::string secret() = 0;

    virtual bool        maySpectate()    = 0;
    virtual std::string spectateSecret() = 0;
};

struct PartyDesc
{
    std::string partyId;
    i32         curPlayers, maxPlayers;

    struct
    {
        std::string secret;
    } spectate;

    struct
    {
        std::string secret;
    } join;
};

class AchievementDelegate
{
  public:
    using AchievementContainer = void;

    virtual ~AchievementDelegate();

    virtual void notifyAchievement(
        std::shared_ptr<Achievement> const& achievment) = 0;

    virtual AchievementContainer getAchievements() = 0;

    virtual void notify(u64 achievementId) = 0;
};

class FriendDelegate
{
  public:
    using PartyContainer  = void;
    using FriendContainer = std::vector<Friend>;

    struct FriendQuery
    {
        FriendStatus state;
        std::string  name;
    };

    virtual ~FriendDelegate();

    virtual FriendContainer getFriends(FriendQuery query = {}) = 0;

    virtual std::shared_ptr<Friend> fromUser(std::shared_ptr<User> const&) = 0;

    virtual PartyContainer parties() = 0;
};

/*!
 * Target services:
 *
 * - Discord Rich Presence
 */
class PresenceDelegate
{
  public:
    virtual ~PresenceDelegate();
    virtual void put(PartyDesc&& party) = 0;
};

class GameDelegate
{
  public:
    struct Builder
    {
        Builder()
        {
        }

        Builder(
            std::string const& name,
            std::string const& activity,
            Url const&         img)
            : gameName(name)
            , activity(activity)
            , gameImage(img)
        {
        }

        std::string gameName;
        std::string activity;
        Url         gameImage;
    };

    virtual ~GameDelegate();
    virtual void put(Builder&& gameDesc) = 0;
};

/*!
 * Target services:
 *
 * - Google Play Games
 * - iOS Game Center
 * - Steam
 */
class Service
{
  public:
    virtual ~Service();

    virtual std::shared_ptr<GameDelegate>        getGame()         = 0;
    virtual std::shared_ptr<PresenceDelegate>    getPresence()     = 0;
    virtual std::shared_ptr<AchievementDelegate> getAchievements() = 0;
    virtual std::shared_ptr<FriendDelegate>      getFriends()      = 0;

    virtual void poll() = 0;
};

} // namespace online
} // namespace platform
