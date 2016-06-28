#include <coffee/core/CApplication>
#include <coffee/core/CInput>
#include <coffee/core/CDebug>

#include <coffee/CAudio>

#include <coffee/sdl2/CSDL2WindowHost>

using namespace Coffee;
using namespace Display;
using namespace CAudio;

using AL = COpenAL::OpenALAPI;

class CDRenderer : public Coffee::Display::SDL2WindowHost
{
    AL::Sample* m_sample_1;
    AL::Sample* m_sample_2;
    AL::Track* m_track_1;
    AL::Track* m_track_2;

public:
    CDRenderer()
    {
    }

    void run()
    {
        AudioSample smp;
        //Read audio sample from file
        CResources::Resource rsc("healing.ogg",false,
                                 ResourceAccess::SpecifyStorage
                                 |ResourceAccess::AssetFile);
        cDebug("Resource");
        CResources::FileMap(rsc);
        cDebug("Mapping file succeeded, size={0},ptr={1}",rsc.size,(uintptr_t)rsc.data);
        if(!CStbAudio::LoadVorbis(&smp,&rsc))
        {
            cDebug("Failed to load audio file: {0}",rsc.resource());
            return;
        }else{
            cDebug("Loaded audio file");
        }
        CResources::FileUnmap(rsc);
        cDebug("Unmapping file");

        //Acquire an audio device, create a soundtrack
        AL::Arbiter man;
        cDebug("Created AL arbiter");
        AL::Device* dev = man.createDevice(man.defaultSoundDevice());
        if(!dev)
        {
            cDebug("Failed to create audio device, descriptor={0}",
                   man.defaultSoundDevice().stringIdentifier());
            return;
        }else{
            cDebug("AL device={0}",
                   man.defaultSoundDevice().stringIdentifier());
        }
        AL::Mixer& mixer = dev->mixer();
        uint64 trackId1 = mixer.createTrack();
        uint64 trackId2 = mixer.createTrack();
        AL::Track& track1 = mixer.soundtrack(trackId1);
        AL::Track& track2 = mixer.soundtrack(trackId2);

        //Set a sample format
        AL::Format fmt;
        fmt.setBitDepth(smp.fmt.bitdepth);
        fmt.setChannels(smp.fmt.channels);
        fmt.setSamplerate(smp.fmt.samplerate);

        //Create an audio buffer with said format
        AL::Buffer& buf = dev->genBuffer();
        buf.setFormat(fmt);
        //Fill buffer with audio data
        buf.fillBuffer(smp.data,smp.samples);

        //Create an audio sample
        AL::Sample& samp1 = dev->genSample(buf,fmt);
        samp1.setPts(0);

        //Free sample data from source
        CFree(smp.data);

        m_track_1 = &track1;
        m_track_2 = &track2;
        m_sample_1 = &samp1;
        m_sample_2 = &samp1;

        //Queue sample for playback
        track1.queueSample(samp1);

        this->showWindow();
        while(!closeFlag())
        {
            track1.updateTrack(5);
            track2.updateTrack(5);
            this->pollEvents();
        }

        delete dev;
    }
    void eventHandleD(const Display::CDEvent &e, c_cptr data)
    {
        SDL2WindowHost::eventHandleD(e,data);
        EventHandlers::WindowManagerCloseWindow(this,e,data);
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        SDL2WindowHost::eventHandleI(e,data);
        EventHandlers::EscapeCloseWindow(this,e,data);

        switch(e.type)
        {
        case CIEvent::Keyboard:
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)data;
            switch(kev->key)
            {
            case CK_Space:
                m_track_1->queueSample(*m_sample_1);
                break;
            case CK_LShift:
                m_track_2->queueSample(*m_sample_2);
                break;
            }
        }
        case CIEvent::MouseButton:
        {
            const CIMouseButtonEvent& mev = *(const CIMouseButtonEvent*)data;
            if(mev.mod&CIMouseButtonEvent::Pressed)
            {
                m_track_1->queueSample(*m_sample_1);
            }else{
                m_track_2->queueSample(*m_sample_2);
            }
            break;
        }
        default:
            break;
        }
    }
};

int32 coffee_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/caudio_test/");

    CDRendererBase *renderer = new CDRenderer();
    CString err;
    if(!renderer->init(GetDefaultVisual(),&err))
    {
        cDebug("Initialization error: {0}",err);
        return 1;
    }
    renderer->run();
    renderer->cleanup();
    delete renderer;
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
