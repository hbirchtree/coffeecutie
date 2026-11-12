#pragma once

#include <blam/volta/blam_unit.h>
#include <coffee/components/subsystem.h>

#include <algorithm>
#include <utility>
#include <vector>

/*! Which teams stand together: players and humans from the start, others as
 *  ai_allegiance pairs them. A player's blows on a friendly team wear its
 *  patience down; past its limit the team turns on the players until it
 *  has gone a while without being hurt. */
struct AllegianceState : compo::SubsystemBase
{
    using type   = AllegianceState;
    using f32    = libc_types::f32;
    using team_t = blam::scn::unit::team_t;

    /* Strain is in units' worth of body taken; one kill is enough */
    static constexpr f32 strain_limit = 1.f;
    static constexpr f32 strain_decay = .1f;  /* per second */
    static constexpr f32 grudge_time  = 30.f; /* s, since the last blow */

    struct grudge_t
    {
        team_t team;
        f32    strain{0.f};
        f32    left{0.f}; /*!< While above 0, the team fights players */
    };

    std::vector<std::pair<team_t, team_t>> pairs{{team_t::player, team_t::human}};
    std::vector<grudge_t>                  grudges;

    void reset()
    {
        pairs   = {{team_t::player, team_t::human}};
        grudges = {};
    }

    bool allied(team_t a, team_t b) const
    {
        return a == b || std::find(pairs.begin(), pairs.end(), ordered(a, b)) != pairs.end();
    }

    void ally(team_t a, team_t b)
    {
        if(!allied(a, b))
            pairs.push_back(ordered(a, b));
    }

    void separate(team_t a, team_t b)
    {
        std::erase(pairs, ordered(a, b));
    }

    /* A team turned on the players by their blows */
    bool betrayed(team_t team) const
    {
        auto it = std::find_if(
            grudges.begin(), grudges.end(), [team](grudge_t const& g) { return g.team == team; });
        return it != grudges.end() && it->left > 0.f;
    }

    /* Whether the team fights the other: players count as betrayed */
    bool hostile(team_t mine, team_t theirs) const
    {
        return !allied(mine, theirs) || (theirs == team_t::player && betrayed(mine));
    }

    /* A player's blow on a unit of a team, as a fraction of its body */
    void betray(team_t team, f32 fraction)
    {
        if(team == team_t::player || !allied(team, team_t::player) || fraction <= 0.f)
            return;
        auto it = std::find_if(
            grudges.begin(), grudges.end(), [team](grudge_t const& g) { return g.team == team; });
        if(it == grudges.end())
            it = grudges.insert(grudges.end(), grudge_t{.team = team});
        it->strain += fraction;
        if(it->left > 0.f || it->strain >= strain_limit)
            it->left = grudge_time;
    }

    void advance(f32 dt)
    {
        for(auto& g : grudges)
        {
            g.strain = std::max(g.strain - strain_decay * dt, 0.f);
            g.left   = std::max(g.left - dt, 0.f);
        }
        std::erase_if(grudges, [](grudge_t const& g) { return g.strain <= 0.f && g.left <= 0.f; });
    }

  private:
    static std::pair<team_t, team_t> ordered(team_t a, team_t b)
    {
        return a < b ? std::pair(a, b) : std::pair(b, a);
    }
};
