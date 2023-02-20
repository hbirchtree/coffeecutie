#pragma once

#include "proxy.h"
#include "types.h"

namespace compo {

using type_safety::type_list::collect_list;

template<typename CompList, typename SubsysList>
struct EntityVisitor : EntityVisitorBase
{
    using ComponentList = CompList;
    using SubsystemList = SubsysList;

    using VisitorFlags = compo::VisitorFlags;
    using time_point   = compo::time_point;
    using Entity       = compo::Entity;
    using VisitorType  = EntityVisitor<CompList, SubsysList>;
    using Proxy        = ConstrainedProxy<CompList, SubsysList>;

    EntityVisitor(u32 tag = 0, VisitorFlags flags = VisitorFlags::None) :
        EntityVisitorBase(
            collect_list<CompList>(),
            collect_list<SubsysList>(),
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

protected:
    void set_current_entity(Proxy& proxy, u64 id)
    {
        proxy.current_entity = id;
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

} // namespace compo
