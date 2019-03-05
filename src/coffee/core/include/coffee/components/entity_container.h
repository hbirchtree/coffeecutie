#pragma once

#include <coffee/components/types.h>

namespace Coffee {
namespace Components {

struct EntityContainer : non_copy
{
    friend struct Entity;

    EntityContainer() : entity_counter(0)
    {
    }

    struct entity_query : Iterator<ForwardIteratorTag, Entity>
    {
        using entity_predicate = Function<bool(Entity const&)>;

        entity_query(EntityContainer& c, u32 tags) :
            pred([=](Entity const& e) { return (e.tags & tags) == tags; }),
            m_container(&c)
        {
            initialize_iterator();
        }

        entity_query(EntityContainer& c, entity_predicate&& predicate) :
            pred(std::move(predicate)), m_container(&c)
        {
            initialize_iterator();
        }

        template<typename ComponentType>
        STATICINLINE entity_query from_container(
            EntityContainer& c, ComponentContainer<ComponentType> const& comp)
        {
            return entity_query(
                c, [&](Entity const& e) { return comp.contains_entity(e.id); });
        }

        entity_query(EntityContainer& c) :
            m_container(&c), it(m_container->entities.end())
        {
        }

        entity_query& operator++()
        {
            if(!pred)
                Throw(undefined_behavior("bad function"));

            it = std::find_if(++it, m_container->entities.end(), pred);

            return *this;
        }

        bool operator==(entity_query const& other)
        {
            return other.it == it;
        }

        bool operator!=(entity_query const& other)
        {
            return other.it != it;
        }

        Entity& operator*() const
        {
            if(it == m_container->entities.end())
                Throw(std::out_of_range("bad iterator"));

            return *it;
        }

      private:
        void initialize_iterator()
        {
            it = std::find_if(
                m_container->entities.begin(),
                m_container->entities.end(),
                pred);
        }

        entity_predicate const   pred;
        EntityContainer* const   m_container;
        Vector<Entity>::iterator it;
    };

    /*
     *
     * Lifetime events
     *
     */

    void exec();

    void reset()
    {
        entity_counter = 0;

        entities.clear();
        subsystems.clear();
        components.clear();
    }

    /*!
     * \brief Used to hint to the entity container and components
     *  to increase their buffer sizes.
     * This avoids stalling on resizing vectors.
     *
     * \param recipe Type of entity to preallocate, will affect components
     * \param count How much to expand, calling prealloc() multiple
     *               times will not stack.
     */
    void prealloc(EntityRecipe const& recipe, szptr count)
    {
        entities.reserve(entities.size() + count);

        for(auto type_id : recipe.components)
            container(type_id).prealloc(count);
    }

    /*
     *
     * Creation and registering components and entities
     *
     */

    template<typename ComponentType>
    void register_component(ComponentContainer<ComponentType>& c);

    template<typename OutputType>
    void register_subsystem(Subsystem<OutputType>& sys)
    {
        static const type_hash type_id = typeid(OutputType).hash_code();

        if(subsystems.find(type_id) != subsystems.end())
            throw implementation_error("cannot register subsystem twice");

        auto adapted = C_DCAST<SubsystemBase>(&sys);

        if(C_RCAST<void*>(adapted) != C_RCAST<void*>(&sys))
            throw implementation_error("pointer casts will fail");

        subsystems.emplace(type_id, adapted);
    }

    void register_system(UqPtr<EntityVisitorBase>&& visitor)
    {
        visitors.push_back(std::move(visitor));
    }

    inline void add_component(u64 entity_id, size_t type_id)
    {
        container(type_id).register_entity(entity_id);
    }
    template<typename T>
    inline void add_component(u64 entity_id)
    {
        add_component(entity_id, typeid(T).hash_code());
    }

    Entity& create_entity(EntityRecipe const& recipe)
    {
        entity_counter++;

        entities.emplace_back();
        auto& entity = entities.back();

        entity.id       = entity_counter;
        entity.interval = recipe.interval;
        entity.tags     = recipe.tags;
        entity.next_run = clock::now();

        for(auto type_id : recipe.components)
            add_component(entity_counter, type_id);

        return entity;
    }

    /*
     *
     * For querying components or getting data
     *
     */

    quick_container<entity_query> select(u32 tags)
    {
        return quick_container<entity_query>(
            [=]() { return entity_query(*this, tags); },
            [=]() { return entity_query(*this); });
    }

    template<typename ComponentType>
    quick_container<entity_query> select()
    {
        return quick_container<entity_query>(
            [=]() {
                return entity_query::from_container(
                    *this, container<ComponentType>());
            },
            [=]() { return entity_query(*this); });
    }

    ComponentContainerBase& container(size_t type_id)
    {
        auto it = components.find(type_id);

        if(it == components.end())
            throw undefined_behavior("component not found");

        return *it->second;
    }

    template<typename ComponentType>
    ComponentContainer<ComponentType>& container()
    {
        using container_t = ComponentContainer<ComponentType>;

        const type_hash type_id = typeid(ComponentType).hash_code();

        auto it = components.find(type_id);

        if(it == components.end())
            throw undefined_behavior("component not found");

        return *C_FCAST<container_t*>(it->second);
    }

    template<typename OutputType>
    Subsystem<OutputType>& subsystem()
    {
        using subsystem_t = Subsystem<OutputType>;

        const type_hash type_id = typeid(OutputType).hash_code();

        auto it = subsystems.find(type_id);

        if(it == subsystems.end())
            throw undefined_behavior("subsystem not found");

        return *C_FCAST<subsystem_t*>(it->second);
    }

    template<typename ComponentType>
    typename ComponentType::type* get(u64 id)
    {
        return container<ComponentType>().get(id);
    }

    using subsystem_map = Map<type_hash, SubsystemBase*>;
    using component_container_map = Map<type_hash, ComponentContainerBase*>;

  private:
    u64            entity_counter; /*!< For enumerating entities */
    Vector<Entity> entities;

    Map<type_hash, SubsystemBase*>          subsystems; /*!< Not really systems */
    Map<type_hash, ComponentContainerBase*> components;
    Vector<UqPtr<EntityVisitorBase>>        visitors;
};

}
}
