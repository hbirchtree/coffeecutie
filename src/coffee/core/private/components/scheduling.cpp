#include <coffee/components/scheduling.h>

#include <platforms/stacktrace.h>

#include <algorithm>
#include <cstdlib>

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
    return access::find(dependent.subsystems, target.self) !=
               access::mode::none ||
           access::find(dependent.services, target.self) != access::mode::none;
}

} // namespace

bool conflicts(access_set const& a, access_set const& b)
{
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
            if(member.access.main_thread)
                out += "*";
            if(member.access.structural_mutation)
                out += "!";
            if(member.access.opaque)
                out += "?";
        }
        out += "\n";
    }
    out += "  " + std::to_string(priority_sorted.size()) +
           " subsystems: " + std::to_string(priority_sorted.size()) +
           " serial stages -> " + std::to_string(batches.size()) +
           " batched stages, widest " + std::to_string(peak) + "\n" +
           "  * pinned to main thread, ! mutates entity structure, "
           "? undeclared access, || more than one member can run at once\n";

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
    out.structural_mutation = subsystem.runtime_access.structural_mutation;
    out.opaque              = subsystem.runtime_access.opaque;

    return out;
}

} // namespace compo::sched
