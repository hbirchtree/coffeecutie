#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/core/plat/application_start.h>
#include <openal/copenal.h>
#include <coffee/core/base/cfiles.h>

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
        coffee_file_pull(&rsc);
        CStbAudio::coffee_stb_audio_vorbis_load(&smp,&rsc);
        coffee_file_free(&rsc);

        CALBuffer *buf = new CALBuffer;
        CALSource src;
        CALListener l;

        //Create the context, acquire possible errors
        CALContext* ctxt = coffee_audio_context_create();

        coffee_audio_context_get_error(ctxt);

        //Set error callback in case of errors
        coffee_audio_context_set_debug_callback(ctxt,[](CALReport* r){
            cDebug("%s",r->message);
        });

        //Create a capture device for testing
        CALCaptureDevice* cdev;
        {
            int32 idevn;
            cstring* idevs = coffee_audio_context_devices_input(&idevn);

            CAudioFormat cfmt;
            cfmt.bitdepth = 16;
            cfmt.channels = 1;
            cfmt.samplerate = 44100;
            cfmt.samples = 1024;
            cdev = coffee_audio_capture_create(ctxt,idevs[0],cfmt);
        }

        //Set distance model for context
        coffee_audio_context_set_distance_model(CDistanceModel::LinearDistanceClamped);

        {
            //Set listener properties
            l.gain = 1;
            l.position = CVec3(0,0,0);
            l.velocity = CVec3(0,0,0);
            l.orientation_forward = CVec3(0,0,1);
            l.orientation_up = CVec3(0,1,0);

            //Set audio listener properties
            coffee_audio_listener_set(&l);

            //Create audio buffer, free the data
            coffee_audio_alloc(buf,&smp);
            cDebug("Snippet length: %f",coffee_audio_sample_get_length(smp));
            free(smp.data);

            //Create audio source
            coffee_audio_alloc(&src);
            coffee_audio_source_transform(&src,CVec3(10,0,0),CVec3(0,0,0),CVec3(0,0,0));
            coffee_audio_source_setf(&src,CSourceProperty::Gain,1.f);
            coffee_audio_source_setf(&src,CSourceProperty::Pitch,1.f);
            coffee_audio_source_setf(&src,CSourceProperty::RolloffFactor,1.f);
            coffee_audio_source_setf(&src,CSourceProperty::MaxDist,110.f);
            coffee_audio_source_setf(&src,CSourceProperty::ReferenceDistance,50.f);
            coffee_audio_source_seti(&src,CSourceProperty::Looping,1);

            //Queue our buffer for playback
            coffee_audio_source_queue_buffers(&src,1,&buf);
        }

        //Start playing

        this->showWindow();
        while(!closeFlag())
        {
            //Grab samples from capture device
            coffee_audio_capture_grab_samples(cdev,smp);
            coffee_audio_capture_stop(cdev);
            //Upload to audio buffer
            coffee_audio_buffer_data(buf,&smp);
            //Reset pointer
            coffee_audio_source_set_offset_bytes(&src,0);
            //Play
            coffee_audio_source_set_state(&src,CALPlaybackState::Playing);
            //Start recording
            coffee_audio_capture_start(cdev);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            coffee_graphics_clear(CClearFlag::Color);
            this->pollEvents();
            this->swapBuffers();
            //Stop playing
            coffee_audio_source_set_state(&src,CALPlaybackState::Stopped);
        }


        //Free the AL context and devices
        coffee_audio_capture_free(cdev);
        coffee_audio_context_destroy(ctxt);
        delete buf;
    }
    void eventWindowsHandle(const CDisplay::CDEvent *e)
    {
        if(e->type==CDEvent::State)
        {
            const CDStateEvent* sev = (const CDStateEvent*)&e[1];
            if(sev->type==CDStateEvent::Closed)
                this->closeWindow();
        }
    }
    void eventInputHandle(const CIEvent *e)
    {
        CSDL2Renderer::eventInputHandle(e);
        if(e->type==CIEvent::Keyboard)
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)&e[1];
            if(kev->key == CK_Escape)
                this->closeWindow();
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
