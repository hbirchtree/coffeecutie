#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/enum/helpers.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/time_types.h>
#include <peripherals/stl/type_list.h>

#define ENTCOMP_CREATE_TAG(name)      \
    struct name                       \
    {                                 \
        using type = libc_types::u32; \
    };

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

template<typename WrappedType, typename ValueType>
struct ValueTag
{
    using type       = WrappedType;
    using value_type = ValueType;
};

template<typename WrappedType, typename Tagging = void>
struct TagType
{
    using type = WrappedType;
};

template<class T>
concept is_tag_type = requires(T& v)
{
    typename T::type;
};

enum class VisitorFlags : u32
{
    None,
    MainThread = 0x1, /* Must run on main-thread */
};

C_FLAGS(VisitorFlags, u32);

struct EntityRecipe
{
    EntityRecipe(Vector<size_t>&& comps = {}, u32 tags = 0) :
        components(std::move(comps)), tags(tags)
    {
    }

    Vector<size_t> components;
    u32            tags;
};

struct Entity : non_copy
{
    u64 id;
    u32 tags;
    u32 _pad = 0;
};

struct ComponentContainerBase : non_copy
{
    virtual ~ComponentContainerBase();

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
    VisitorFlags      flags;
    u32               tags;

    EntityVisitorBase(
        type_hashes&& components,
        type_hashes&& subsystems,
        u32           tags  = 0,
        VisitorFlags  flags = VisitorFlags::None) :
        components(std::move(components)),
        subsystems(std::move(subsystems)), flags(flags), tags(tags)
    {
    }

    virtual ~EntityVisitorBase();

    virtual bool dispatch(
        EntityContainer& container, const time_point& current) = 0;
};

template<class T>
concept is_subsystem = true;
// concept is_subsystem = std::is_convertible_v<T&, SubsystemBase&>;

template<class T>
concept is_component = std::is_convertible_v<T&, ComponentContainerBase&> &&
    requires(T v)
{
    typename T::value_type;
};

template<class T>
concept is_visitor = std::is_convertible_v<T&, EntityVisitorBase&>;

template<typename T>
concept is_subsystem_tag = is_tag_type<T> && is_subsystem<typename T::type>;

template<typename T>
concept is_component_tag = is_tag_type<T> && is_component<typename T::type>;

template<typename T>
concept is_visitor_tag = is_tag_type<T> && is_visitor<typename T::type>;

template<is_subsystem... Args>
using subsystem_list = TypeList<Args...>;

template<is_component... Args>
using component_list = TypeList<Args...>;

template<is_visitor... Args>
using visitor_list = TypeList<Args...>;

template<typename ComponentType>
struct ComponentContainer : ComponentContainerBase
{
    using tag_type   = ComponentType;
    using type       = typename ComponentType::type;
    using value_type = typename ComponentType::value_type;

    virtual value_type* get(u64 id) = 0;

    virtual value_type const* get(u64 id) const final
    {
        return C_CCAST<ComponentContainer<ComponentType>*>(this)->get(id);
    }
};

struct SubsystemBase
{
    using ContainerProxy = Components::ContainerProxy;
    using time_point     = Components::time_point;
    using duration       = Components::duration;

    static constexpr u32 default_prio = 1024;
    static constexpr u32 system_prio  = 0;

    virtual ~SubsystemBase();

    SubsystemBase()                     = default;
    SubsystemBase(SubsystemBase const&) = delete;

    virtual void start_frame(ContainerProxy&, time_point const&)
    {
    }
    virtual void end_frame(ContainerProxy&, time_point const&)
    {
    }

    u32 priority = default_prio;

  protected:
    static EntityContainer& get_container(ContainerProxy& proxy);
};

template<is_tag_type OutputType>
struct Subsystem : SubsystemBase
{
    using tag_type = OutputType;
    using type     = typename OutputType::value_type;

    virtual type const& get() const
    {
        Throw(implementation_error("unimplemented getter"));
    }
    virtual type& get()
    {
        Throw(implementation_error("unimplemented getter"));
    }
};

namespace Globals {

template<is_tag_type T>
struct ValueSubsystem : Subsystem<T>
{
    using value_type = typename T::value_type;

    virtual value_type const& get() const override
    {
        return m_value;
    }
    virtual value_type& get() override
    {
        return m_value;
    }

  private:
    value_type m_value;
};

} // namespace Globals

// namespace Globals
} // namespace Components
} // namespace Coffee
