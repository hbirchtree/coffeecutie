#include "web_output.h"

#include <coffee/core/debug/formatting.h>
#include <emscripten/em_js.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace oaf::web {

namespace {

/* Both powers of two, so a chunk never straddles the end of the ring */
constexpr u32 chunk_frames = 256;
constexpr u32 ring_frames  = 8192;

// clang-format off
EM_JS(int, oaf_web_open, (), {
    var Context = globalThis.AudioContext || globalThis.webkitAudioContext;
    if(!Context || !globalThis.AudioWorkletNode)
        return 0;
    var ctx = new Context({latencyHint: "interactive"});
    /* Autoplay policy keeps it suspended until the page is interacted with */
    var resume = function() {
        if(ctx.state === "suspended")
            ctx.resume();
    };
    ["pointerdown", "keydown", "touchend"].forEach(function(name) {
        document.addEventListener(name, resume, true);
    });
    Module["oafAudio"] = {ctx: ctx, resume: resume};
    return ctx.sampleRate;
});

EM_JS(void, oaf_web_connect, (float* ring, u32* positions, u32 frames), {
    var audio = Module["oafAudio"];
    /* Runs on the audio thread, copying what the mixer thread queued */
    var processor = function() {
        class OafOutput extends AudioWorkletProcessor {
            constructor(options) {
                super();
                var o = options.processorOptions;
                this.positions = new Uint32Array(o.memory, o.positions, 3);
                this.ring = new Float32Array(o.memory, o.ring, o.frames * 2);
                this.mask = o.frames - 1;
            }
            process(inputs, outputs) {
                var left = outputs[0][0];
                var right = outputs[0][1];
                var written = Atomics.load(this.positions, 0);
                var read = Atomics.load(this.positions, 1);
                var count = Math.min((written - read) >>> 0, left.length);
                for(var i = 0; i < count; i++) {
                    var at = ((read + i) & this.mask) * 2;
                    left[i] = this.ring[at];
                    right[i] = this.ring[at + 1];
                }
                if(count < left.length)
                    Atomics.add(this.positions, 2, 1);
                Atomics.store(this.positions, 1, (read + count) >>> 0);
                return true;
            }
        }
        registerProcessor("oaf-output", OafOutput);
    };
    var url = URL.createObjectURL(new Blob(
        ["(" + processor.toString() + ")();"], {type: "text/javascript"}));
    audio.ctx.audioWorklet.addModule(url).then(function() {
        URL.revokeObjectURL(url);
        if(Module["oafAudio"] !== audio)
            return;
        audio.node = new AudioWorkletNode(audio.ctx, "oaf-output", {
            numberOfInputs: 0,
            outputChannelCount: [2],
            processorOptions: {
                memory: HEAPU8.buffer,
                ring: Number(ring),
                positions: Number(positions),
                frames: frames,
            },
        });
        audio.node.connect(audio.ctx.destination);
    }).catch(function(error) {
        err("Failed to start audio output: " + error);
    });
});

EM_JS(void, oaf_web_close, (), {
    var audio = Module["oafAudio"];
    if(!audio)
        return;
    ["pointerdown", "keydown", "touchend"].forEach(function(name) {
        document.removeEventListener(name, audio.resume, true);
    });
    if(audio.node)
        audio.node.disconnect();
    audio.ctx.close();
    delete Module["oafAudio"];
});
// clang-format on

using render_fn = void (*)(ALCdevice*, ALCvoid*, ALCsizei);

} // namespace

using namespace Coffee::Logging;

struct output_t
{
    ~output_t()
    {
        stop.store(true);
        if(mixer.joinable())
            mixer.join();
        oaf_web_close();
    }

    void mix(ALCdevice* device, render_fn render);

    u32                rate{0};
    std::vector<float> ring = std::vector<float>(ring_frames * 2);
    /* Frames written, frames read (both wrapping) and underruns */
    std::atomic<u32> positions[3]{};
    std::atomic_bool stop{false};
    std::thread      mixer;
};

void output_t::mix(ALCdevice* device, render_fn render)
{
    using namespace std::chrono;

    /* About 40 ms queued ahead of the audio thread */
    u32 const ahead = std::min(
        ring_frames - chunk_frames,
        (rate / 25 + chunk_frames - 1) / chunk_frames * chunk_frames);
    auto const nap = microseconds(500'000 * chunk_frames / rate);

    u32  reported_underruns = 0;
    auto reported_at        = steady_clock::now();
    while(!stop.load(std::memory_order_relaxed))
    {
        u32 const written = positions[0].load(std::memory_order_relaxed);
        u32 const read    = positions[1].load(std::memory_order_acquire);
        if(written - read >= ahead)
        {
            std::this_thread::sleep_for(nap);
            continue;
        }
        render(
            device,
            ring.data() + (written & (ring_frames - 1)) * 2,
            chunk_frames);
        positions[0].store(written + chunk_frames, std::memory_order_release);

        u32 const underruns = positions[2].load(std::memory_order_relaxed);
        if(underruns != reported_underruns &&
           steady_clock::now() - reported_at > seconds(1))
        {
            cWarning("Audio output underruns: {}", underruns);
            reported_underruns = underruns;
            reported_at        = steady_clock::now();
        }
    }
}

std::shared_ptr<output_t> open_output()
{
    int const rate = oaf_web_open();
    if(rate <= 0)
        return nullptr;
    auto output  = std::make_shared<output_t>();
    output->rate = static_cast<u32>(rate);
    return output;
}

u32 sample_rate(output_t const& output)
{
    return output.rate;
}

void start_output(output_t& output, ALCdevice* device)
{
    auto render = reinterpret_cast<render_fn>(
        alcGetProcAddress(device, "alcRenderSamplesSOFT"));
    output.mixer =
        std::thread([&output, device, render] { output.mix(device, render); });
    oaf_web_connect(
        output.ring.data(),
        reinterpret_cast<u32*>(output.positions),
        ring_frames);
}

} // namespace oaf::web
