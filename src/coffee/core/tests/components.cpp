#include <coffee/components/components.h>
#include <coffee/components/scheduling.h>
#include <coffee/core/CUnitTesting>

using namespace compo;

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
static_assert(
    std::is_same_v<
        decltype(std::declval<MixedProxy&>()
                     .select<Position, Velocity>()
                     .front()
                     .get<Velocity>()),
        Velocity const&>);
static_assert(can_get_via_tag_query<MixedProxy, Velocity>);

/* An unrestricted container query keeps payloads mutable */
static_assert(
    std::is_same_v<
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

bool test_component_projection()
{
    EntityContainer container;
    container.register_component_inplace<Position>();
    container.register_component_inplace<Velocity>();
    container.register_subsystem_inplace<Clock>();
    container.register_subsystem_inplace<Journal>();

    EntityRecipe recipe;
    recipe.components = {
        typeid(Position).hash_code(), typeid(Velocity).hash_code()};
    recipe.tags = 0x1;

    for(int i = 0; i < 4; i++)
        container.create_entity(recipe);

    MixedProxy proxy(container);

    size_t seen = 0;
    for(auto entity : proxy.select<Position, Velocity>())
    {
        entity.get<Position>().x = 2.f + entity.get<Velocity>().dx;
        seen++;
    }
    assertEquals(seen, size_t(4));

    /* Tag queries yield refs routed through the proxy, not the container */
    size_t tagged = 0;
    for(auto entity : proxy.select(0x1))
    {
        entity.get<Position>().x += 1.f;
        tagged++;
    }
    assertEquals(tagged, size_t(4));
    assertEquals(proxy.get<Position>(1)->x, 3.f);

    proxy.subsystem<Clock>().ticks = 11;
    assertEquals(proxy.subsystem<Clock>().ticks, 11);

    return true;
}

/*
 * Scheduling. Access sets are built by hand so the batching rules are tested
 * on their own, independent of how a manifest is collected.
 */

sched::access_set set_of(
    type_hash                          self,
    std::vector<sched::access_entry>&& components,
    std::vector<sched::access_entry>&& systems = {})
{
    sched::access_set out;
    out.self       = self;
    out.components = std::move(components);
    out.subsystems = std::move(systems);
    return out;
}

constexpr type_hash pos_h = 1;
constexpr type_hash vel_h = 2;

sched::access_entry reads(type_hash h)
{
    return {h, access::mode::read};
}

sched::access_entry writes(type_hash h)
{
    return {h, access::mode::write};
}

std::vector<sched::node> nodes_of(std::vector<sched::access_set>&& sets)
{
    std::vector<sched::node> out;
    for(size_t i = 0; i < sets.size(); i++)
        out.push_back({nullptr, sets.at(i), "sys" + std::to_string(i)});
    return out;
}

bool test_conflicts()
{
    auto write_pos = set_of(10, {writes(pos_h)});
    auto read_pos  = set_of(11, {reads(pos_h)});
    auto read_pos2 = set_of(12, {reads(pos_h)});
    auto write_vel = set_of(13, {writes(vel_h)});

    /* Concurrent reads of the same component are fine */
    assertFalse(sched::conflicts(read_pos, read_pos2));
    /* A writer excludes both readers and other writers */
    assertTrue(sched::conflicts(write_pos, read_pos));
    assertTrue(sched::conflicts(write_pos, write_pos));
    /* Disjoint components never conflict */
    assertFalse(sched::conflicts(write_pos, write_vel));

    /* A subsystem is a resource: its own frame hooks write it */
    auto depends_on_10 = set_of(14, {}, {reads(10)});
    assertTrue(sched::conflicts(write_pos, depends_on_10));

    /* Structural mutation invalidates every query, so it excludes all */
    auto mutator             = set_of(15, {});
    mutator.structural_mutation = true;
    assertTrue(sched::conflicts(mutator, write_vel));
    assertTrue(sched::conflicts(mutator, read_pos));

    return true;
}

bool test_batching()
{
    /* Two disjoint readers merge, a writer of the same component splits */
    auto nodes = nodes_of(
        {set_of(10, {reads(pos_h)}),
         set_of(11, {reads(pos_h)}),
         set_of(12, {writes(pos_h)}),
         set_of(13, {writes(vel_h)})});
    auto batches = sched::build_batches(nodes);

    assertEquals(batches.size(), size_t(2));
    assertEquals(batches.at(0).members.size(), size_t(2));
    assertEquals(batches.at(1).members.size(), size_t(2));

    /* Priority order is never inverted: a conflicting pair keeps its
     * relative order across the batch boundary */
    size_t previous = 0;
    for(auto const& batch : batches)
        for(auto member : batch.members)
        {
            assertTrue(member >= previous);
            previous = member;
        }

    /* A structural mutator gets a stage to itself */
    auto mutator                = set_of(11, {reads(vel_h)});
    mutator.structural_mutation = true;

    auto mixed    = nodes_of(
        {set_of(10, {reads(pos_h)}), mutator, set_of(12, {reads(pos_h)})});
    auto isolated = sched::build_batches(mixed);

    assertEquals(isolated.size(), size_t(3));
    assertEquals(isolated.at(1).members.size(), size_t(1));

    /* Width counts what could actually run at once: unpinned members plus
     * one main thread */
    auto pinned_a        = set_of(10, {reads(pos_h)});
    auto pinned_b        = set_of(11, {reads(pos_h)});
    pinned_a.main_thread = true;
    pinned_b.main_thread = true;

    auto pinned_nodes = nodes_of({pinned_a, pinned_b, set_of(12, {reads(pos_h)})});
    auto pinned       = sched::build_batches(pinned_nodes);

    assertEquals(pinned.size(), size_t(1));
    assertEquals(pinned.at(0).width, size_t(2));

    return true;
}

} // namespace

COFFEE_TESTS_BEGIN(3)

    {test_component_projection,
     "Manifest access projection",
     "const in a manifest list makes the proxy hand back const payloads"},
    {test_conflicts,
     "Access set conflicts",
     "shared writes, subsystem dependencies and structural mutation"},
    {test_batching,
     "Frame batching",
     "priority-ordered greedy batching of non-conflicting subsystems"},

COFFEE_TESTS_END()
