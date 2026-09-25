#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <coffee/components/components.h>
#include <coffee/components/scheduling.h>
#include <coffee/components/worker_pool.h>

#include <atomic>
#include <stdexcept>
#include <string>
#include <vector>

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

/* Position is writable, Velocity read-only; Clock writable, Journal not.
 * The compile-time side of this is checked in allocators.cpp */
using MixedManifest = SubsystemManifest<
    type_list_t<Position, const Velocity>,
    type_list_t<Clock, const Journal>,
    empty_list_t>;

using MixedProxy = proxy_of<MixedManifest>;

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

} // namespace

TEST_CASE("Manifest access projection")
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
    CHECK_EQ(seen, 4);

    /* Tag queries yield refs routed through the proxy, not the container */
    size_t tagged = 0;
    for(auto entity : proxy.select(0x1))
    {
        entity.get<Position>().x += 1.f;
        tagged++;
    }
    CHECK_EQ(tagged, 4);
    CHECK_EQ(proxy.get<Position>(1)->x, 3.f);

    proxy.subsystem<Clock>().ticks = 11;
    CHECK_EQ(proxy.subsystem<Clock>().ticks, 11);
}

TEST_CASE("Access set conflicts")
{
    auto write_pos = set_of(10, {writes(pos_h)});
    auto read_pos  = set_of(11, {reads(pos_h)});
    auto read_pos2 = set_of(12, {reads(pos_h)});
    auto write_vel = set_of(13, {writes(vel_h)});

    /* Concurrent reads of the same component are fine */
    CHECK_FALSE(sched::conflicts(read_pos, read_pos2));
    /* A writer excludes both readers and other writers */
    CHECK(sched::conflicts(write_pos, read_pos));
    CHECK(sched::conflicts(write_pos, write_pos));
    /* Disjoint components never conflict */
    CHECK_FALSE(sched::conflicts(write_pos, write_vel));

    /* A subsystem is a resource: its own frame hooks write it */
    auto depends_on_10 = set_of(14, {}, {reads(10)});
    CHECK(sched::conflicts(write_pos, depends_on_10));

    /* Structural mutation invalidates every query, so it excludes all */
    auto mutator                = set_of(15, {});
    mutator.structural_mutation = true;
    CHECK(sched::conflicts(mutator, write_vel));
    CHECK(sched::conflicts(mutator, read_pos));

    auto state_only     = set_of(16, {});
    state_only.has_work = false;
    CHECK_FALSE(sched::conflicts(mutator, state_only));
    CHECK_FALSE(sched::conflicts(state_only, write_pos));

    auto queried        = set_of(17, {});
    queried.self_access = access::mode::read;

    auto asks   = set_of(18, {}, {reads(17)});
    auto drives = set_of(19, {}, {writes(17)});

    CHECK_FALSE(sched::conflicts(asks, queried));
    CHECK(sched::conflicts(drives, queried));

    auto working = set_of(20, {});
    CHECK(sched::conflicts(set_of(21, {}, {reads(20)}), working));
}

TEST_CASE("Frame batching")
{
    SUBCASE("disjoint readers merge, a writer of the same component splits")
    {
        auto nodes = nodes_of(
            {set_of(10, {reads(pos_h)}),
             set_of(11, {reads(pos_h)}),
             set_of(12, {writes(pos_h)}),
             set_of(13, {writes(vel_h)})});
        auto batches = sched::build_batches(nodes);

        REQUIRE_EQ(batches.size(), 2);
        CHECK_EQ(batches.at(0).members.size(), 2);
        CHECK_EQ(batches.at(1).members.size(), 2);

        /* Priority order is never inverted: a conflicting pair keeps its
         * relative order across the batch boundary */
        size_t previous = 0;
        for(auto const& batch : batches)
            for(auto member : batch.members)
            {
                CHECK(member >= previous);
                previous = member;
            }
    }

    SUBCASE("a structural mutator gets a stage to itself")
    {
        auto mutator                = set_of(11, {reads(vel_h)});
        mutator.structural_mutation = true;

        auto mixed = nodes_of(
            {set_of(10, {reads(pos_h)}), mutator, set_of(12, {reads(pos_h)})});
        auto isolated = sched::build_batches(mixed);

        REQUIRE_EQ(isolated.size(), 3);
        CHECK_EQ(isolated.at(1).members.size(), 1);
    }

    SUBCASE("width counts unpinned members plus one main thread")
    {
        auto pinned_a        = set_of(10, {reads(pos_h)});
        auto pinned_b        = set_of(11, {reads(pos_h)});
        pinned_a.main_thread = true;
        pinned_b.main_thread = true;

        auto pinned_nodes =
            nodes_of({pinned_a, pinned_b, set_of(12, {reads(pos_h)})});
        auto pinned = sched::build_batches(pinned_nodes);

        REQUIRE_EQ(pinned.size(), 1);
        CHECK_EQ(pinned.at(0).width, 2);
    }
}

TEST_CASE("Offload windows")
{
    SUBCASE("opting in is not enough on its own")
    {
        /* Undeclared access, structural mutation and a main thread pin all
         * keep a subsystem where it is */
        auto opted_in     = set_of(10, {reads(pos_h)});
        opted_in.parallel = true;
        CHECK(sched::can_offload(opted_in));

        auto escaped   = opted_in;
        escaped.opaque = true;
        CHECK_FALSE(sched::can_offload(escaped));

        auto pinned        = opted_in;
        pinned.main_thread = true;
        CHECK_FALSE(sched::can_offload(pinned));
    }

    SUBCASE("a subsystem that conflicts with nobody covers the whole frame")
    {
        auto writer_a = set_of(11, {writes(pos_h)});
        auto writer_b = set_of(12, {writes(pos_h)});
        auto late     = set_of(13, {reads(vel_h)});
        late.parallel = true;

        auto nodes   = nodes_of({writer_a, writer_b, late});
        auto batches = sched::build_batches(nodes);
        auto windows = sched::build_windows(nodes, batches);

        REQUIRE_EQ(batches.size(), 2);
        REQUIRE_EQ(windows.size(), 3);
        CHECK(windows.at(2).offloaded());
        CHECK_EQ(windows.at(2).first, 0);
        CHECK_EQ(windows.at(2).last, 1);
        /* ...while the ones that stayed behind are not offloaded at all */
        CHECK_FALSE(windows.at(0).offloaded());
        CHECK_FALSE(windows.at(1).offloaded());
    }

    SUBCASE("a conflict on either side closes the window to its own batch")
    {
        /* The data dependency case: it has to hold inside the frame, not
         * just at its edges */
        auto producer     = set_of(20, {writes(pos_h)});
        auto consumer     = set_of(21, {reads(pos_h)});
        consumer.parallel = true;
        auto next         = set_of(22, {writes(pos_h)});

        auto fenced         = nodes_of({producer, consumer, next});
        auto fenced_batches = sched::build_batches(fenced);
        auto fenced_windows = sched::build_windows(fenced, fenced_batches);

        REQUIRE_EQ(fenced_batches.size(), 3);
        REQUIRE_EQ(fenced_windows.size(), 3);
        CHECK(fenced_windows.at(1).offloaded());
        CHECK_EQ(fenced_windows.at(1).first, 1);
        CHECK_EQ(fenced_windows.at(1).last, 1);
    }
}

TEST_CASE("Worker pool")
{
    SUBCASE("no workers: jobs still run, on the submitting thread")
    {
        sched::worker_pool serial(0);
        int                ran = 0;
        serial.submit([&ran]() { ran++; });
        serial.wait_all();
        CHECK_EQ(ran, 1);
    }

    SUBCASE("jobs run exactly once per frame and are joined within it")
    {
        sched::worker_pool pool(3);
        REQUIRE_EQ(pool.size(), 3);

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
            /* ...but by the end of the frame every one of them has run,
             * exactly once, and nothing is left over for the next frame */
            CHECK_EQ(counter.load(), static_cast<int>((frame + 1) * 8));
        }
    }

    SUBCASE("a throw on a worker is contained")
    {
        /* It neither escapes the pool nor leaves the frame waiting forever */
        sched::worker_pool pool(3);
        pool.reset();
        auto thrower = pool.submit(
            []() { throw std::runtime_error("subsystem blew up"); });
        pool.wait(thrower);
        pool.wait_all();
        CHECK_EQ(pool.failed(), 1);
    }
}
