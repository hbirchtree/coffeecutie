#include <coffee/Core>
#include <coffee/core/Graphics>
#include <coffee/core/plat/application_start.h>
#include <coffee/audio/openal/copenal.h>
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
        CResources::CResource rsc("monosample.ogg");
        coffee_file_pull(&rsc);
        CStbAudio::coffee_stb_audio_vorbis_load(&smp,&rsc);
        coffee_file_free(&rsc);

        CALContext ctxt;

        CALBuffer *buf = new CALBuffer;
        CALSource src;
        CALListener l;

        //Set error callback
        ctxt.callback = [](CALReport* r){
            cDebug("%s",r->message);
        };

        //Create the context, acquire possible errors
        coffee_audio_context_create(&ctxt);
        coffee_audio_context_get_error(&ctxt);

        //Set listener properties
        l.gain = 1.0;
        l.position = CVec3(0,0,0);
        l.velocity = CVec3(0,0,0);
        l.orientation_forward = CVec3(0,0,1);
        l.orientation_up = CVec3(0,1,0);

        //Set audio listener properties
        coffee_audio_listener_set(&l);
        //Create audio buffer, free the data
        coffee_audio_alloc(buf,&smp);
        free(smp.data);
        //Create audio source
        coffee_audio_alloc(&src);
        coffee_audio_source_transform(&src,CVec3(100,0,0),CVec3(0,0,0),CVec3(0,0,0));
        coffee_audio_source_setf(&src,CSourceProperty::RolloffFactor,1.f);
        //Queue our buffer for playback
        coffee_audio_source_queue_buffers(&src,1,&buf);
        //Start playing
        coffee_audio_source_set_state(&src,CALPlaybackState::Playing);

        this->showWindow();
        while(!closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color);

            this->pollEvents();
            this->swapBuffers();
        }

        //Free the AL context
        coffee_audio_context_destroy(&ctxt);
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

int32 coffee_main(int32 argc, byte** argv)
{
    CDRendererBase *renderer = new CDRenderer();
    renderer->init(coffee_get_default_visual());
    renderer->run();
    renderer->cleanup();
    delete renderer;
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
