#include <coffee/CAudio>
#include <coffee/CCore>
#include <coffee/CGraphics>

using namespace Coffee;
using namespace CDisplay;
using namespace CAudio;
using namespace COpenAL;

class CDRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
    CSoundSample<CALSource,CALBuffer>* m_sample;
    CSoundTrack<CALSource,CALBuffer>* m_track;
public:
    CDRenderer()
        : CGLBindingRenderer(0)
    {
    }

    void run()
    {
        CAudioSample smp;
        //Read audio sample from file
        CResources::CResource rsc("caudio_test/monosample.ogg");
        coffee_file_pull(rsc);
        CStbAudio::coffee_stb_audio_vorbis_load(&smp,&rsc);
        coffee_file_free(rsc);

        //Acquire an audio device, create a soundtrack
        CALSoundManager man;
        CSoundDevice<CALSource,CALBuffer>* dev = man.createDevice(man.defaultSoundDevice());
        CSoundMixer<CALSource,CALBuffer>& mixer = dev->mixer();
        uint64 trackId = mixer.createTrack();
        CSoundTrack<CALSource,CALBuffer>& track = mixer.soundtrack(trackId);

        //Set a sample format
        CALSoundFormat fmt;
        fmt.setBitDepth(smp.fmt.bitdepth);
        fmt.setChannels(smp.fmt.channels);
        fmt.setSamplerate(smp.fmt.samplerate);

        //Create an audio buffer with said format
        CSoundBuffer<CALSource,CALBuffer>& buf = dev->genBuffer();
        buf.setFormat(fmt);
        //Fill buffer with audio data
        buf.fillBuffer(smp.data,smp.samples);

        //Create an audio sample
        CSoundSample<CALSource,CALBuffer>& samp = dev->genSample(buf,fmt);
        samp.setPts(0);

        CSoundStream<CALSource,CALBuffer>& strm = dev->genStream(fmt);
        strm.feedData(smp.data,fmt,smp.samples);

        //Free sample data from source
        c_free(smp.data);

        m_track = &track;
        m_sample = &samp;

        //Queue sample for playback
        track.queueSample(samp);

        this->showWindow();
        while(!closeFlag())
        {
            track.updateTrack(5);
            coffee_graphics_clear(CClearFlag::Color);
            this->pollEvents();
            this->swapBuffers();
        }

        delete dev;
    }
    void eventHandleD(const CDisplay::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);
        switch(e.type)
        {
        case CIEvent::Keyboard:
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)data;
            switch(kev->key)
            {
            case CK_Escape:
                this->closeWindow();
                break;
            case CK_Space:
                m_track->queueSample(*m_sample);
                break;
            }
        }
        default:break;
        }
    }
};

int32 coffee_main(int32 argc, byte_t** argv)
{
    CResources::coffee_file_set_resource_prefix("sample_data/");

    CDRendererBase *renderer = new CDRenderer();
    renderer->init(coffee_get_default_visual());
    renderer->run();
    renderer->cleanup();
    delete renderer;
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
