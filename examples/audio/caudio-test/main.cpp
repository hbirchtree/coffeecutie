#include <coffee/audio/abstraction/openal/copenalabstract.h>
#include <coffee/audio/caudio.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/interfaces/full_launcher.h>
#include <coffee/strings/libc_types.h>
#include <coffee/windowing/renderer/renderer.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Display;
using namespace CAudio;
using namespace Input;

using AL = COpenAL::OpenALAPI;

struct AudioData
{
    AL::Device* device;

    AL::Sample* m_sample_1;
    AL::Sample* m_sample_2;
    AL::Track*  m_track_1;
    AL::Track*  m_track_2;
};

i32 coffee_main(i32, cstring_w*)
{
    RuntimeQueue::CreateNewQueue("Main");

    AutoExec<GLEAMAPI, Display::CSDL2Renderer, AudioData>(
        [](ShPtr<Display::CSDL2Renderer> r,
           ShPtr<AudioData>              data,
           Properties&) {
            WkPtr<AudioData> borrow = data;

            r->installEventHandler(EHandle<CIEvent>::MkFunc<CIKeyEvent>(
                [borrow](CIEvent const& e, CIKeyEvent const* kev) {
                    auto m = borrow.lock();

                    auto& m_track_1  = m->m_track_1;
                    auto& m_track_2  = m->m_track_2;
                    auto& m_sample_1 = m->m_sample_1;
                    auto& m_sample_2 = m->m_sample_2;

                    switch(kev->key)
                    {
                    case CK_Space:
                        m_track_1->queueSample(*m_sample_1);
                        break;
                    case CK_LShift:
                        m_track_2->queueSample(*m_sample_2);
                        break;
                    }
                },
                "Input handler"));
            r->installEventHandler(EHandle<CIEvent>::MkHandler(
                EventHandlers::ExitOn<EventHandlers::OnKey<CK_Escape>>(
                    r->window()),
                "Escape handler"));
            r->installEventHandler(EHandle<CIEvent>::MkHandler(
                EventHandlers::ExitOn<EventHandlers::OnQuit>(r->window()),
                "Close handler"));
        },
        [&](Display::CSDL2Renderer& r, AudioData* d) {
            AudioSample smp;
            // Read audio sample from file
            auto rsc = "healing.ogg"_rsc;
            cDebug("Resource");
            FileMap(rsc);
            cDebug(
                "Mapping file succeeded, size={0},ptr={1}",
                rsc.size,
                (uintptr_t)rsc.data);
            if(!Stb::LoadVorbis(&smp, rsc))
            {
                cFatal("Failed to load audio file: {0}", rsc.resource());
            } else
            {
                cDebug("Loaded audio file");
            }

            // Acquire an audio device, create a soundtrack
            AL::Arbiter man;
            cDebug("Created AL arbiter");
            AL::Device* dev = man.createDevice(man.defaultSoundDevice());
            d->device       = dev;
            if(!dev)
            {
                cDebug(
                    "Failed to create audio device, descriptor={0}",
                    man.defaultSoundDevice().stringIdentifier());
                return;
            } else
            {
                cDebug(
                    "AL device={0}",
                    man.defaultSoundDevice().stringIdentifier());
            }
            AL::Mixer& mixer    = dev->mixer();
            u64        trackId1 = mixer.createTrack();
            u64        trackId2 = mixer.createTrack();
            AL::Track& track1   = mixer.soundtrack(trackId1);
            AL::Track& track2   = mixer.soundtrack(trackId2);

            // Set a sample format
            AL::Format fmt;
            fmt.setBitDepth(smp.fmt.bitdepth);
            fmt.setChannels(smp.fmt.channels);
            fmt.setSamplerate(smp.fmt.samplerate);

            // Create an audio buffer with said format
            AL::Buffer& buf = dev->genBuffer();
            buf.setFormat(fmt);
            // Fill buffer with audio data
            buf.fillBuffer(smp.data, smp.samples);

            // Create an audio sample
            AL::Sample& samp1 = dev->genSample(buf, fmt);
            samp1.setPts(0);

            AL::Properties samp1_props = {};
            samp1_props.looping        = true;
            samp1_props.active |= AL::Properties::Looping;

            samp1.assignProperties(&samp1_props);

            d->m_track_1  = &track1;
            d->m_track_2  = &track2;
            d->m_sample_1 = &samp1;
            d->m_sample_2 = &samp1;

            // Queue sample for playback
            track1.queueSample(samp1);

            r.showWindow();
        },
        [&](Display::CSDL2Renderer& r, AudioData* d) {
            d->m_track_1->updateTrack(5);
            d->m_track_2->updateTrack(5);

            r.pollEvents();
            r.swapBuffers();
        },
        [&](Display::CSDL2Renderer&, AudioData* d) { delete d->device; });

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
