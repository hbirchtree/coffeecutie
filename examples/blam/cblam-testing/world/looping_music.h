#pragma once

#if defined(FEATURE_ENABLE_OAF)
#include <oaf/api.h>

#include <memory>
#include <vector>

namespace world {

/*! Background music as one source: the intro once, then the loops in turn */
struct LoopingMusic
{
    using buffer_ptr = std::shared_ptr<oaf::buffer_t>;

    std::shared_ptr<oaf::source_t> source;
    buffer_ptr                     intro;
    std::vector<buffer_ptr>        loops;

    /*! Starts playing; false, and silent, without any loops */
    bool play(oaf::api& api, float gain, buffer_ptr intro, std::vector<buffer_ptr> loops);

    /*! Once the intro has played the source loops on the rest */
    void update();

    void stop()
    {
        *this = {};
    }
};

} // namespace world
#endif
