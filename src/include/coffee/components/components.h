#pragma once

#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <algorithm>

namespace Coffee{
namespace Components{

using clock = Chrono::steady_clock;
using duration = Chrono::microseconds;
using time_point = Chrono::time_point<clock>;

struct EntityContainer;

using LoopFun = void(*)(EntityContainer&);

struct EntityRecipe
{
    Vector<size_t> components;
    LoopFun loop;
    duration interval;
    u32 tags;
};

struct Entity : non_copy
{
    u64 id;
    LoopFun loop;

    duration interval;
    time_point next_run;

    u32 tags;
};

struct ComponentContainerBase : non_copy
{
    virtual ~ComponentContainerBase()
    {
    }

    virtual void registerEntity(u64 id) = 0;
    virtual void unregisterEntity(u64 id) = 0;
};

template<typename ComponentType>
struct ComponentContainer : ComponentContainerBase
{
    virtual ComponentType* get(u64 id) = 0;
};

struct EntityContainer : non_copy
{
    EntityContainer():
        current_entity(0),
        entity_counter(0)
    {
    }

    struct entity_query : Iterator<ForwardIteratorTag, Entity>
    {
        entity_query(EntityContainer& c, u32 tags):
            pred([=](Entity const& e) {
            return (e.tags & tags) == tags;
        }),
        m_container(&c)
        {
            it = std::find_if(
                        m_container->entities.begin(),
                        m_container->entities.end(),
                        pred);
        }

        entity_query(EntityContainer& c):
            m_container(&c),
            it(m_container->entities.end())
        {
        }

        entity_query& operator++()
        {
            if(!pred)
                throw undefined_behavior("bad function");

            it = std::find_if(
                        ++it,
                        m_container->entities.end(),
                        pred);

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
        EntityContainer* m_container;
        Vector<Entity>::iterator it;
    };

    friend struct Entity;

    void exec()
    {
        time_point time_now = clock::now();

        for(auto& e : entities)
            if(e.next_run <= time_now)
            {
                current_entity = e.id;
                e.loop(*this);
                e.next_run = time_now + e.interval;
            }
        current_entity = 0;
    }

    template<typename ComponentType>
    void register_component(ComponentContainer<ComponentType>& c)
    {
        static const type_hash type_id = typeid(ComponentType).hash_code();

        if(components.find(type_id) != components.end())
            throw implementation_error(
                    "cannot register type twice"
                    );

        auto adapted = C_RCAST<void*>(&c);

        components.emplace(type_id, adapted);
    }

    Entity& create_entity(EntityRecipe const& recipe)
    {
        entity_counter++;

        entities.emplace_back();
        auto& entity = entities.back();

        entity.id = entity_counter;
        entity.loop = recipe.loop;
        entity.interval = recipe.interval;
        entity.tags = recipe.tags;
        entity.next_run = clock::now();

        for(auto type_id : recipe.components)
            container(type_id).registerEntity(entity_counter);

        return entity;
    }

    quick_container<entity_query> select(u32 tags)
    {
        return quick_container<entity_query>(
                    [=]()
        {
            return entity_query(*this, tags);
        },
        [=]()
        {
            return entity_query(*this);
        }
                    );
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

    template<typename ComponentType>
    ComponentType& get()
    {
        ComponentType* v = container<ComponentType>().
                get(current_entity);

        if(!v)
            throw undefined_behavior("entity not found in container");

        return *v;
    }

protected:
    /* for accessing components */
    using type_hash = size_t;

    u64 current_entity;

private:
    u64 entity_counter;
    Vector<Entity> entities;

    Map<type_hash, void*> components;
};

template<typename T>
using CNT = ComponentContainer<T>;

}
}
