#include <coffee/components/components.h>
#include <coffee/components/scheduling.h>
#include <coffee/components/worker_pool.h>
#include <coffee/core/CUnitTesting>

#include <atomic>

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

    auto state_only     = set_of(16, {});
    state_only.has_work = false;
    assertFalse(sched::conflicts(mutator, state_only));
    assertFalse(sched::conflicts(state_only, write_pos));

    auto queried        = set_of(17, {});
    queried.self_access = access::mode::read;

    auto asks   = set_of(18, {}, {reads(17)});
    auto drives = set_of(19, {}, {writes(17)});

    assertFalse(sched::conflicts(asks, queried));
    assertTrue(sched::conflicts(drives, queried));

    auto working = set_of(20, {});
    assertTrue(sched::conflicts(set_of(21, {}, {reads(20)}), working));

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

bool test_windows()
{
    /* Opting in is not enough on its own: undeclared access, structural
     * mutation and a main thread pin all keep a subsystem where it is */
    auto opted_in     = set_of(10, {reads(pos_h)});
    opted_in.parallel = true;
    assertTrue(sched::can_offload(opted_in));

    auto escaped     = opted_in;
    escaped.opaque   = true;
    assertFalse(sched::can_offload(escaped));

    auto pinned        = opted_in;
    pinned.main_thread = true;
    assertFalse(sched::can_offload(pinned));

    /* A subsystem that conflicts with nobody covers the whole frame, even
     * though it sits in the last batch */
    auto writer_a = set_of(11, {writes(pos_h)});
    auto writer_b = set_of(12, {writes(pos_h)});
    auto late     = set_of(13, {reads(vel_h)});
    late.parallel = true;

    auto nodes   = nodes_of({writer_a, writer_b, late});
    auto batches = sched::build_batches(nodes);
    auto windows = sched::build_windows(nodes, batches);

    assertEquals(batches.size(), size_t(2));
    assertTrue(windows.at(2).offloaded());
    assertEquals(windows.at(2).first, size_t(0));
    assertEquals(windows.at(2).last, size_t(1));
    /* ...while the ones that stayed behind are not offloaded at all */
    assertFalse(windows.at(0).offloaded());
    assertFalse(windows.at(1).offloaded());

    /* A conflict on either side closes the window down to the batch the
     * subsystem was scheduled in: this is the data dependency case, and it
     * has to hold inside the frame, not just at its edges */
    auto producer   = set_of(20, {writes(pos_h)});
    auto consumer   = set_of(21, {reads(pos_h)});
    consumer.parallel = true;
    auto next       = set_of(22, {writes(pos_h)});

    auto fenced         = nodes_of({producer, consumer, next});
    auto fenced_batches = sched::build_batches(fenced);
    auto fenced_windows = sched::build_windows(fenced, fenced_batches);

    assertEquals(fenced_batches.size(), size_t(3));
    assertTrue(fenced_windows.at(1).offloaded());
    assertEquals(fenced_windows.at(1).first, size_t(1));
    assertEquals(fenced_windows.at(1).last, size_t(1));

    return true;
}

bool test_worker_pool()
{
    /* No workers: jobs still run, on the submitting thread */
    {
        sched::worker_pool serial(0);
        int                ran = 0;
        serial.submit([&ran]() { ran++; });
        serial.wait_all();
        assertEquals(ran, 1);
    }

    sched::worker_pool pool(3);
    assertEquals(pool.size(), size_t(3));

    std::atomic<int> counter{0};

    for(size_t frame = 0; frame < 32; frame++)
    {
        pool.reset();

        std::vector<sched::worker_pool::job_id> tickets;
        for(size_t job = 0; job < 8; job++)
            tickets.push_back(pool.submit([&counter]() { counter++; }));

        /* Waiting on one job says nothing about the others... */
        pool.wait(tickets.front());
        pool.wait_all();
        /* ...but by the end of the frame every one of them has run, exactly
         * once, and nothing is left over for the next frame */
        assertEquals(counter.load(), static_cast<int>((frame + 1) * 8));
    }

    /* An exception on a worker is contained: it neither escapes the pool nor
     * leaves the frame waiting forever */
    pool.reset();
    auto thrower = pool.submit(
        []() { throw std::runtime_error("subsystem blew up"); });
    pool.wait(thrower);
    pool.wait_all();
    assertEquals(pool.failed(), size_t(1));

    return true;
}

} // namespace

COFFEE_TESTS_BEGIN(5)

    {test_component_projection,
     "Manifest access projection",
     "const in a manifest list makes the proxy hand back const payloads"},
    {test_conflicts,
     "Access set conflicts",
     "shared writes, subsystem dependencies and structural mutation"},
    {test_batching,
     "Frame batching",
     "priority-ordered greedy batching of non-conflicting subsystems"},
    {test_windows,
     "Offload windows",
     "how far an opted-in subsystem may run outside its own batch"},
    {test_worker_pool,
     "Worker pool",
     "jobs run once per frame, are joined within it, and contain throws"},

COFFEE_TESTS_END()
