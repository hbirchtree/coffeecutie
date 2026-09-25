#include <coffee/components/components.h>
#include <peripherals/stl/range.h>

namespace compo {

ComponentContainerBase::~ComponentContainerBase()
{
}

EntityVisitorBase::~EntityVisitorBase()
{
}

SubsystemBase::~SubsystemBase()
{
}

/* Compile-time checks of manifest access projection: what a
 * ConstrainedProxy hands back, and what it refuses to compile */
namespace {

struct Position
{
    using value_type = Position;
    using type       = alloc::VectorContainer<value_type>;

    float x{0.f};
};

struct Velocity
{
    using value_type = Velocity;
    using type       = alloc::VectorContainer<value_type>;

    float dx{0.f};
};

struct Unlisted
{
    using value_type = Unlisted;
    using type       = alloc::VectorContainer<value_type>;
};

struct Clock : SubsystemBase
{
    using type = Clock;

    int ticks{0};
};

struct Journal : SubsystemBase
{
    using type = Journal;

    int lines{0};
};

/* Position is writable, Velocity read-only; Clock writable, Journal not */
using MixedManifest = SubsystemManifest<
    type_list_t<Position, const Velocity>,
    type_list_t<Clock, const Journal>,
    empty_list_t>;

using MixedProxy = proxy_of<MixedManifest>;

/*
 * Negative cases go through a dependent receiver: GCC reports a hard error
 * rather than an unsatisfied requirement when a requires-expression names a
 * member template on a concrete class.
 */
template<typename Proxy, typename T>
concept can_get = requires(Proxy& p) { p.template get<T>(); };

template<typename Proxy, typename... T>
concept can_select = requires(Proxy& p) { p.template select<T...>(); };

template<typename Proxy, typename T>
concept can_take_mutable =
    requires(Proxy& p, typename T::type* s) { p.template subsystem<T>(s); };

template<typename Proxy>
concept can_escape = requires(Proxy& p) { p.underlying(); };

template<typename Proxy, typename T>
concept can_get_via_tag_query =
    requires(Proxy& p) { p.select(0u).front().template get<T>(); };

/* 1. const in a manifest list projects onto what the proxy hands back */
static_assert(std::is_same_v<
              decltype(std::declval<MixedProxy&>().get<Position>()),
              Position&>);
static_assert(std::is_same_v<
              decltype(std::declval<MixedProxy&>().get<Velocity>()),
              Velocity const&>);
static_assert(std::is_same_v<
              decltype(std::declval<MixedProxy&>().subsystem<Clock>()),
              Clock&>);
static_assert(std::is_same_v<
              decltype(std::declval<MixedProxy&>().subsystem<Journal>()),
              Journal const&>);

/* ...including through select<>() payloads and tag-based queries */
static_assert(std::is_same_v<
              decltype(std::declval<MixedProxy&>()
                           .select<Position, Velocity>()
                           .front()
                           .get<Velocity>()),
              Velocity const&>);
static_assert(can_get_via_tag_query<MixedProxy, Velocity>);

/* An unrestricted container query keeps payloads mutable */
static_assert(std::is_same_v<
              decltype(std::declval<EntityContainer&>()
                           .select<Position>()
                           .front()
                           .get<Position>()),
              Position&>);

/* 2. undeclared access does not compile */
static_assert(can_get<MixedProxy, Position> && can_get<MixedProxy, Velocity>);
static_assert(!can_get<MixedProxy, Unlisted>);
static_assert(can_select<MixedProxy, Position, Velocity>);
static_assert(!can_select<MixedProxy, Unlisted>);
static_assert(!can_get_via_tag_query<MixedProxy, Unlisted>);
static_assert(can_take_mutable<MixedProxy, Clock>);
static_assert(!can_take_mutable<MixedProxy, Journal>);

/* 3. the constraint cannot be walked around */
static_assert(!can_escape<MixedProxy>);
static_assert(can_escape<ContainerProxy>);

} // namespace

} // namespace compo
