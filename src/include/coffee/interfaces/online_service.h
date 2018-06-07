#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee{
namespace Online{

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
    virtual u64 identifier() = 0;
    virtual CString displayName() = 0;
    virtual Url image() = 0;
};

class Achievement
{
public:
    virtual u64 identifier()      = 0;
    virtual CString displayName() = 0;
    virtual Url image() = 0;
};

class Friend
{
public:
    virtual ShPtr<User> getUnderlyingUser() = 0;
    virtual FriendStatus state()  = 0;
};

class Party
{
public:
    using UserContainer = void;

    virtual UserContainer getParticipants() = 0;

    virtual u64 userLimit() = 0;
    virtual u64 count() = 0;

    virtual CString secret() = 0;

    virtual bool maySpectate() = 0;
    virtual CString spectateSecret() = 0;
};

class AchievementDelegate
{
public:
    using AchievementContainer = void;

    virtual void notifyAchievement(
        ShPtr<AchievementDesc> const& achievment) = 0;

    virtual AchievementContainer getAchievements() = 0;

    virtual void notify(u64 achievementId) = 0;
};

class FriendDelegate
{
public:
    using PartyContainer = void;
    struct FriendQuery
    {
        FriendStatus state = FriendStatus::Undefined;
        CString name = {};
    };

    virtual FriendContainer getFriends(
        FriendQuery query = {}) = 0;

    virtual ShPtr<Friend> fromUser(ShPtr<User> const&) = 0;

    virtual PartyContainer parties() = 0;
};

/*!
 * Target services:
 * 
 * - Discord Rich Presence
 */
class PresenceService
{
public:
    virtual void identify(CString const&) = 0;
    virtual void setActivity(CString const&) = 0;
    virtual void setDescriptiveActivity(CString const&) = 0;
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
    virtual ~Service() = 0;

    virtual ShPtr<AchievementDelegate> getAchievements() = 0;
    virtual ShPtr<FriendDelegate> getFriends() = 0;

    virtual void registerPresence(UqPtr<PresenceService>&&) = 0;
};

}
}
