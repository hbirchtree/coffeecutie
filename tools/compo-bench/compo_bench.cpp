/* Micro benchmarks for compo::EntityContainer::select()/match() and
 * stl_types::quick_container.
 *
 * Two dimensions:
 *  - perf:   wall time per full iteration pass over a populated container
 *  - memory: heap allocation count + bytes, split into range construction
 *            (quick_container + entity_query) and the iteration itself
 *
 * The allocation numbers are what motivated the quick_container rewrite:
 * the previous implementation stored two std::function generators per
 * range, allocating on every select() call. The rewrite stores the two
 * iterators by value, so constructing a range must not allocate beyond
 * what entity_query's own predicate needs.
 */

#include <coffee/core/CApplication>
#include <coffee/components/components.h>
#include <coffee/core/CDebug>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <new>

using namespace Coffee;
using libc_types::i32;
using libc_types::u64;

/*
 * Global allocation counters. Every operator new in the process counts,
 * so benchmark sections must not log or touch other allocating machinery
 * while a measurement window is open.
 */
namespace alloc_meter {

static thread_local u64  g_allocs  = 0;
static thread_local u64  g_bytes   = 0;
static thread_local bool g_armed   = false;

struct window
{
    u64 allocs{};
    u64 bytes{};

    window()
    {
        g_allocs = 0;
        g_bytes  = 0;
        g_armed  = true;
    }

    void close()
    {
        g_armed = false;
        allocs  = g_allocs;
        bytes   = g_bytes;
    }
};

} // namespace alloc_meter

void* operator new(std::size_t size)
{
    if(alloc_meter::g_armed)
    {
        ++alloc_meter::g_allocs;
        alloc_meter::g_bytes += size;
    }
    if(void* ptr = std::malloc(size))
        return ptr;
    throw std::bad_alloc();
}

void operator delete(void* ptr) noexcept
{
    std::free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept
{
    std::free(ptr);
}

/* Verbatim copy of the pre-rewrite quick_container: two std::function
 * generators, invoked on every begin()/end(). Kept here so the benchmark
 * can put numbers on the old vs. new implementation side by side. */
template<typename IteratorType>
struct legacy_quick_container
{
    legacy_quick_container(
        std::function<IteratorType()>&& begin_con,
        std::function<IteratorType()>&& end_con)
        : m_begin(begin_con)
        , m_end(end_con)
    {
    }

    IteratorType begin()
    {
        return m_begin();
    }

    IteratorType end()
    {
        return m_end();
    }

  private:
    std::function<IteratorType()> m_begin;
    std::function<IteratorType()> m_end;
};

/* Benchmark components: separate data structs from the component tags so
 * the container template never sees an incomplete type */
struct position_t
{
    f32 x{}, y{}, z{};
};

struct velocity_t
{
    f32 x{}, y{}, z{};
};

struct Position
{
    using value_type = position_t;
    using type       = compo::alloc::VectorContainer<Position>;
};

struct Velocity
{
    using value_type = velocity_t;
    using type       = compo::alloc::VectorContainer<Velocity>;
};

constexpr u64 tag_visible = 0x1;
constexpr u64 tag_static  = 0x2;

/* 1M entities: large enough that iteration is memory-bound and RSS numbers
 * are meaningful; fewer passes to keep the total runtime sane */
constexpr u32 num_entities = 1'000'000;
constexpr u32 num_passes   = 50;

/* Current and peak resident set, from /proc/self/status (Linux) */
struct rss_snapshot
{
    u64 current_kb{};
    u64 peak_kb{};

    static rss_snapshot take()
    {
        rss_snapshot out{};
        if(FILE* f = std::fopen("/proc/self/status", "r"))
        {
            char line[256];
            while(std::fgets(line, sizeof(line), f))
            {
                std::sscanf(line, "VmRSS: %lu kB", &out.current_kb);
                std::sscanf(line, "VmHWM: %lu kB", &out.peak_kb);
            }
            std::fclose(f);
        }
        return out;
    }
};

static compo::EntityContainer& make_container()
{
    static compo::EntityContainer container;
    container.register_component_inplace<Position>();
    container.register_component_inplace<Velocity>();

    compo::EntityRecipe pos_only{
        .components = {compo::type_hash_v<Position>()},
        .tags       = tag_visible,
    };
    compo::EntityRecipe pos_vel{
        .components =
            {compo::type_hash_v<Position>(), compo::type_hash_v<Velocity>()},
        .tags = tag_visible | tag_static,
    };

    auto                before = rss_snapshot::take();
    alloc_meter::window mem;

    /* Every entity gets Position, every other entity gets Velocity too,
     * so select<Velocity>() has to skip half the population */
    for(u32 i = 0; i < num_entities; ++i)
        container.create_entity((i % 2) == 0 ? pos_vel : pos_only);

    mem.close();
    auto after = rss_snapshot::take();

    cBasicPrint(
        "population: {0} entities, {1} allocs / {2} MB heap, RSS {3} -> {4} "
        "MB ({5} B/entity)",
        num_entities,
        mem.allocs,
        mem.bytes / (1024.0 * 1024.0),
        before.current_kb / 1024.0,
        after.current_kb / 1024.0,
        (after.current_kb - before.current_kb) * 1024.0 / num_entities);

    return container;
}

template<typename Fn>
static void bench(const char* name, Fn&& pass)
{
    using clock = std::chrono::steady_clock;

    /* Memory: one range construction + one pass, measured separately from
     * timing so the counters don't distort the numbers */
    alloc_meter::window mem;
    u64                 visited = pass();
    mem.close();

    /* Perf: warm cache with the pass above, then time. RSS around the
     * timed loop shows whether iterating grows the working set at all. */
    auto rss_before = rss_snapshot::take();
    auto t0         = clock::now();
    for(u32 i = 0; i < num_passes; ++i)
        visited += pass();
    auto t1        = clock::now();
    auto rss_after = rss_snapshot::take();

    auto total_ns =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
    auto per_pass_us = f64(total_ns) / num_passes / 1000.0;
    auto per_ent_ns  = f64(total_ns) / num_passes / num_entities;

    if(auto rss_delta = rss_after.current_kb - rss_before.current_kb;
       rss_delta != 0)
        cBasicPrint("{0}: RSS grew {1} kB during iteration!", name, rss_delta);

    cBasicPrint(
        "{0}: {1} us/pass, {2} ns/entity-slot, {3} allocs / {4} B per pass "
        "(visited={5})",
        name,
        per_pass_us,
        per_ent_ns,
        mem.allocs,
        mem.bytes,
        visited / (num_passes + 1));
}

i32 bench_main(i32, cstring_w*)
{
    auto& container = make_container();

    /* Self-check: the operator new override must see this allocation,
     * otherwise every memory number below is silently meaningless.
     * Direct operator-new call: new-*expressions* may legally be elided
     * by the optimizer (and are, in release), direct calls may not. */
    {
        alloc_meter::window mem;
        void*               probe = ::operator new(4096);
        asm volatile("" : : "r"(probe) : "memory");
        ::operator delete(probe);
        mem.close();
        if(mem.allocs != 1 || mem.bytes < 4096)
        {
            cBasicPrint(
                "alloc meter broken (saw {0} allocs / {1} B), aborting",
                mem.allocs,
                mem.bytes);
            return 1;
        }
    }

    cBasicPrint(
        "compo-bench: {0} entities, {1} passes per benchmark",
        num_entities,
        num_passes);
    cBasicPrint(
        "sizeof: entity_query={0}, quick_container={1} (new), "
        "legacy_quick_container={2} (old, + possible heap per function)",
        sizeof(compo::EntityContainer::entity_query),
        sizeof(stl_types::quick_container<
               compo::EntityContainer::entity_query>),
        sizeof(legacy_quick_container<
               compo::EntityContainer::entity_query>));

    bench("select<Position>            (all)", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select<Position>())
            n += ent.id() != 0;
        return n;
    });

    bench("select<Velocity>           (half)", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select<Velocity>())
            n += ent.id() != 0;
        return n;
    });

    bench("select(tags)            (visible)", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select(tag_visible))
            n += ent.id() != 0;
        return n;
    });

    bench("select(tags)             (static)", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select(tag_static))
            n += ent.id() != 0;
        return n;
    });

    bench("select<Velocity> + get<Position>", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select<Velocity>())
            n += container.get<Position>(ent.id()) != nullptr;
        return n;
    });

    /* Variadic select: both payloads prefetched by the iterator, get<T>()
     * is a cached pointer dereference */
    bench("select<Velocity, Position> (fused)", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select<Velocity, Position>())
        {
            n += ent.get<Velocity>().x == 0.f;
            n += ent.get<Position>().x == 0.f;
        }
        return n / 2;
    });

    /* Same, consumed through the components() tuple + structured binding */
    bench("select<V, P> components() binding", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select<Velocity, Position>())
        {
            auto [vel, pos] = ent.components();
            n += vel.x == 0.f;
            n += pos.x == 0.f;
        }
        return n / 2;
    });

    /* Old vs. new quick_container, same entity_query iterator inside.
     * "legacy" reconstructs the pre-rewrite path: a range holding two
     * std::function generators, calling them on begin()/end(). */
    using query_t = compo::EntityContainer::entity_query;

    bench("quick_container      (new, select)", [&container]() -> u64 {
        u64 n = 0;
        for(auto ent : container.select(tag_visible))
            n += ent.id() != 0;
        return n;
    });

    bench("legacy_quick_container (old shape)", [&container]() -> u64 {
        u64  n     = 0;
        auto range = legacy_quick_container<query_t>(
            [&container]() { return query_t(container, tag_visible); },
            [&container]() { return query_t(container); });
        for(auto ent : range)
            n += ent.id() != 0;
        return n;
    });

    /* Range construction alone: how much does calling select() cost when
     * the range is never iterated? This is the path the quick_container
     * rewrite targets — previously 2+ std::function allocations. */
    {
        alloc_meter::window mem;
        auto                range = container.select<Position>();
        (void)range;
        mem.close();
        cBasicPrint(
            "select<Position>  construction only: {0} allocs / {1} B",
            mem.allocs,
            mem.bytes);
    }
    {
        alloc_meter::window mem;
        auto                range = container.select(tag_visible);
        (void)range;
        mem.close();
        cBasicPrint(
            "select(tags)      construction only: {0} allocs / {1} B",
            mem.allocs,
            mem.bytes);
    }
    {
        alloc_meter::window mem;
        auto                range = legacy_quick_container<query_t>(
            [&container]() { return query_t(container, tag_visible); },
            [&container]() { return query_t(container); });
        (void)range;
        mem.close();
        cBasicPrint(
            "legacy(tags)      construction only: {0} allocs / {1} B",
            mem.allocs,
            mem.bytes);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN_CUSTOM(bench_main, 0x1 | 0x2)
