#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/subsystem.h>

#include <functional>
#include <map>
#include <memory>
#include <type_traits>
#include <vector>

/* What a checkpoint keeps, bound once by the systems that own it, and taken
 * or put back at the end of the frame a WorldSave or WorldRestore was raised,
 * when the container is quiet. Only the authority saves.
 *
 * Entities are the networked objects, matched by net id, and with
 * `players` the players, by index; where players stand and what they hold
 * is the CheckpointSystem's. Going back:
 *  - an object saved and still there gets its values back;
 *  - one saved and gone since gets them once it is spawned again with the
 *    component (or `create` gives it one), if within `pending_frames`;
 *  - one made since is left alone: CheckpointSystem despawns it.
 * Entity ids held in a bound value are carried across by net id with
 * `entity`, and become 0 when what they named is gone.
 *
 * A subsystem's own state, such as the effects' decals, is copied whole
 * with an object binding; whatever else it made since, and that may simply
 * go, is for an `on_restore` to clear */
struct SaveState : compo::SubsystemBase
{
    using type = SaveState;
    using u32  = libc_types::u32;
    using u64  = libc_types::u64;
    using key_t = u64; /*!< A net id, or a player's index above them */

    static constexpr key_t player_key(u32 index)
    {
        return (key_t(1) << 32) | index;
    }

    static constexpr bool is_player(key_t key)
    {
        return (key >> 32) != 0;
    }

    /* Saved entity ids to live ones */
    struct remap_t
    {
        compo::EntityContainer const* container{nullptr};
        std::map<u64, key_t> const*   saved_keys{nullptr}; /* entity at the save, its key */
        std::map<key_t, u64> const*   live{nullptr};       /* key, entity now */

        u64 operator()(u64 id) const;
    };

    struct entity_binding_base
    {
        virtual ~entity_binding_base() = default;
        virtual void take(compo::EntityContainer& e, std::map<u64, key_t> const& entities) = 0;
        /* False while the entity can't take it yet */
        virtual bool put(compo::EntityContainer& e, u64 id, key_t key, remap_t const& remap) = 0;
        virtual void keys(std::vector<key_t>& out) const = 0;
    };

    /* A component, or one member of it */
    template<typename C, typename V>
    struct entity_binding : entity_binding_base
    {
        using value_type = typename C::value_type;
        using access_t   = std::function<V&(value_type&)>;
        using after_t    = std::function<void(compo::EntityContainer&, u64, value_type&)>;

        explicit entity_binding(access_t access)
            : m_access(std::move(access))
        {
        }

        /* A member holding an entity id; whole components only */
        entity_binding& entity(u64 value_type::*member)
        {
            static_assert(std::is_same_v<V, value_type>);
            m_entities.push_back(member);
            return *this;
        }

        /* Run on each entity once its value is back */
        entity_binding& then(after_t after)
        {
            m_after.push_back(std::move(after));
            return *this;
        }

        /* An object spawned again without the component gets one */
        entity_binding& create()
        {
            m_create = true;
            return *this;
        }

        /* Players' too */
        entity_binding& players()
        {
            m_players = true;
            return *this;
        }

        void take(compo::EntityContainer& e, std::map<u64, key_t> const& entities) override
        {
            m_saved.clear();
            for(auto const& [id, key] : entities)
                if(auto* c = e.get<C>(id); c && (m_players || !is_player(key)))
                    m_saved.emplace(key, m_access(*c));
        }

        bool put(compo::EntityContainer& e, u64 id, key_t key, remap_t const& remap) override
        {
            auto it = m_saved.find(key);
            if(it == m_saved.end())
                return true;
            auto* c = e.get<C>(id);
            if(!c && m_create)
            {
                e.add_component<C>(id);
                c = e.get<C>(id);
            }
            if(!c)
                return false;
            m_access(*c) = it->second;
            if constexpr(std::is_same_v<V, value_type>)
                for(auto member : m_entities)
                    (*c).*member = remap((*c).*member);
            for(auto const& after : m_after)
                after(e, id, *c);
            return true;
        }

        void keys(std::vector<key_t>& out) const override
        {
            for(auto const& [key, _] : m_saved)
                out.push_back(key);
        }

      private:
        access_t                         m_access;
        std::vector<u64 value_type::*>   m_entities;
        std::vector<after_t>             m_after;
        bool                             m_create{false};
        bool                             m_players{false};
        std::map<key_t, V>               m_saved;
    };

    struct object_binding_base
    {
        virtual ~object_binding_base() = default;
        virtual void take()            = 0;
        virtual void put()             = 0;
    };

    template<typename O, typename M>
    struct object_binding : object_binding_base
    {
        object_binding(O& owner, M O::*member)
            : m_owner(owner)
            , m_member(member)
        {
        }

        void take() override
        {
            m_saved = m_owner.*m_member;
        }

        void put() override
        {
            m_owner.*m_member = m_saved;
        }

      private:
        O&    m_owner;
        M O::*m_member;
        M     m_saved{};
    };

    /* A whole component; it must be safe to copy */
    template<typename C>
    entity_binding<C, typename C::value_type>& bind(compo::EntityContainer&)
    {
        using T = typename C::value_type;
        static_assert(std::is_copy_assignable_v<T>, "bind members of components that can't be copied");
        auto binding = std::make_unique<entity_binding<C, T>>([](T& c) -> T& { return c; });
        auto& out    = *binding;
        m_entities.push_back(std::move(binding));
        return out;
    }

    /* One member of a component */
    template<typename C, typename M>
    entity_binding<C, M>& bind(compo::EntityContainer&, M C::*member)
    {
        static_assert(std::is_copy_assignable_v<M>);
        static_assert(std::is_same_v<typename C::value_type, C>);
        auto binding = std::make_unique<entity_binding<C, M>>([member](C& c) -> M& { return c.*member; });
        auto& out    = *binding;
        m_entities.push_back(std::move(binding));
        return out;
    }

    /* A member of a subsystem or another long-lived object */
    template<typename O, typename M>
    void bind(O& owner, M O::*member)
    {
        static_assert(std::is_copy_assignable_v<M>);
        m_objects.push_back(std::make_unique<object_binding<O, M>>(owner, member));
    }

    /* State that just goes on a restore */
    void on_restore(std::function<void(compo::EntityContainer&)> reset)
    {
        m_resets.push_back(std::move(reset));
    }

    void take(compo::EntityContainer& e, u32 generation);
    void put(compo::EntityContainer& e, u32 generation);
    void settle(compo::EntityContainer& e);

    /* How many frames a saved object may take to be spawned again */
    static constexpr u32 pending_frames = 300;

  private:
    std::map<key_t, u64> live_entities(compo::EntityContainer& e) const;

    std::vector<std::unique_ptr<entity_binding_base>> m_entities;
    std::vector<std::unique_ptr<object_binding_base>> m_objects;
    std::vector<std::function<void(compo::EntityContainer&)>> m_resets;

    bool               m_saved{false};
    u32                m_generation{0};
    std::map<u64, key_t> m_saved_keys; /*!< Entity at the save, its key */
    /*! Binding and key still to put back, frames left */
    std::map<std::pair<size_t, key_t>, u32> m_pending;
};

/* Registers the save state; before the systems that bind into it */
SaveState& alloc_save_state(compo::EntityContainer& e);
