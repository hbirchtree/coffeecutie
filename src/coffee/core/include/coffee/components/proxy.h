#pragma once

#include <peripherals/stl/type_list.h>

#include "access.h"
#include "entity_reference.h"
#include "platforms/stacktrace.h"
#include "types.h"

namespace compo {

using type_safety::empty_list_t;
using type_safety::type_list_t;
using type_safety::type_list::type_in_list_v;

struct ContainerProxy : stl_types::non_copy
{
    friend struct EntityContainer;
    template<typename T1, typename T2>
    friend struct EntityVisitor;
    friend struct SubsystemBase;

    FORCEDINLINE
    quick_container<EntityContainer::entity_query> select(u64 tags)
    {
        return m_container.select(tags);
    }

    template<is_component_tag... Components>
    FORCEDINLINE quick_container<component_query<void, Components...>> select()
    {
        return m_container.select<Components...>();
    }

    template<typename Matcher>
    FORCEDINLINE quick_container<EntityContainer::entity_query> match()
    {
        return m_container.match<Matcher>();
    }

    template<typename ContainerType>
    FORCEDINLINE EntityRef<ContainerType> ref(Entity const& e)
    {
        return EntityRef<ContainerType>(e.id, C_CAST<ContainerType*>(this));
    }

    template<typename ContainerType>
    FORCEDINLINE EntityRef<ContainerType> ref(u64 e)
    {
        return EntityRef<ContainerType>(e, C_CAST<ContainerType*>(this));
    }

    FORCEDINLINE EntityRef<EntityContainer> ref(Entity const& e)
    {
        return m_container.ref(e);
    }

    FORCEDINLINE EntityRef<EntityContainer> ref(u64 e)
    {
        return m_container.ref(e);
    }

    FORCEDINLINE EntityRef<EntityContainer> create_entity(
        EntityRecipe const& recipe)
    {
        return m_container.create_entity(recipe);
    }

    FORCEDINLINE void remove_entity_if(
        std::function<bool(Entity const&)>&& predicate)
    {
        m_container.remove_entity_if(std::move(predicate));
    }

    template<is_tag_type Service>
    FORCEDINLINE typename Service::type* service()
    {
        return m_container.service<Service>();
    }

    auto& underlying()
    {
        return m_container;
    }

  protected:
    u64              current_entity;
    EntityContainer& m_container;

    ContainerProxy(EntityContainer& container)
        : current_entity(0)
        , m_container(container)
    {
    }
};

/*!
 * \brief The container as seen through a subsystem's manifest.
 */
template<
    typename ComponentList,
    typename SubsystemList,
    typename ServiceList = type_safety::empty_list_t>
struct ConstrainedProxy : private ContainerProxy
{
    template<typename T1, typename T2>
    friend struct EntityVisitor;

    using component_list = ComponentList;
    using subsystem_list = SubsystemList;
    using service_list   = ServiceList;

    using self_type = ConstrainedProxy<ComponentList, SubsystemList, ServiceList>;

    ConstrainedProxy(EntityContainer& container)
        : ContainerProxy(container)
    {
    }

    FORCEDINLINE quick_container<proxy_entity_query<self_type>> select(u64 tags)
    {
        return wrap_query(m_container.select(tags));
    }

    template<is_component_tag... Components>
    requires(access::readable_v<Components, ComponentList> && ...)
    FORCEDINLINE quick_container<component_query<ComponentList, Components...>>
    select()
    {
        return m_container
            .template select_restricted<ComponentList, Components...>();
    }

    template<typename Matcher>
    FORCEDINLINE quick_container<proxy_entity_query<self_type>> match()
    {
        return wrap_query(m_container.match<Matcher>());
    }

    FORCEDINLINE EntityRef<EntityContainer> ref(Entity const& e)
    {
        return m_container.ref(e);
    }

    FORCEDINLINE EntityRef<EntityContainer> ref(u64 e)
    {
        return m_container.ref(e);
    }

    template<typename ContainerType>
    FORCEDINLINE EntityRef<ContainerType> ref(u64 e)
    {
        return EntityRef<ContainerType>(e, C_CAST<ContainerType*>(this));
    }

    template<typename ContainerType>
    FORCEDINLINE EntityRef<ContainerType> ref(Entity const& e)
    {
        return EntityRef<ContainerType>(e.id, C_CAST<ContainerType*>(this));
    }

    FORCEDINLINE EntityRef<EntityContainer> create_entity(
        EntityRecipe const& recipe)
    {
        note_structural_mutation();
        return m_container.create_entity(recipe);
    }

    FORCEDINLINE void remove_entity_if(
        std::function<bool(Entity const&)>&& predicate)
    {
        note_structural_mutation();
        m_container.remove_entity_if(std::move(predicate));
    }

    /*!
     * \brief Full container access, deliberately outside the manifest.
     */
    FORCEDINLINE EntityContainer& unconstrained_container()
    {
        note_opaque_access();
        return m_container;
    }

    FORCEDINLINE bool exists(u64 id) const
    {
        return m_container.exists(id);
    }

    FORCEDINLINE u64 tags_of(u64 id) const
    {
        return m_container.tags_of(id);
    }

    /*
     * Component access
     */

    /*!
     * \brief The component, in whichever buffer the declaration implies.
     */
    template<is_component_tag ComponentType>
    requires access::readable_v<ComponentType, ComponentList>
    FORCEDINLINE access::value_t<ComponentType, ComponentList>* get(u64 id)
    {
        constexpr size_t offset =
            access::writable_v<ComponentType, ComponentList> ? 1u : 0u;
        return m_container.get_at<ComponentType>(id, offset);
    }

    /*!
     * \brief The component at an explicit frame offset.
     */
    template<is_component_tag ComponentType, size_t Offset>
    requires(
        Offset == 0 ? access::readable_v<ComponentType, ComponentList>
                    : access::writable_v<ComponentType, ComponentList>)
    FORCEDINLINE std::conditional_t<
        Offset == 0,
        access::value_t<ComponentType, ComponentList>,
        typename ComponentType::value_type>* get_at(u64 id)
    {
        return m_container.get_at<ComponentType>(id, Offset);
    }

    template<is_component_tag ComponentType>
    requires access::readable_v<ComponentType, ComponentList>
    FORCEDINLINE access::value_t<ComponentType, ComponentList>& get()
    {
        auto v = get<ComponentType>(current_entity);

        if constexpr(compile_info::debug_mode)
            if(!v)
                ThrowNotFound<ComponentType>("component not found: ");

        return *v;
    }

    template<is_tag_type SubsystemType>
    requires access::readable_v<SubsystemType, SubsystemList>
    FORCEDINLINE access::system_t<SubsystemType, SubsystemList>& subsystem()
    {
        return m_container.subsystem_cast<SubsystemType>();
    }

    template<is_tag_type SubsystemType>
    requires access::writable_v<SubsystemType, SubsystemList>
    FORCEDINLINE void subsystem(SubsystemType*& target)
    {
        target = &m_container.subsystem_cast<SubsystemType>();
    }

    template<is_tag_type SubsystemType>
    requires access::readable_v<SubsystemType, SubsystemList>
    FORCEDINLINE void subsystem(SubsystemType const*& target)
    {
        target = &m_container.subsystem_cast<SubsystemType>();
    }

    template<is_tag_type Service>
    requires access::readable_v<Service, ServiceList>
    FORCEDINLINE access::system_t<Service, ServiceList>* service()
    {
        return m_container.service<Service>();
    }

    /*!
     * \brief Every registered service implementing BaseType.
     */
    template<class BaseType>
    requires access::readable_v<TagType<BaseType>, ServiceList>
    FORCEDINLINE auto services_with()
    {
        return m_container.services_with<BaseType>();
    }

  private:
    template<typename Query>
    FORCEDINLINE quick_container<proxy_entity_query<self_type>> wrap_query(
        Query&& query)
    {
        return quick_container<proxy_entity_query<self_type>>(
            proxy_entity_query<self_type>(*this, query.begin()),
            proxy_entity_query<self_type>(*this, query.end()));
    }

    /* Recorded against whichever subsystem's frame hook is running */
    FORCEDINLINE void note_structural_mutation()
    {
        access::note_structural_mutation();
    }

    FORCEDINLINE void note_opaque_access()
    {
        access::note_opaque_access();
    }
};

template<typename Manifest>
using proxy_of = ConstrainedProxy<
    typename Manifest::components,
    typename Manifest::subsystems,
    typename Manifest::services>;

} // namespace compo
