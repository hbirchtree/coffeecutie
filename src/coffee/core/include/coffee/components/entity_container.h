#pragma once

#include <algorithm>
#include <coffee/components/types.h>
#include <coffee/core/printing/log_interface.h>
#include <peripherals/stl/quick_container.h>

namespace compo {
namespace convenience {

using hash_type = declmemtype(&std::type_info::hash_code);

template<typename T>
FORCEDINLINE hash_type type_hash_v()
{
    return typeid(T).hash_code();
}

} // namespace convenience

using convenience::type_hash_v;
using stl_types::quick_container;

namespace detail {
struct visitor_path;
}

template<typename ContainerType>
struct EntityRef;

template<typename ContainerType, typename ComponentType>
struct ComponentRef;

template<typename Service>
struct ServiceRef;

template<typename T, bool Reversed = false>
struct service_query;

struct reverse_query_t
{
};

constexpr reverse_query_t reverse_query;

namespace matchers {

template<typename subsystem_list>
struct all_subsystems_in
{
    struct match_single
    {
        match_single(SubsystemBase* sub, bool& match) : sub(sub), match(match)
        {
        }
        template<typename T>
        void operator()()
        {
            match = match || C_DCAST<T>(sub);
        }
        SubsystemBase* sub;
        bool&          match;
    };

    static bool match(SubsystemBase* sub)
    {
        bool out = false;
        type_safety::type_list::for_each<subsystem_list>(
            match_single(sub, out));
        return out;
    }
};

} // namespace matchers

template<class T>
concept is_matcher
    = std::is_same_v<decltype(T::match(std::declval<SubsystemBase>())), bool>;

template<typename BaseType>
using service_sort_predicate = std::function<bool(BaseType*, BaseType*)>;

template<class T>
concept has_priority = requires(T& a)
{
    std::is_integral_v<decltype(a.priority)>;
};

namespace sorter
{

    template<class BaseType, class CompType>
    requires has_priority<CompType>
    constexpr bool (*priority_ranked)(BaseType*, BaseType*)
        = [](BaseType* left, BaseType* right) {
              return dynamic_cast<CompType*>(left)->priority
                     < dynamic_cast<CompType*>(right)->priority;
          };

} // namespace sorter

struct EntityContainer : stl_types::non_copy
{
    enum DebugFlags
    {
        Verbose_Subsystems = 0x1,
        Verbose_Visitors   = 0x2,
    };

    friend struct Entity;
    template<typename T, bool Reversed>
    friend struct service_query;

    EntityContainer() : entity_counter(0), debug_flags(0)
    {
        time_offset = clock::now() - clock::time_point();
    }

    struct entity_query
    {
        using value_type       = Entity;
        using size_type        = szptr;
        using entity_predicate = std::function<bool(Entity const&)>;

        entity_query(EntityContainer& c, u64 tags) :
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
            it = std::find_if(++it, m_container->entities.end(), pred);
            return *this;
        }

        bool operator==(entity_query const& other) const
        {
            return it == other.it;
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

        entity_predicate const        pred;
        EntityContainer* const        m_container;
        std::vector<Entity>::iterator it;
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
    void register_component(
        std::unique_ptr<ComponentContainer<ComponentType>>&& c);

    template<typename ContainerType, typename... Args>
    void register_component_inplace(Args... args)
    {
        register_component<ContainerType>(
            std::make_unique<typename ContainerType::type>(
                std::forward<Args>(args)...));
    }

    template<typename OutputType>
    void register_subsystem(std::unique_ptr<typename OutputType::type>&& sys)
    {
        static const type_hash type_id = typeid(OutputType).hash_code();

        if(subsystems.find(type_id) != subsystems.end())
            throw implementation_error("cannot register subsystem twice");

        subsystems.emplace(type_id, std::move(sys));
    }

    void register_system(std::unique_ptr<EntityVisitorBase>&& visitor)
    {
        visitors.push_back(std::move(visitor));
    }

    template<
        typename OutputType,
        typename AllocType = typename OutputType::type,
        typename... Args>
    requires std::is_convertible_v<AllocType*, typename OutputType::type*>
        AllocType& register_subsystem_inplace(Args... args)
    {
        register_subsystem<OutputType>(
            std::make_unique<AllocType>(std::forward<Args>(args)...));

        return C_OCAST<AllocType&>(subsystem_cast<OutputType>());
    }

    template<typename SystemType, typename... Args>
    void register_inplace(Args... args)
    {
        register_system(
            std::make_unique<SystemType>(std::forward<Args>(args)...));
    }

    template<typename ServiceType, typename SubsystemType>
    void register_subsystem_services(SubsystemType* subsystem);

    inline void add_component(u64 entity_id, size_t type_id)
    {
        container(type_id).register_entity(entity_id);
    }
    template<typename T>
    inline void add_component(u64 entity_id)
    {
        add_component(entity_id, typeid(T).hash_code());
    }

    /*
     *
     * For querying components or getting data
     *
     */

    quick_container<entity_query> select(u64 tags)
    {
        return quick_container<entity_query>(
            [this, tags]() { return entity_query(*this, tags); },
            [this]() { return entity_query(*this); });
    }

    template<is_component_tag ComponentType>
    quick_container<entity_query> select()
    {
        return quick_container<entity_query>(
            [this]() {
                return entity_query::from_container(
                    *this, container<ComponentType>());
            },
            [this]() { return entity_query(*this); });
    }

    template<is_matcher Matcher>
    quick_container<entity_query> match()
    {
        return quick_container<entity_query>(
            [this]() {
                return entity_query(*this, [this](Entity const& e) {
                    return Matcher::match(*this, e.id);
                });
            },
            [this]() { return entity_query(*this); });
    }

    ComponentContainerBase& container(size_t type_id)
    {
        auto it = components.find(type_id);

        if(it == components.end())
            throw undefined_behavior("component not found");

        return *it->second;
    }

    template<is_component_tag ComponentType>
    typename ComponentType::type& container_cast()
    {
        const type_hash type_id = typeid(ComponentType).hash_code();

        auto it = components.find(type_id);

        if(it == components.end())
            Throw(undefined_behavior("component not found"));

        auto ptr = C_DCAST<typename ComponentType::type>(it->second.get());

        if(ptr)
            return *ptr;
        else
            Throw(undefined_behavior("component not found"));
    }

    template<is_component_tag ComponentType>
    ComponentContainer<ComponentType>& container()
    {
        using container_t = ComponentContainer<ComponentType>;

        const type_hash type_id = typeid(ComponentType).hash_code();

        auto it = components.find(type_id);

        if(it == components.end())
            Throw(undefined_behavior("component not found"));

        return *C_DCAST<container_t>(it->second.get());
    }

    template<is_subsystem SubsystemType>
    typename SubsystemType::type& subsystem_cast()
    {
        const type_hash type_id = typeid(SubsystemType).hash_code();

        auto it = subsystems.find(type_id);

        if(it == subsystems.end())
            Throw(undefined_behavior("subsystem not found"));

        auto ptr = C_DCAST<typename SubsystemType::type>(it->second.get());

        if(ptr)
            return *ptr;
        else
            Throw(undefined_behavior("subsystem not found"));
    }

    template<is_tag_type TagType>
    SubsystemBase& subsystem()
    {
        const type_hash type_id = typeid(TagType).hash_code();

        auto it = subsystems.find(type_id);

        if(it == subsystems.end())
            Throw(undefined_behavior("subsystem not found"));

        return *it->second;
    }

    template<is_tag_type Service>
    typename Service::type* service()
    {
        const type_hash type_id = typeid(Service).hash_code();
        auto            it      = services.find(type_id);

        if(it == services.end())
            return nullptr;

        return C_DCAST<typename Service::type>(it->second);
    }

    template<is_subsystem Service>
    ServiceRef<Service> service_ref();

    template<class BaseType, bool Reversed = false>
    quick_container<service_query<BaseType, Reversed>> services_with();

    template<class BaseType>
    auto services_with(reverse_query_t);

    template<class BaseType>
    auto services_with(service_sort_predicate<BaseType> sorter);

    template<is_component_tag ComponentType>
    typename ComponentType::value_type* get(u64 id)
    {
        return container<ComponentType>().get(id);
    }

    EntityRef<EntityContainer> ref(Entity const& entity);
    EntityRef<EntityContainer> ref(u64 entity);

    template<is_tag_type ComponentTag>
    ComponentRef<EntityContainer, ComponentTag> ref_comp(u64 entity);

    template<is_matcher Matcher>
    void remove_subsystems_matching()
    {
        using subsystem_pair = decltype(subsystems)::value_type;

        auto it   = subsystems.begin();
        auto pred = [](subsystem_pair const& sub) {
            return Matcher::match(sub.second.get());
        };

        while(it != subsystems.end())
        {
            it = std::find_if(subsystems.begin(), subsystems.end(), pred);
            if(it != subsystems.end())
            {
                subsystems.erase(it);
                it = subsystems.begin();
            }
        }
    }

    EntityRef<EntityContainer> create_entity(EntityRecipe const& recipe);
    void remove_entity_if(std::function<bool(Entity const&)>&& predicate);

    u64 tags_of(u64 id) const;

    /* Time tracking */
    clock::time_point relative_timestamp() const
    {
        auto current_time = clock::now();
        return current_time - time_offset;
    }

    /* For optimizations */
    using visitor_graph = std::set<std::vector<bool>>;

    visitor_graph create_task_graph();

  private:
    u64                 entity_counter; /*!< For enumerating entities */
    std::vector<Entity> entities;

    std::map<type_hash, std::unique_ptr<ComponentContainerBase>> components;
    std::map<type_hash, std::unique_ptr<SubsystemBase>>          subsystems;
    std::vector<std::unique_ptr<EntityVisitorBase>>              visitors;
    std::map<type_hash, SubsystemBase*>                          services;

    clock::duration time_offset;

  public:
    u32 debug_flags;
};

} // namespace compo
