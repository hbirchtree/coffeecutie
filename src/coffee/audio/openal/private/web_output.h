#pragma once

#include <AL/alc.h>
#include <memory>
#include <peripherals/libc/types.h>

namespace oaf::web {

using libc_types::u32;

/* Plays a loopback device through the page's AudioContext */
struct output_t;

/* nullptr when the page has no AudioContext or AudioWorklet */
std::shared_ptr<output_t> open_output();

u32 sample_rate(output_t const& output);

/* Mixes stereo float frames from the device on a thread of its own */
void start_output(output_t& output, ALCdevice* device);

} // namespace oaf::web
