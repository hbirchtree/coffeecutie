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

template<
    typename PodType = f32,
    typename std::enable_if<
        std::is_floating_point<PodType>::value>::type* = nullptr>
struct Lerp
{
    STATICINLINE PodType Linear(PodType v, PodType target, PodType t)
    {
        return v + (target - v) * t;
    }

    STATICINLINE PodType Exponential(PodType v, PodType target, PodType t)
    {
        return v + (target - v) * std::pow(t, 2.f);
    }

    STATICINLINE PodType Cubed(PodType v, PodType target, PodType t)
    {
        return v + (target - v) * std::pow(t, 3.f);
    }

    STATICINLINE PodType Bounce(PodType v, PodType target, PodType t)
    {
        constexpr auto pi = stl_types::math::pi;
        f64 t_ = C_CAST<f64>(t);

        /* TODO: Remove double conversions here */

        PodType scale
            = std::max(
                  C_CAST<PodType>(
                      std::cos(1.6 * t_) * std::cos(t_ * 0.9)
                      * std::sin(17.2 * t_ + pi * 0.5)),
                  0.f)
              + std::max(
                  C_CAST<f32>(
                      std::cos(1.6 * t_) * std::sin(17.0 * t_ + pi * 1.5)),
                  0.f);

        return v + (target - v) * (1.0f - scale);
    }

    STATICINLINE PodType Cubic(PodType v, PodType target, PodType t)
    {
        return v + (target - v) * (std::pow(t - 0.5f, 3.0f) * 4.f + 0.5f);
    }

    template<
        unsigned int Curvature = 3,
        typename std::enable_if<
            (Curvature >= 3) && (Curvature % 2 == 1)>::type* = nullptr>
    STATICINLINE PodType Ease(PodType v, PodType target, PodType t)
    {
        constexpr auto curv = PodType(Curvature);

        return v + (target - v) * (std::pow(t - 1.f, curv) + 1.0f);
    }
};

template<typename PrimitiveType>
/*!
 * \brief Storage for data related to interpolation, and also input
 */
struct LerpStore
{
    std::vector<PrimitiveType>  source;  /*!< Source values  */
    std::vector<PrimitiveType*> values;  /*!< Output location for values */
    std::vector<PrimitiveType>  targets; /*!< Target values */

    rq::detail::time_point start_time;
    rq::detail::time_point end_time;
    rq::detail::duration   delta;
    f32                    timescale;
    f32                    time;

    std::function<void()> end_trigger;
};

template<
    typename PrimitiveType,
    PrimitiveType (*Func)(PrimitiveType, PrimitiveType, scalar)

    >
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
FORCEDINLINE rq::runtime_task GenLerpTask(
    std::shared_ptr<LerpStore<PrimitiveType>> store,
    rq::detail::duration const&               time,
    rq::detail::duration const&       timeStep = std::chrono::milliseconds(10),
    std::shared_ptr<stl_types::Mutex> lock     = nullptr)
{
    /* Why are we here? */
    if(((store->source.size() != store->targets.size())
        && (store->source.size() != store->values.size()))
       || store->source.size() == 0)
        return {};

    /* Initialize state for interpolation */
    store->start_time = rq::detail::clock::now();
    store->end_time   = rq::detail::clock::now() + time;
    store->delta      = std::chrono::milliseconds(0);
    store->time       = 0.f;

    std::chrono::duration<f32> tscale = time;
    /* Used for scaling the delta value for duration */
    store->timescale = 1.f / tscale.count();

    return {
        [store, lock]() {
            if(lock)
                lock->lock();

            auto now  = rq::detail::clock::now();
            auto then = store->start_time + store->delta;
            auto diff = now - then;

            /* Calculate a difference, we don't care much
             *  about interruptions, as long as the deadline is met */
            store->time += std::chrono::duration<bigscalar>(diff).count()
                           * store->timescale;
            store->delta = now - store->start_time;

            /* Make some temporaries, make for easier debugging */
            auto& time    = store->time;
            auto  values  = store->source.data();
            auto  targets = store->targets.data();
            auto  output  = store->values.data();

            for(szptr i = 0; i < store->source.size(); i++)
                (*output[i]) = Func(values[i], targets[i], time);

            if(rq::detail::clock::now() >= store->end_time)
            {
                for(szptr i = 0; i < store->source.size(); i++)
                    (*output[i]) = targets[i];

                rq::runtime_queue::CancelTask(
                    rq::runtime_queue::GetSelfId().assume_value());

                if(store->end_trigger)
                    store->end_trigger();
            }

            if(lock)
                lock->unlock();
        },
        {},
        timeStep,
        rq::task_flags::periodic,
    };
}

} // namespace Coffee
