#pragma once

#include <coffee/core/base/files/url.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {
namespace Online {

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
    virtual u64     identifier()  = 0;
    virtual CString displayName() = 0;
    virtual Url     image()       = 0;
};

class Achievement
{
  public:
    virtual ~Achievement();
    virtual u64     identifier()  = 0;
    virtual CString displayName() = 0;
    virtual Url     image()       = 0;
};

class Friend
{
  public:
    virtual ~Friend();
    virtual ShPtr<User>  getUnderlyingUser() = 0;
    virtual FriendStatus state()             = 0;
};

class Party
{
  public:
    using UserContainer = void;

    virtual ~Party();

    virtual UserContainer getParticipants() = 0;

    virtual u64 userLimit() = 0;
    virtual u64 count()     = 0;

    virtual CString secret() = 0;

    virtual bool    maySpectate()    = 0;
    virtual CString spectateSecret() = 0;
};

struct PartyDesc
{
    CString partyId;
    i32     curPlayers, maxPlayers;

    struct
    {
        CString secret;
    } spectate;
    struct
    {
        CString secret;
    } join;
};

class AchievementDelegate
{
  public:
    using AchievementContainer = void;

    virtual ~AchievementDelegate();

    virtual void notifyAchievement(ShPtr<Achievement> const& achievment) = 0;

    virtual AchievementContainer getAchievements() = 0;

    virtual void notify(u64 achievementId) = 0;
};

class FriendDelegate
{
  public:
    using PartyContainer  = void;
    using FriendContainer = Vector<Friend>;
    struct FriendQuery
    {
        FriendStatus state;
        CString      name;
    };

    virtual ~FriendDelegate();

    virtual FriendContainer getFriends(FriendQuery query = {}) = 0;

    virtual ShPtr<Friend> fromUser(ShPtr<User> const&) = 0;

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
        Builder(CString const& name, CString const& activity, Url const& img) :
            gameName(name), activity(activity), gameImage(img)
        {
        }

        CString gameName;
        CString activity;
        Url     gameImage;
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

    virtual ShPtr<GameDelegate>        getGame()         = 0;
    virtual ShPtr<PresenceDelegate>    getPresence()     = 0;
    virtual ShPtr<AchievementDelegate> getAchievements() = 0;
    virtual ShPtr<FriendDelegate>      getFriends()      = 0;

    virtual void poll() = 0;
};

} // namespace Online
} // namespace Coffee
