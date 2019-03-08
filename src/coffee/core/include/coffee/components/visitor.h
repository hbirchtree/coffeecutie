#pragma once

#include <coffee/components/proxy.h>
#include <coffee/components/types.h>

namespace Coffee {
namespace Components {

template<typename CompList, typename SubsysList>
struct EntityVisitor : EntityVisitorBase
{
    using ComponentList = CompList;
    using SubsystemList = SubsysList;

    using VisitorFlags = Components::VisitorFlags;
    using time_point   = Components::time_point;
    using Entity       = Components::Entity;
    using VisitorType  = EntityVisitor<CompList, SubsysList>;
    using Proxy        = ConstrainedProxy<CompList, SubsysList>;

    EntityVisitor(u32 tag = 0, VisitorFlags flags = VisitorFlags::None) :
        EntityVisitorBase(
            type_list::collect_list<CompList>(),
            type_list::collect_list<SubsysList>(),
            tag,
            flags)
    {
    }

    virtual bool dispatch(
        EntityContainer& container, time_point const& current);

    virtual bool visit(Proxy&, Entity const&, time_point const&)
    {
        return false;
    }
};

template<typename CompList, typename SubsysList>
inline bool EntityVisitor<CompList, SubsysList>::dispatch(
    EntityContainer& container, time_point const& current)
{
    Proxy proxy(container);

    for(auto const& entity : proxy.select(tags))
    {
        proxy.current_entity = entity.id;
        this->visit(proxy, entity, current);
    }

    return true;
}

} // namespace Components
} // namespace Coffee
