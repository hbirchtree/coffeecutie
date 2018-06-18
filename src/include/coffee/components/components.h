#pragma once

#include <algorithm>
#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee {
namespace Components {

using clock      = Chrono::high_resolution_clock;
using duration   = Chrono::microseconds;
using time_point = Chrono::time_point<clock>;

struct EntityContainer;

using LoopFun = void (*)(EntityContainer&);

template<typename WrappedType, typename TagType>
struct TaggedTypeWrapper
{
    using type = WrappedType;
    using tag = TagType;
};

template<typename WrappedType, typename TaggedType>
using TagType = TaggedTypeWrapper<WrappedType, TaggedType>;

struct EntityRecipe
{
    Vector<size_t> components;
    LoopFun        loop;
    duration       interval;
    u32            tags;
};

struct Entity : non_copy
{
    u64     id;
    LoopFun loop;

    duration   interval;
    time_point next_run;

    u32 tags;
};

struct ComponentContainerBase : non_copy
{
    virtual ~ComponentContainerBase()
    {
    }

    virtual void register_entity(u64 id)   = 0;
    virtual void unregister_entity(u64 id) = 0;
};

template<typename ComponentType>
struct ComponentContainer : ComponentContainerBase
{
    using type = typename ComponentType::type;

    virtual type* get(u64 id) = 0;

    virtual bool visit(EntityContainer&, Entity& entity)
    {
        return visit(entity);
    }
    virtual bool visit(Entity&)
    {
        return false;
    }
    virtual void process(EntityContainer& c);
};

template<typename OutputType>
struct Subsystem
{
    using type = typename OutputType::type;

    virtual void start_frame()
    {
    }
    virtual void end_frame()
    {
    }

    virtual type const& get() const = 0;
    virtual type& get() = 0;
};

struct EntityContainer : non_copy
{
    EntityContainer() : current_entity(0), entity_counter(0)
    {
    }

    struct entity_query : Iterator<ForwardIteratorTag, Entity>
    {
        entity_query(EntityContainer& c, u32 tags) :
            pred([=](Entity const& e) { return (e.tags & tags) == tags; }),
            m_container(&c)
        {
            it = std::find_if(
                m_container->entities.begin(),
                m_container->entities.end(),
                pred);
        }

        template<typename ComponentType>
        entity_query(
            EntityContainer& c, ComponentContainer<ComponentType> const&) :
            pred([&](Entity const& e) {
                return c.get<ComponentType>(e.id) != nullptr;
            }),
            m_container(&c)
        {
            it = std::find_if(
                m_container->entities.begin(),
                m_container->entities.end(),
                pred);
        }

        entity_query(EntityContainer& c) :
            m_container(&c), it(m_container->entities.end())
        {
        }

        entity_query& operator++()
        {
            if(!pred)
                throw undefined_behavior("bad function");

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

        Entity& operator*()
        {
            if(it == m_container->entities.end())
                throw std::out_of_range("bad iterator");

            return *it;
        }

      private:
        Function<bool(Entity const&)> pred;
        EntityContainer*              m_container;
        Vector<Entity>::iterator      it;
    };

    friend struct Entity;

    void reset()
    {
        current_entity = 0;
        entity_counter = 0;

        entities.clear();
        subsystems.clear();
        components.clear();

        subsystem_updaters.clear();
        component_updaters.clear();
    }

    void exec()
    {
        time_point time_now = clock::now();

        for(auto& subsys : subsystem_updaters)
            subsys.second.first(*this);

        for(auto& e : entities)
            if(e.next_run <= time_now)
            {
                current_entity = e.id;
                e.loop(*this);
                e.next_run = time_now + e.interval;
            }
        current_entity = 0;

        for(auto& subsys : subsystem_updaters)
            subsys.second.second(*this);

        for(auto& component : component_updaters)
            component.second(*this);
    }

    template<typename ComponentType>
    void register_component(ComponentContainer<ComponentType>& c)
    {
        static const type_hash type_id = typeid(ComponentType).hash_code();

        if(components.find(type_id) != components.end())
            throw implementation_error("cannot register type twice");

        auto adapted = C_RCAST<void*>(&c);

        components.emplace(type_id, adapted);

        component_updaters.emplace(type_id, [](EntityContainer& entities) {
            entities.container<ComponentType>().process(entities);
        });
    }

    template<typename OutputType>
    void register_subsystem(Subsystem<OutputType>& sys)
    {
        static const type_hash type_id = typeid(OutputType).hash_code();

        if(subsystems.find(type_id) != subsystems.end())
            throw implementation_error("cannot register subsystem twice");

        auto adapted = C_RCAST<void*>(&sys);

        subsystems.emplace(type_id, adapted);

        subsystem_updaters[type_id] = {
            [](EntityContainer& c) { c.subsystem<OutputType>().start_frame(); },
            [](EntityContainer& c) { c.subsystem<OutputType>().end_frame(); }};
    }

    Entity& create_entity(EntityRecipe const& recipe)
    {
        entity_counter++;

        entities.emplace_back();
        auto& entity = entities.back();

        entity.id       = entity_counter;
        entity.loop     = recipe.loop;
        entity.interval = recipe.interval;
        entity.tags     = recipe.tags;
        entity.next_run = clock::now();

        for(auto type_id : recipe.components)
            container(type_id).register_entity(entity_counter);

        return entity;
    }

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
            [=]() { return entity_query(*this, container<ComponentType>()); },
            [=]() { return entity_query(*this); });
    }

    ComponentContainerBase& container(size_t type_id)
    {
        auto it = components.find(type_id);

        if(it == components.end())
            throw undefined_behavior("component not found");

        return *C_RCAST<ComponentContainerBase*>(it->second);
    }

    template<typename ComponentType>
    ComponentContainer<ComponentType>& container()
    {
        using container_t = ComponentContainer<ComponentType>;

        const type_hash type_id = typeid(ComponentType).hash_code();

        auto it = components.find(type_id);

        if(it == components.end())
            throw undefined_behavior("component not found");

        return *C_RCAST<container_t*>(it->second);
    }

    template<typename OutputType>
    Subsystem<OutputType>& subsystem()
    {
        using subsystem_t = Subsystem<OutputType>;

        const type_hash type_id = typeid(OutputType).hash_code();

        auto it = subsystems.find(type_id);

        if(it == subsystems.end())
            throw undefined_behavior("subsystem not found");

        return *C_RCAST<subsystem_t*>(it->second);
    }

    template<typename ComponentType>
    typename ComponentType::type* get(u64 id)
    {
        return container<ComponentType>().get(id);
    }

    template<typename ComponentType>
    typename ComponentType::type& get()
    {
        typename ComponentType::type* v = get<ComponentType>(current_entity);

        if(!v)
            throw undefined_behavior("entity not found in container");

        return *v;
    }

  protected:
    /* for accessing components */
    using type_hash = size_t;

    u64 current_entity;

  private:
    using SubsystemUpdaters =
        Pair<void (*)(EntityContainer&), void (*)(EntityContainer&)>;

    u64            entity_counter;
    Vector<Entity> entities;

    Map<type_hash, void*>             subsystems;
    Map<type_hash, SubsystemUpdaters> subsystem_updaters;

    Map<type_hash, void*>                      components;
    Map<type_hash, void (*)(EntityContainer&)> component_updaters;
};

template<typename T>
using CNT = ComponentContainer<T>;

template<typename ComponentType>
void ComponentContainer<ComponentType>::process(EntityContainer& c)
{
    auto query = c.select<ComponentType>();

    for(auto& entity : query)
    {
        this->visit(c, entity);
    }
}

} // namespace Components
} // namespace Coffee
