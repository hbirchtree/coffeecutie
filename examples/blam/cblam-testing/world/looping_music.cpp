#include "looping_music.h"

#if defined(FEATURE_ENABLE_OAF)
namespace world {

bool LoopingMusic::play(oaf::api& api, float gain, buffer_ptr intro_, std::vector<buffer_ptr> loops_)
{
    stop();
    if(loops_.empty())
        return false;
    source = api.alloc_source();
    source->template set_property<oaf::source_property::relative>(true);
    source->template set_property<oaf::source_property::gain>(gain);
    source->spatialize_as(oaf::source_t::never);
    intro = std::move(intro_);
    loops = std::move(loops_);
    if(intro)
        source->queue(*intro);
    else
        source->template set_property<oaf::source_property::looping>(true);
    for(auto const& loop : loops)
        source->queue(*loop);
    return true;
}

void LoopingMusic::update()
{
    if(!source || !intro)
        return;
    auto [queued, processed] = source->buffer_queue();
    if(processed < 1)
        return;
    source->unqueue(*intro);
    intro.reset();
    source->template set_property<oaf::source_property::looping>(true);
}

} // namespace world
#endif
