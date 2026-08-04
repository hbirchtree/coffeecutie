#pragma once

#include "access.h"
#include "types.h"

#include <string>
#include <vector>

namespace compo::sched {

/*! Everything a subsystem is declared to touch in a frame */
struct access_set
{
    /* Tag hash the subsystem is registered under */
    type_hash self{0};
    std::vector<access::entry> components;
    std::vector<access::entry> subsystems;
    std::vector<access::entry> services;
    /* Creates or removes entities */
    bool structural_mutation{false};
    /* Reached the container outside its manifest */
    bool opaque{false};
    /* Pinned to the main thread (GL contexts, windowing, input) */
    bool main_thread{false};
    bool parallel{false};
    access::mode self_access{access::mode::write};
    bool has_work{true};
};

using access_entry = access::entry;

/*! True when the two cannot share a frame stage */
bool conflicts(access_set const& a, access_set const& b);

struct node
{
    SubsystemBase* subsystem{nullptr};
    access_set     access;
    std::string    name;
    /*! Measured cost of the subsystem's frame hooks, 0 if unmeasured */
    libc_types::u64 cost{0};
};

struct batch
{
    /*! Indices into the node list the batch was built from */
    std::vector<size_t> members;
    size_t width{0};
};

struct window
{
    size_t first{0};
    size_t last{0};

    bool offloaded() const
    {
        return first <= last;
    }
};

/*! Whether the subsystem may leave the main thread at all */
bool can_offload(access_set const& set);

std::vector<window> build_windows(
    std::vector<node> const& priority_sorted,
    std::vector<batch> const& batches);

std::string format_windows(
    std::vector<node> const& priority_sorted,
    std::vector<batch> const& batches,
    std::vector<window> const& windows);

size_t configured_worker_count();

size_t warmup_frames();

/*! Spread main-thread affinity along declared dependencies */
void propagate_main_thread(std::vector<node>& nodes);

/*! Group a priority-sorted subsystem list into stages */
std::vector<batch> build_batches(std::vector<node> const& priority_sorted);

/*! Human-readable schedule, one line per batch plus a summary */
std::string format_batches(
    std::vector<node> const& priority_sorted,
    std::vector<batch> const& batches);

/*! Access set of a subsystem, from the manifest it declared */
access_set access_of(type_hash self, SubsystemBase const& subsystem);

bool access_changed(access_set const& previous, SubsystemBase const& subsystem);

/*! Frame to emit the schedule report on, from COFFEE_ECS_SCHEDULE */
size_t report_frame();

} // namespace compo::sched
