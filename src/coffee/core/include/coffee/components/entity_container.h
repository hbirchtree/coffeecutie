#pragma once

#include <algorithm>
#include <coffee/components/types.h>

namespace Coffee {
namespace Components {
namespace Convenience {

template<typename T>
FORCEDINLINE typename declmemtype(&std::type_info::hash_code) type_hash_v()
{
    return typeid(T).hash_code();
}

} // namespace Convenience

namespace detail {
struct visitor_path;
}

template<typename ContainerType>
struct EntityRef;

template<typename ContainerType, typename ComponentType>
struct ComponentRef;

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
    void register_component(UqPtr<ComponentContainer<ComponentType>>&& c);

    template<typename ContainerType, typename... Args>
    void register_component_inplace(Args... args)
    {
        register_component<typename ContainerType::tag_type>(
            MkUq<ContainerType>(std::forward<Args>(args)...));
    }

    template<typename OutputType>
    void register_subsystem(UqPtr<Subsystem<OutputType>>&& sys)
    {
        static const type_hash type_id = typeid(OutputType).hash_code();

        if(subsystems.find(type_id) != subsystems.end())
            throw implementation_error("cannot register subsystem twice");

        auto adapted = C_DCAST<SubsystemBase>(sys.get());

        if(C_RCAST<void*>(adapted) != C_RCAST<void*>(sys.get()))
            throw implementation_error("pointer casts will fail");

        subsystems.emplace(type_id, std::move(sys));
    }

    void register_system(UqPtr<EntityVisitorBase>&& visitor)
    {
        visitors.push_back(std::move(visitor));
    }

    template<typename OutputType, typename AllocType, typename... Args>
    AllocType& register_subsystem_inplace(Args... args)
    {
        register_subsystem<OutputType>(
            MkUq<AllocType>(std::forward<Args>(args)...));

        return subsystem_cast<AllocType>();
    }

    template<typename SystemType, typename... Args>
    void register_inplace(Args... args)
    {
        register_system(MkUq<SystemType>(std::forward<Args>(args)...));
    }

    template<typename ServiceType, typename SubsystemType>
    void register_subsystem_services(SubsystemType* subsystem)
    {
        static_assert(
            std::is_same<type_hash, size_t>::value, "Mismatched types");

        auto types = type_list::collect_list<typename ServiceType::services>();

        for(type_hash v : types)
        {
            services.insert({v, subsystem});
        }
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

        entity.id   = entity_counter;
        entity.tags = recipe.tags;

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

    template<typename Matcher>
    quick_container<entity_query> match()
    {
        return quick_container<entity_query>(
            [=]() {
                return entity_query(*this, [=](Entity const& e) {
                    return Matcher::match(*this, e.id);
                });
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

    template<typename ComponentContainerType>
    ComponentContainerType& container_cast()
    {
        using component_type = typename ComponentContainerType::tag_type;

        const type_hash type_id = typeid(component_type).hash_code();

        auto it = components.find(type_id);

        if(it == components.end())
            Throw(undefined_behavior("component not found"));

        auto ptr = C_DCAST<ComponentContainerType>(it->second.get());

        if(ptr)
            return *ptr;
        else
            Throw(undefined_behavior("component not found"));
    }

    template<typename ComponentType>
    ComponentContainer<ComponentType>& container()
    {
        using container_t = ComponentContainer<ComponentType>;

        const type_hash type_id = typeid(ComponentType).hash_code();

        auto it = components.find(type_id);

        if(it == components.end())
            Throw(undefined_behavior("component not found"));

        return *C_FCAST<container_t*>(it->second.get());
    }

    template<typename SubsystemType>
    SubsystemType& subsystem_cast()
    {
        using output_type = typename SubsystemType::tag_type;

        const type_hash type_id = typeid(output_type).hash_code();

        auto it = subsystems.find(type_id);

        if(it == subsystems.end())
            Throw(undefined_behavior("subsystem not found"));

        auto ptr = C_DCAST<SubsystemType>(it->second.get());

        if(ptr)
            return *ptr;
        else
            Throw(undefined_behavior("subsystem not found"));
    }

    template<typename TagType>
    Subsystem<TagType>& subsystem()
    {
        using subsystem_t = Subsystem<TagType>;

        const type_hash type_id = typeid(TagType).hash_code();

        auto it = subsystems.find(type_id);

        if(it == subsystems.end())
            Throw(undefined_behavior("subsystem not found"));

        return *C_FCAST<subsystem_t*>(it->second.get());
    }

    template<typename Service>
    Service* service()
    {
        const type_hash type_id = typeid(Service).hash_code();
        auto            it      = services.find(type_id);

        if(it == services.end())
            return nullptr;

        return C_DCAST<Service>(it->second);
    }

    template<class BaseType>
    int services_with()
    {
        return 0;
    }

    template<typename ComponentType>
    typename ComponentType::type* get(u64 id)
    {
        return container<ComponentType>().get(id);
    }

    EntityRef<EntityContainer> ref(Entity& entity);
    EntityRef<EntityContainer> ref(u64 entity);

    template<typename ComponentTag>
    ComponentRef<EntityContainer, ComponentTag> ref_comp(u64 entity);

    /* For optimizations */
    using visitor_graph = Set<Vector<bool>>;

    visitor_graph create_task_graph();

  private:
    u64            entity_counter; /*!< For enumerating entities */
    Vector<Entity> entities;

    Map<type_hash, UqPtr<ComponentContainerBase>> components;
    Map<type_hash, UqPtr<SubsystemBase>>          subsystems;
    Vector<UqPtr<EntityVisitorBase>>              visitors;
    Map<type_hash, SubsystemBase*>                services;
};

} // namespace Components
} // namespace Coffee
