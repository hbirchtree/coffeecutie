#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/time_types.h>
#include <peripherals/stl/type_list.h>

namespace Coffee {
namespace Components {

using clock      = Chrono::high_resolution_clock;
using duration   = Chrono::microseconds;
using time_point = Chrono::time_point<clock>;

using type_hash = declreturntype(std::type_info::hash_code);

struct EntityContainer;
struct ContainerProxy;

template<typename... Types>
using TypeList = type_safety::type_list::type_list<Types...>;

template<typename WrappedType, typename TagType = void>
struct TaggedTypeWrapper
{
    using type = WrappedType;
    using tag  = TagType;
};

template<typename WrappedType, typename TaggedType = void>
using TagType = TaggedTypeWrapper<WrappedType, TaggedType>;

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
    using tag_type = ComponentType;
    using type     = typename ComponentType::type;

    virtual type* get(u64 id) = 0;
};

struct SubsystemBase : non_copy
{
    using ContainerProxy = Components::ContainerProxy;
    using time_point     = Components::time_point;

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
    using tag_type = OutputType;
    using type     = typename OutputType::type;

    virtual type const& get() const = 0;
    virtual type&       get()       = 0;
};

namespace Globals {

template<typename T>
struct ValueSubsystem : Subsystem<T>
{
    using type = typename Subsystem<T>::type;

    virtual type const& get() const
    {
        return m_value;
    }
    virtual type& get()
    {
        return m_value;
    }

  private:
    type m_value;
};

} // namespace Globals
} // namespace Components
} // namespace Coffee
