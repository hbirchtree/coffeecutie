#pragma once

#include "entity_container.h"

#include <tuple>
#include <utility>

namespace compo {

template<typename ContainerType, typename ComponentType>
struct ComponentRef
{
    ComponentRef()
        : m_id(0)
        , m_ref(nullptr)
    {
    }

    ComponentRef(u64 id, ContainerType* container)
        : m_id(id)
        , m_ref(container)
    {
    }

    ComponentRef(ComponentRef&&)      = default;
    ComponentRef(ComponentRef const&) = default;

    ComponentRef& operator=(ComponentRef const&) = default;

    decltype(auto) operator*() const
    {
        return (*m_ref->template get<ComponentType>(m_id));
    }

    bool exists() const
    {
        return m_ref && m_ref->exists(m_id);
    }

    u64            m_id;
    ContainerType* m_ref;
};

template<typename ContainerType>
struct EntityRef
{
    EntityRef()
    {
    }

    EntityRef(u64 source, ContainerType* container)
        : m_id(source)
        , container(container)
    {
    }

    template<typename T>
    requires requires(ContainerType& c, u64 id) { c.template get<T>(id); }
    decltype(auto) get() const
    {
        auto ptr = container->template get<T>(m_id);
        if(!ptr)
            Throw(std::out_of_range("component not found"));
        return (*ptr);
    }

    u64 id() const
    {
        return m_id;
    }

    u64 tags() const
    {
        return container->tags_of(m_id);
    }

    template<typename T>
    ComponentRef<ContainerType, T> ref() const
    {
        return ComponentRef<ContainerType, T>(m_id, container);
    }

    bool exists() const
    {
        return container && container->exists(m_id);
    }

  private:
    u64            m_id{};
    ContainerType* container{};
};

/* Out-of-line: needs the full EntityRef definition, which
 * entity_container.h only forward-declares */
FORCEDINLINE EntityRef<EntityContainer> EntityContainer::entity_query::
operator*() const
{
    if(it == m_container->entities.end())
        Throw(std::out_of_range("bad iterator"));

    return EntityRef<EntityContainer>(it->id, m_container);
}

template<typename ProxyType>
struct proxy_entity_query
{
    using value_type = EntityRef<ProxyType>;

    proxy_entity_query(
        ProxyType& proxy, EntityContainer::entity_query const& query)
        : m_proxy(&proxy)
        , m_query(query)
    {
    }

    proxy_entity_query& operator++()
    {
        ++m_query;
        return *this;
    }

    bool operator==(proxy_entity_query const& other) const
    {
        return m_query == other.m_query;
    }

    value_type operator*() const
    {
        return value_type((*m_query).id(), m_proxy);
    }

  private:
    ProxyType*                    m_proxy;
    EntityContainer::entity_query m_query;
};

namespace detail {

template<typename U, typename... Ts>
constexpr size_t pack_index()
{
    constexpr bool matches[] = {std::is_same_v<U, Ts>...};
    for(size_t i = 0; i < sizeof...(Ts); ++i)
        if(matches[i])
            return i;
    return static_cast<size_t>(-1);
}

} // namespace detail

/*!
 * \brief EntityRef with prefetched payload pointers for the components it
 * was selected with. get<T>() for a selected T dereferences the cached
 * pointer; any other component falls back to the container lookup.
 * The pointers obey component-storage rules: valid until the next
 * mutation of the respective component container.
 */
template<typename AccessList, typename... Components>
struct ComponentEntityRef : EntityRef<EntityContainer>
{
    template<typename U>
    static constexpr bool is_selected = (std::is_same_v<U, Components> || ...);

    template<typename U>
    using payload_t = access::projected_value_t<U, AccessList>;

    ComponentEntityRef(
        u64              id,
        EntityContainer* container,
        typename Components::value_type*... payload)
        : EntityRef<EntityContainer>(id, container)
        , m_payload(payload...)
    {
    }

    template<typename U>
    payload_t<U>& get() const
    {
        static_assert(
            access::projected_readable_v<U, AccessList>,
            "component not declared in the manifest of this subsystem");

        if constexpr(is_selected<U>)
            return *std::get<detail::pack_index<U, Components...>()>(m_payload);
        else
            return EntityRef<EntityContainer>::template get<U>();
    }

    /*!
     * \brief All prefetched payloads as a tuple of references, in the
     * order they were selected. For structured bindings:
     *   for(auto ent : e.select<Velocity, Position>()) {
     *       auto [vel, pos] = ent.components();
     *   }
     */
    std::tuple<payload_t<Components>&...> components() const
    {
        return std::apply(
            [](auto*... p) {
                return std::tuple<payload_t<Components>&...>(*p...);
            },
            m_payload);
    }

  private:
    std::tuple<typename Components::value_type*...> m_payload;
};

/*!
 * \brief Iterator for EntityContainer::select<Components...>(): walks the
 * first component's dense id array, filters on the remaining components'
 * sparse sets, and yields ComponentEntityRef with all payload pointers
 * resolved.
 */
template<typename AccessList, typename... Components>
struct component_query
{
    using value_type = ComponentEntityRef<AccessList, Components...>;

    component_query(EntityContainer& c, bool at_end)
        : m_container(&c)
        , m_comps(&c.container_cast<Components>()...)
        , m_idx(0)
    {
        if(at_end)
            m_idx = primary().m_dense.size();
        else
            advance_valid();
    }

    component_query& operator++()
    {
        ++m_idx;
        advance_valid();
        return *this;
    }

    bool operator==(component_query const& other) const
    {
        return m_idx == other.m_idx;
    }

    value_type operator*() const
    {
        auto const& dense = primary().m_dense;
        if(m_idx >= dense.size())
            Throw(std::out_of_range("bad iterator"));
        return deref(dense[m_idx], std::index_sequence_for<Components...>());
    }

  private:
    auto& primary() const
    {
        return *std::get<0>(m_comps);
    }

    bool all_contain(u64 id) const
    {
        return std::apply(
            [id](auto* /*first*/, auto*... rest) {
                return (rest->contains_entity(id) && ...);
            },
            m_comps);
    }

    void advance_valid()
    {
        auto const& dense = primary().m_dense;
        while(m_idx < dense.size() && !all_contain(dense[m_idx]))
            ++m_idx;
    }

    template<typename Component>
    static constexpr size_t write_offset =
        std::is_void_v<AccessList>
            ? 0u
            : (access::projected_writable_v<Component, AccessList> ? 1u : 0u);

    template<size_t... Is>
    value_type deref(u64 id, std::index_sequence<Is...>) const
    {
        return value_type(
            id,
            m_container,
            (std::get<Is>(m_comps)->resolve(id, write_offset<Components>))...);
    }

    EntityContainer*                          m_container;
    std::tuple<typename Components::type*...> m_comps;
    size_t                                    m_idx;
};

} // namespace compo
