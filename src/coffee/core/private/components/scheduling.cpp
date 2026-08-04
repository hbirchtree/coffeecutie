#include <coffee/components/scheduling.h>

#include <platforms/stacktrace.h>

#include <algorithm>
#include <cstdlib>
#include <string_view>
#include <thread>

namespace compo::sched {

namespace {

access::entry const* find_entry(
    std::vector<access::entry> const& set, type_hash hash)
{
    for(auto const& candidate : set)
        if(candidate.hash == hash)
            return &candidate;
    return nullptr;
}

bool shares_written_resource(
    std::vector<access::entry> const& a, std::vector<access::entry> const& b)
{
    for(auto const& lhs : a)
    {
        auto const* rhs = find_entry(b, lhs.hash);

        if(!rhs)
            continue;

        bool const lhs_writes = access::is_write(lhs.access);
        bool const rhs_writes = access::is_write(rhs->access);

        if(!lhs_writes && !rhs_writes)
            continue;

        /* Buffered storage decouples a producer from a consumer */
        if(lhs.buffered && rhs->buffered && !(lhs_writes && rhs_writes))
            continue;

        return true;
    }
    return false;
}

std::vector<access::entry> systems_of(access_set const& set)
{
    std::vector<access::entry> out = set.subsystems;
    for(auto const& service : set.services)
    {
        auto it = std::find_if(
            out.begin(), out.end(), [&service](access::entry const& e) {
                return e.hash == service.hash;
            });
        if(it == out.end())
            out.push_back(service);
        else if(access::is_write(service.access))
            it->access = access::mode::write;
    }
    return out;
}

bool depends_on(access_set const& dependent, access_set const& target)
{
    if(target.self == 0)
        return false;

    auto const as_subsystem = access::find(dependent.subsystems, target.self);
    auto const as_service   = access::find(dependent.services, target.self);
    if(as_subsystem == access::mode::none && as_service == access::mode::none)
        return false;
    if(access::is_write(as_subsystem) || access::is_write(as_service))
        return true;
    return access::is_write(target.self_access);
}

} // namespace

bool conflicts(access_set const& a, access_set const& b)
{
    if(!a.has_work || !b.has_work)
        return false;
    if(a.structural_mutation || b.structural_mutation || a.opaque || b.opaque)
        return true;
    if(depends_on(a, b) || depends_on(b, a))
        return true;
    return shares_written_resource(a.components, b.components) ||
           shares_written_resource(systems_of(a), systems_of(b));
}

void propagate_main_thread(std::vector<node>& nodes)
{
    bool changed = true;
    while(changed)
    {
        changed = false;
        for(auto& candidate : nodes)
        {
            if(candidate.access.main_thread)
                continue;
            for(auto const& source : nodes)
            {
                if(!source.access.main_thread || source.access.self == 0)
                    continue;
                if(!depends_on(candidate.access, source.access))
                    continue;
                candidate.access.main_thread = true;
                changed                      = true;
                break;
            }
        }
    }
}

namespace {

size_t width_of(std::vector<node> const& nodes, batch const& current)
{
    size_t unpinned = 0;
    bool   pinned   = false;

    for(auto index : current.members)
    {
        if(nodes.at(index).access.main_thread)
            pinned = true;
        else
            unpinned++;
    }

    return unpinned + (pinned ? 1u : 0u);
}

} // namespace

bool can_offload(access_set const& set)
{
    return set.parallel && !set.main_thread && !set.opaque &&
           !set.structural_mutation;
}

std::vector<batch> build_batches(std::vector<node> const& priority_sorted)
{
    std::vector<batch> out;

    for(size_t candidate = 0; candidate < priority_sorted.size(); candidate++)
    {
        auto const& access = priority_sorted.at(candidate).access;
        bool        fits   = !out.empty();
        if(fits)
            for(auto member : out.back().members)
                if(conflicts(priority_sorted.at(member).access, access))
                {
                    fits = false;
                    break;
                }
        if(!fits)
            out.emplace_back();
        out.back().members.push_back(candidate);
    }
    for(auto& current : out)
        current.width = width_of(priority_sorted, current);
    return out;
}

std::vector<window> build_windows(
    std::vector<node> const& priority_sorted, std::vector<batch> const& batches)
{
    /* An empty window, first past last, means "not offloaded" */
    std::vector<window> out(
        priority_sorted.size(), window{.first = 1, .last = 0});

    std::vector<size_t> batch_of(priority_sorted.size(), 0);
    for(size_t index = 0; index < batches.size(); index++)
        for(auto member : batches.at(index).members)
            batch_of.at(member) = index;

    for(size_t candidate = 0; candidate < priority_sorted.size(); candidate++)
    {
        auto const& access = priority_sorted.at(candidate).access;

        if(!can_offload(access))
            continue;

        size_t const home = batch_of.at(candidate);
        window       span{.first = home, .last = home};

        /* Back up until the batch after the last conflicting predecessor */
        for(size_t earlier = candidate; earlier-- > 0;)
        {
            if(conflicts(priority_sorted.at(earlier).access, access))
            {
                span.first = batch_of.at(earlier) + 1;
                break;
            }
            span.first = std::min(span.first, batch_of.at(earlier));
        }

        /* ...and run on until the batch before the first conflicting one */
        span.last = batches.empty() ? home : batches.size() - 1;
        for(size_t later = candidate + 1; later < priority_sorted.size();
            later++)
        {
            if(conflicts(priority_sorted.at(later).access, access))
            {
                span.last = batch_of.at(later) - 1;
                break;
            }
        }

        /* Conflicts on both sides can meet inside the home batch: the
         * subsystem is offloaded, it just cannot overlap anything */
        span.first = std::min(span.first, home);
        span.last  = std::max(span.last, home);

        out.at(candidate) = span;
    }

    return out;
}

std::string format_windows(
    std::vector<node> const&   priority_sorted,
    std::vector<batch> const&  batches,
    std::vector<window> const& windows)
{
    std::string out;

    std::vector<size_t> batch_of(priority_sorted.size(), 0);
    for(size_t index = 0; index < batches.size(); index++)
        for(auto member : batches.at(index).members)
            batch_of.at(member) = index;

    for(size_t candidate = 0; candidate < windows.size(); candidate++)
    {
        auto const& span = windows.at(candidate);

        if(!span.offloaded())
            continue;

        out += "    " + priority_sorted.at(candidate).name + ": batch " +
               std::to_string(batch_of.at(candidate)) + ", runs across " +
               std::to_string(span.first) + ".." + std::to_string(span.last) +
               "\n";
    }

    if(out.empty())
        return "  no subsystem opted in to running off the main thread\n";

    return "  offloaded subsystems:\n" + out;
}

size_t configured_worker_count()
{
    static const size_t count = []() -> size_t {
        auto const* value = std::getenv("COFFEE_ECS_THREADS");
        if(!value)
            return 0;
        if(std::string_view(value) == "auto")
        {
            auto available = std::thread::hardware_concurrency();
            return available > 2 ? available - 1u : 1u;
        }
        auto parsed = std::atoll(value);
        return parsed > 0 ? static_cast<size_t>(parsed) : 0;
    }();
    return count;
}

size_t warmup_frames()
{
    static const size_t frames = []() -> size_t {
        auto const* value = std::getenv("COFFEE_ECS_WARMUP");
        if(!value)
            return 2;
        auto parsed = std::atoll(value);
        return parsed > 0 ? static_cast<size_t>(parsed) : 0;
    }();
    return frames;
}

std::string format_batches(
    std::vector<node> const& priority_sorted, std::vector<batch> const& batches)
{
    std::string out;
    size_t      peak = 0;
    size_t      idx  = 0;

    for(auto const& current : batches)
    {
        peak = std::max(peak, current.width);

        out += "  batch " + std::to_string(idx++);
        out += current.width > 1 ? " ||  " : " --  ";

        for(size_t i = 0; i < current.members.size(); i++)
        {
            auto const& member = priority_sorted.at(current.members.at(i));
            out += (i ? ", " : "") + member.name;
            if(!member.access.has_work)
            {
                out += "~";
                continue;
            }
            if(member.access.main_thread)
                out += "*";
            if(member.access.structural_mutation)
                out += "!";
            if(member.access.opaque)
                out += "?";
            if(can_offload(member.access))
                out += "+";
        }
        out += "\n";
    }
    out += "  " + std::to_string(priority_sorted.size()) +
           " subsystems: " + std::to_string(priority_sorted.size()) +
           " serial stages -> " + std::to_string(batches.size()) +
           " batched stages, widest " + std::to_string(peak) + "\n" +
           "  * pinned to main thread, ! mutates entity structure, "
           "? undeclared access, + runs on a worker, ~ state only (no hooks), "
           "|| more than one member can run at once\n";

    libc_types::u64 serial = 0;
    libc_types::u64 walled = 0;

    for(auto const& current : batches)
    {
        libc_types::u64 pinned   = 0;
        libc_types::u64 slowest  = 0;

        for(auto index : current.members)
        {
            auto const& member = priority_sorted.at(index);
            serial += member.cost;

            if(member.access.main_thread)
                pinned += member.cost;
            else
                slowest = std::max(slowest, member.cost);
        }

        walled += std::max(pinned, slowest);
    }

    if(serial)
    {
        out += "  measured: " + std::to_string(serial / 1000000) +
               "ms serial -> " + std::to_string(walled / 1000000) +
               "ms if every batch ran in parallel (" +
               std::to_string((serial * 100) / std::max<libc_types::u64>(walled, 1)) +
               "% of serial, upper bound, zero sync cost)\n";

        /* Ranked, because a 17-wide batch of subsystems that do nothing is
         * worth less than a 2-wide batch of ones that do */
        std::vector<size_t> ranked(priority_sorted.size());
        for(size_t i = 0; i < ranked.size(); i++)
            ranked[i] = i;
        std::sort(
            ranked.begin(), ranked.end(), [&](size_t a, size_t b) {
                return priority_sorted.at(a).cost > priority_sorted.at(b).cost;
            });

        out += "  costliest subsystems:\n";
        for(size_t i = 0; i < std::min<size_t>(8, ranked.size()); i++)
        {
            auto const& member = priority_sorted.at(ranked.at(i));
            if(!member.cost)
                break;
            out += "    " + std::to_string(member.cost * 100 / serial) + "% " +
                   std::to_string(member.cost / 1000000) + "ms  " +
                   member.name + "\n";
        }
    }

    return out;
}

size_t report_frame()
{
    static const size_t frame = []() -> size_t {
        auto const* value = std::getenv("COFFEE_ECS_SCHEDULE");
        if(!value)
            return 0;
        auto parsed = std::atoll(value);
        return parsed > 1 ? static_cast<size_t>(parsed) : 300;
    }();
    return frame;
}

access_set access_of(type_hash self, SubsystemBase const& subsystem)
{
    access_set out;

    out.self                = self;
    out.components          = subsystem.declared_components();
    out.subsystems          = subsystem.declared_subsystems();
    out.services            = subsystem.declared_services();
    out.main_thread         = subsystem.main_thread_only();
    out.parallel            = subsystem.parallel_safe();
    out.self_access         = subsystem.self_access();
    out.has_work            = subsystem.has_frame_work();
    out.structural_mutation = subsystem.runtime_access.structural_mutation;
    /* No manifest means nothing is known, not that nothing is touched */
    out.opaque =
        subsystem.runtime_access.opaque || !subsystem.declares_access();

    return out;
}

bool access_changed(access_set const& previous, SubsystemBase const& subsystem)
{
    return previous.has_work != subsystem.has_frame_work() ||
           previous.self_access != subsystem.self_access() ||
           previous.opaque !=
               (subsystem.runtime_access.opaque ||
                !subsystem.declares_access()) ||
           previous.structural_mutation !=
               subsystem.runtime_access.structural_mutation ||
           previous.parallel != subsystem.parallel_safe();
}

} // namespace compo::sched
