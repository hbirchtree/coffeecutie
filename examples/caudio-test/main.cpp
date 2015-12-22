#include <coffee/CAudio>
#include <coffee/CCore>
#include <coffee/CGraphics>

using namespace Coffee;
using namespace CDisplay;
using namespace CAudio;
using namespace COpenAL;

class CDRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
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

        /*
//        CALBuffer *buf = new CALBuffer;
//        CALSource src;
//        CALListener l;

//        //Create the context, acquire possible errors
//        CALContext* ctxt = coffee_audio_context_create();

//        coffee_audio_context_get_error(ctxt);

//        //Set error callback in case of errors
//        coffee_audio_context_set_debug_callback(ctxt,[](CALReport* r){
//            cDebug("{0}",r->message);
//        });

//        //Create a capture device for testing
//        CALCaptureDevice* cdev;
//        {
//            uint32 idevn;
//            cstring* idevs = coffee_audio_context_devices_input(&idevn);

//            CAudioFormat cfmt;
//            cfmt.bitdepth = 16;
//            cfmt.channels = 1;
//            cfmt.samplerate = 44100;
//            cdev = coffee_audio_capture_create(ctxt,idevs[0],cfmt,cfmt.samplerate*4*2);
//        }

//        //Set distance model for context
//        coffee_audio_context_set_distance_model(CDistanceModel::LinearDistanceClamped);

//        {
//            //Set listener properties
//            l.gain = 1;
//            l.position = CVec3(0,0,0);
//            l.velocity = CVec3(0,0,0);
//            l.orientation_forward = CVec3(0,0,1);
//            l.orientation_up = CVec3(0,1,0);

//            //Set audio listener properties
//            coffee_audio_listener_set(&l);

//            //Create audio buffer, free the data
//            coffee_audio_alloc(buf,&smp);
//            cDebug("Snippet length: {0}",coffee_audio_sample_get_length(smp));
//            free(smp.data);

//            //Create audio source
//            coffee_audio_alloc(&src);
//            coffee_audio_source_transform(&src,CVec3(10,0,0),CVec3(0,0,0),CVec3(0,0,0));
//            coffee_audio_source_setf(&src,CSourceProperty::Gain,1.f);
//            coffee_audio_source_setf(&src,CSourceProperty::Pitch,1.f);
//            coffee_audio_source_setf(&src,CSourceProperty::RolloffFactor,1.f);
//            coffee_audio_source_setf(&src,CSourceProperty::MaxDist,110.f);
//            coffee_audio_source_setf(&src,CSourceProperty::ReferenceDistance,50.f);
//            coffee_audio_source_seti(&src,CSourceProperty::Looping,1);

//            //Queue our buffer for playback
//            coffee_audio_source_queue_buffers(&src,1,&buf);

//            coffee_audio_source_set_state(&src,CALPlaybackState::Playing);
//        }
*/

        CALSoundManager man;

        CSoundDevice<CALSource,CALBuffer>* dev = man.createDevice(man.defaultSoundDevice());

        CSoundMixer<CALSource,CALBuffer>& mixer = dev->mixer();

        uint64 trackId = mixer.createTrack();

        CSoundTrack<CALSource,CALBuffer>& track = mixer.soundtrack(trackId);

        CALSoundFormat fmt;
        fmt.setBitDepth(smp.fmt.bitdepth);
        fmt.setChannels(smp.fmt.channels);
        fmt.setSamplerate(smp.fmt.samplerate);

        CSoundBuffer<CALSource,CALBuffer>& buf = dev->genBuffer();
        buf.setFormat(fmt);

        CSoundSample<CALSource,CALBuffer>& samp = dev->genSample(buf,fmt);
        samp.setPts(0);
        buf.fillBuffer(smp.data,smp.samples);

        track.queueSample(samp);

        //Start playing

        this->showWindow();
        while(!closeFlag())
        {
            track.updateTrack(5);
            coffee_graphics_clear(CClearFlag::Color);
            this->pollEvents();
            this->swapBuffers();
        }

        c_free(smp.data);

        //Free the AL context and devices
//        coffee_audio_capture_free(cdev);
//        coffee_audio_context_destroy(ctxt);
//        delete buf;
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
            if(kev->key == CK_Escape)
                this->closeWindow();
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
