#pragma once
#include "task.h"
#include <coffee/core/CMath>

namespace Coffee {

enum class LerpType
{
    /* These functions are described for 1 -> 0 */
    /* For 0 -> 1, some functions are simply 1-f(x) */

    /* Reference function:
     *   x
     * Surprise!*/
    Linear = 1,
    /* Reference function:
     *   1-x^2 */
    Exponential,

    /* Reference function:
     *   max(cos(1.6x) * sin(17.2x + pi*0.5) * cos(x*0.9),0)
     * + max(cos(1.6x) * sin(17.0x + pi*1.5),0) */
    BounceIn,

    /* Reference function:
     *   1-((x - 0.5)^3) * 4 - 0.5 */
    Cubic,
};

using LerpFunction = scalar (*)(scalar v, scalar target, scalar t);

namespace Lerp {

FORCEDINLINE scalar Linear(scalar v, scalar target, scalar t)
{
    return v + (target - v) * t;
}

FORCEDINLINE scalar Exponential(scalar v, scalar target, scalar t)
{
    return v + (target - v) * CMath::pow(t, 2.f);
}

FORCEDINLINE scalar Cubed(scalar v, scalar target, scalar t)
{
    return v + (target - v) * CMath::pow(t, 3.f);
}

FORCEDINLINE scalar Bounce(scalar v, scalar target, scalar t)
{
    using namespace CMath;

    bigscalar t_ = C_CAST<bigscalar>(t);

    /* TODO: Remove double conversions here */

    scalar scale =
        CMath::max(
            C_CAST<scalar>(
                CMath::cos(1.6 * t_) * CMath::cos(t_ * 0.9) *
                CMath::sin(17.2 * t_ + CMath::pi * 0.5)),
            0.f) +
        CMath::max(
            C_CAST<scalar>(
                CMath::cos(1.6 * t_) *
                CMath::sin(17.0 * t_ + pi * 1.5)),
            0.f);

    return v + (target - v) * (1.0f - scale);
}

FORCEDINLINE scalar Cubic(scalar v, scalar target, scalar t)
{
    using namespace CMath;

    return v + (target - v) * (CMath::pow(t - 0.5f, 3.0f) * 4.f + 0.5f);
}

FORCEDINLINE scalar Ease(scalar v, scalar target, scalar t)
{
    return v + (target - v) * (CMath::pow(t - 1.f, 3.0f) + 1.0f);
}
} // namespace Lerp

/*!
 * \brief Storage for data related to interpolation, and also input
 */
struct LerpStore
{
    Vector<scalar>
                           source; /*!< Source values, from which interpolation starts */
    Vector<scalar*>        values;  /*!< Output location for values */
    Vector<scalar>         targets; /*!< Target values */
    RuntimeTask::Timepoint start_time;
    RuntimeTask::Timepoint end_time;
    RuntimeTask::Duration  delta;
    scalar                 timescale;
    scalar                 time;
};

template<LerpFunction Func>
/*!
 * \brief Generate a RuntimeTask encapsulating interpolation state
 * \param values Batch of values to be interpolated
 * \param output Output location for values
 * \param targets Batch of target values
 * \param numValues Number of values in batch
 * \param timeStore Storage for the `t` variable used in interpolation
 * \param time How much time it should spend in total
 * \param timeStep How much time between each step,
 *                  could be tied against framerate
 * \param lock For multi-threading, allow exclusive access to values
 * \return
 */
FORCEDINLINE RuntimeTask GenLerpTask(
    LerpStore*                   store,
    RuntimeTask::Duration const& time,
    RuntimeTask::Duration const& timeStep = Chrono::milliseconds(10),
    Mutex*                       lock     = nullptr)
{
    /* Why are we here? */
    if(((store->source.size() != store->targets.size()) &&
        (store->source.size() != store->values.size())) ||
       store->source.size() == 0)
        return {};

    /* Initialize state for interpolation */
    store->start_time = RuntimeTask::clock::now();
    store->end_time   = RuntimeTask::clock::now() + time;
    store->delta      = Chrono::milliseconds(0);
    store->time       = 0.f;

    Chrono::duration<scalar> tscale = time;
    /* Used for scaling the delta value for duration */
    store->timescale = 1.f / tscale.count();

    return {
        [store, lock]() {
            if(lock)
                lock->lock();

            auto now  = RuntimeTask::clock::now();
            auto then = store->start_time + store->delta;
            auto diff = now - then;

            /* Calculate a difference, we don't care much
             *  about interruptions, as long as the deadline is met */
            store->time +=
                Chrono::duration<bigscalar>(diff).count() * store->timescale;
            store->delta = now - store->start_time;

            /* Make some temporaries, make for easier debugging */
            auto& time    = store->time;
            auto  values  = store->source.data();
            auto  targets = store->targets.data();
            auto  output  = store->values.data();

            for(szptr i = 0; i < store->source.size(); i++)
                (*output[i]) = Func(values[i], targets[i], time);

            if(RuntimeTask::clock::now() >= store->end_time)
            {
                for(szptr i = 0; i < store->source.size(); i++)
                    (*output[i]) = targets[i];

                runtime_queue_error ec;
                RuntimeQueue::CancelTask(RuntimeQueue::GetSelfId(ec), ec);

                C_ERROR_CHECK(ec);
            }

            if(lock)
                lock->unlock();

        },
        {},
        timeStep,
        RuntimeTask::Periodic,
    };
}

} // namespace Coffee
