#pragma once

#include <algorithm>
#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/time_types.h>
#include <peripherals/stl/type_list.h>

#define COMPONENTS_ACCESS_TEMPLATES 1

namespace Coffee {
namespace Components {

using clock      = Chrono::high_resolution_clock;
using duration   = Chrono::microseconds;
using time_point = Chrono::time_point<clock>;

struct EntityContainer;
struct ContainerProxy;

template<typename WrappedType, typename TagType>
struct TaggedTypeWrapper
{
    using type = WrappedType;
    using tag  = TagType;
};

template<typename WrappedType, typename TaggedType>
using TagType = TaggedTypeWrapper<WrappedType, TaggedType>;

template<typename... Types>
using TypeList = type_safety::type_list::type_list<Types...>;

struct EntityRecipe
{
    Vector<size_t> components;
    duration       interval;
    u32            tags;
};

struct Entity : non_copy
{
    u64 id;

    duration   interval;
    time_point next_run;

    u32 tags;

    u32 _pad = 0;
};

struct ComponentContainerBase : non_copy
{
    virtual ~ComponentContainerBase()
    {
    }

    virtual void register_entity(u64 id)       = 0;
    virtual void unregister_entity(u64 id)     = 0;
    virtual void prealloc(szptr count)         = 0;
    virtual bool contains_entity(u64 id) const = 0;
};

struct EntityVisitorBase : non_copy
{
    using type_hashes = Vector<size_t>;

    type_hashes const components;
    type_hashes const subsystems;

    u32 tags;

    EntityVisitorBase(
        type_hashes&& components, type_hashes&& subsystems, u32 tags = 0) :
        components(std::move(components)),
        subsystems(std::move(subsystems)),

        tags(tags)
    {
    }

    virtual bool dispatch(
        EntityContainer& container, const time_point& current) = 0;
};

template<typename ComponentType>
struct ComponentContainer : ComponentContainerBase
{
    using type = typename ComponentType::type;

    virtual type* get(u64 id) = 0;
};

struct SubsystemBase : non_copy
{
    virtual void start_frame(ContainerProxy&, time_point const&)
    {
    }
    virtual void end_frame(ContainerProxy&, time_point const&)
    {
    }
};

template<typename OutputType>
struct Subsystem : SubsystemBase
{
    using type = typename OutputType::type;

    virtual type const& get() const = 0;
    virtual type&       get()       = 0;
};

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

        Entity& operator*() const
        {
            if(it == m_container->entities.end())
                throw std::out_of_range("bad iterator");

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

  private:
    /* for accessing components */
    using type_hash = size_t;
    using SubsystemUpdaters =
        Pair<void (*)(EntityContainer&), void (*)(EntityContainer&)>;

    u64            entity_counter;
    Vector<Entity> entities;

    Map<type_hash, SubsystemBase*>          subsystems;
    Map<type_hash, ComponentContainerBase*> components;
    Vector<UqPtr<EntityVisitorBase>>        visitors;
};

struct ContainerProxy : non_copy
{
    friend struct EntityContainer;

    FORCEDINLINE
    quick_container<EntityContainer::entity_query> select(u32 tags)
    {
        return m_container.select(tags);
    }

    template<typename ComponentType>
    FORCEDINLINE quick_container<EntityContainer::entity_query> select()
    {
        return m_container.select<ComponentType>();
    }

    template<typename OutputType>
    FORCEDINLINE Subsystem<OutputType>& subsystem()
    {
        return m_container.subsystem<OutputType>();
    }

    template<typename ComponentType>
    typename ComponentType::type* get(u64 id)
    {
        return m_container.get<ComponentType>(id);
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type& get()
    {
        typename ComponentType::type* v = get<ComponentType>(current_entity);

#if MODE_DEBUG
        if(!v)
            Throw(undefined_behavior("entity not found in container"));
#endif

        return *v;
    }

  protected:
    u64              current_entity;
    EntityContainer& m_container;

    ContainerProxy(EntityContainer& container) :
        current_entity(0), m_container(container)
    {
    }
};

template<typename ComponentList, typename SubsystemList>
struct ConstrainedProxy : ContainerProxy
{
    ConstrainedProxy(EntityContainer& container) : ContainerProxy(container)
    {
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type* get(u64 id)
    {
        type_list::type_in_list<ComponentType, ComponentList>();
        return ContainerProxy::get<ComponentType>(id);
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type* get()
    {
        return get<ComponentType>(current_entity);
    }

    template<typename OutputType>
    FORCEDINLINE Subsystem<OutputType>& subsystem()
    {
        type_list::type_in_list<OutputType, SubsystemList>();
        return ContainerProxy::subsystem<OutputType>();
    }
};

template<typename CompList, typename SubsysList>
struct EntityVisitor : EntityVisitorBase
{
    using ComponentList = CompList;
    using SubsystemList = SubsysList;

    using VisitorType = EntityVisitor<CompList, SubsysList>;
    using Proxy       = ConstrainedProxy<CompList, SubsysList>;

    EntityVisitor(u32 tag = 0) :
        EntityVisitorBase(
            type_list::collect_list<CompList>(),
            type_list::collect_list<SubsysList>(),
            tag)
    {
    }

    virtual bool dispatch(
        EntityContainer& container, time_point const& current);

    virtual bool visit(Proxy&, Entity const& entity, time_point const& current)
    {
        return false;
    }
};

template<typename T>
using CNT = ComponentContainer<T>;

FORCEDINLINE
void EntityContainer::exec()
{
    time_point     time_now = clock::now();
    ContainerProxy proxy(*this);

    for(auto& subsys : subsystems)
        subsys.second->start_frame(proxy, time_now);

    /* TODO: Put visitors in buckets according to what data they access */
    for(auto const& visitor : visitors)
    {
        visitor->dispatch(*this, time_now);
    }

    for(auto& subsys : subsystems)
        subsys.second->end_frame(proxy, time_now);
}

template<typename ComponentType>
FORCEDINLINE void EntityContainer::register_component(
    ComponentContainer<ComponentType>& c)
{
    static const type_hash type_id = typeid(ComponentType).hash_code();

    if(components.find(type_id) != components.end())
        throw implementation_error("cannot register type twice");

    auto adapted = C_DCAST<ComponentContainerBase>(&c);

    if(C_RCAST<void*>(adapted) != C_RCAST<void*>(&c))
        throw implementation_error("pointer casts will fail");

    components.emplace(type_id, adapted);
}

namespace Allocators {

template<
    typename ComponentType,
    typename AllocationType = typename ComponentType::type>
struct VectorBaseContainer : ComponentContainer<ComponentType>
{
    using typename ComponentContainer<ComponentType>::type;

    using vector_type = Vector<AllocationType>;
    using map_type    = Map<u64, size_t>;

    map_type    m_mapping;
    vector_type m_data;

    virtual void register_entity(u64 id);
    virtual void unregister_entity(u64 id);
    virtual void prealloc(szptr count);
    virtual bool contains_entity(u64 id) const;

    //    virtual typename ComponentType::type* get(u64 id);
};

template<typename ComponentType, typename AllocationType>
inline void VectorBaseContainer<ComponentType, AllocationType>::register_entity(
    u64 id)
{
    m_mapping.insert({id, m_data.size()});
    m_data.push_back(typename vector_type::value_type());
}

template<typename ComponentType, typename AllocationType>
inline void VectorBaseContainer<ComponentType, AllocationType>::
    unregister_entity(u64 id)
{
    m_mapping.erase(id);
}

template<typename ComponentType, typename AllocationType>
inline void VectorBaseContainer<ComponentType, AllocationType>::prealloc(
    szptr count)
{
    m_data.reserve(m_data.size() + count);
}

template<typename ComponentType, typename AllocationType>
inline bool VectorBaseContainer<ComponentType, AllocationType>::contains_entity(
    u64 id) const
{
    return m_mapping.find(id) != m_mapping.end();
}

template<
    typename ComponentType,
    typename AllocationType = typename ComponentType::type>
struct VectorContainer : VectorBaseContainer<ComponentType, AllocationType>
{
    virtual typename ComponentType::type* get(u64 id);
};

template<typename ComponentType, typename AllocationType>
inline typename ComponentType::type* VectorContainer<
    ComponentType,
    AllocationType>::get(u64 id)
{
    auto it = this->m_mapping.find(id);

    if(it == this->m_mapping.end())
        return nullptr;

    return &this->m_data.at((*it).second);
}

} // namespace Allocators

template<typename CompList, typename SubsysList>
bool EntityVisitor<CompList, SubsysList>::dispatch(
    EntityContainer& container, time_point const& current)
{
    Proxy proxy(container);

    for(auto const& entity : proxy.select(tags))
        this->visit(proxy, entity, current);

    return true;
}

} // namespace Components
} // namespace Coffee
