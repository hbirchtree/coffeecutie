#include "cdrenderer.h"

//#define LOAD_FILE

#include <coffee/core/graphics/cframebuffer.h>
#include <coffee/core/graphics/cgraphics_quirks.h>
#include <coffee/assimp/assimpfun.h>
#include <coffee/core/plat/plat_wm.h>
#include <coffee/core/input/cinputfunctions.h>
#include "base_case.h"


using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CRendering::CTest;

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLBindingRenderer(parent)
{
    m_msg_filter = [](CGLReport* r){
        if(r->severity==GL_DEBUG_SEVERITY_NOTIFICATION)
            return false;
        return true;
    };
}

CDRenderer::~CDRenderer()
{
}

void CDRenderer::run()
{
    fetchGLExtensions();

    CGraphicsQuirks::CFeatureSet quirks;
    quirks.extensions = extensions();
    CGraphicsQuirks::coffee_quirks_set(&quirks);

    double delta = contextTime();
    double deltaT = 0;
    uint64 frames = 0;
    uint64 rendertime = 0;
    uint64 inputtime = 0;
    uint64 qtime = 0;
    uint64 swaptime = 0;

    game = new game_context;
    game->features = &quirks;

    coffee_test_fun_set(game);

    CResource gdata("ubw/models/ubw.blend");
    gdata.memory_map();
    CAssimpData* d2 = CAssimpImporters::importResource(&gdata,gdata.resource());
    for(szptr i=0;i<d2->numMeshes;i++)
        game->meshes.push_back(d2->meshes[i]);
    gdata.memory_unmap();

    CResource zdata("ubw/models/Zelda 1.fbx");
    zdata.memory_map();
    CAssimpData* d1 = CAssimpImporters::importResource(&zdata,zdata.resource());
    for(szptr i=0;i<d1->numMeshes;i++)
        game->meshes.push_back(d1->meshes[i]);
    zdata.memory_unmap();

    if(!coffee_test_load(game))
        return;

    CResourceTypes::coffee_assimp_free(d2);
    CResourceTypes::coffee_assimp_free(d1);

    showWindow();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_FRONT);

    glClearColor(0.175f,0.175f,0.175f,1.f);
    glViewport(0,0,m_properties.size.w,m_properties.size.h);

    bigscalar mtime = 0.0;
    CElapsedTimerMicro swap;

    setSwapInterval(0);
    cMsg("Coffee","Init time: %fs",contextTime());

    coffee_prepare_test(game);

    game->transforms.transforms.d[0].rotation =
            CMath::quat(CMath::vec3(-90.f,0,0))*game->transforms.transforms.d[0].rotation;

    while(!closeFlag()){
        delta = contextTime();

        swap.start();

        //Rendering part

        game->transforms.transforms.d[0].rotation =
                CMath::normalize(
                    CMath::quat(2,0,-0.1*deltaT,0)*
                    game->transforms.transforms.d[0].rotation);
//        game->transforms.cameras.d[0].position.z = CMath::fmod(contextTime()*4,90.0);

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        coffee_render_test(game,deltaT);
        // END Rendering part
        rendertime = swap.elapsed();

        //Event handling
        swap.start();
        executeRunQueue();
        qtime = swap.elapsed();
        swap.start();
        pollEvents();
        inputtime = swap.elapsed();
        //Buffer swapping
        swap.start();
        swapBuffers();
        swaptime = swap.elapsed();
        frames++;

        //Info
        deltaT = contextTime()-delta;
        if(contextTime()>mtime){
            cDebug("Render time: %lldus, swap time: %lldus, "
                   "input: %lldus, queue: %lldus, frames: %lld, frametime: %lldus",
                   rendertime,swaptime,
                   inputtime,qtime,frames-1,
                   rendertime+swaptime+inputtime+qtime);
            mtime = contextTime()+1.0;
            frames = 0;
        }
    }

    swap.start();
    coffee_unload_test(game);
    delete game;

    hideWindow();

    cMsg("Coffee","Termination time: %lldus",swap.elapsed());
}

void CDRenderer::run(const CDWindowProperties& props)
{
    init(props);
    run();
    cleanup();
}

void CDRenderer::eventWindowsHandle(const CDEvent *event)
{
    if(event->type==CDEvent::Resize &&
            m_properties.contextProperties.flags&CGLContextProperties::GLAutoResize){
        const CDResizeEvent* resize = (const CDResizeEvent*)&event[1];
        glViewport(0,0,resize->w,resize->h);

        game->transforms.cameras.d[0].aspect =
                (float)resize->w/(float)resize->h;
    }else if(event->type==CDEvent::State){
        const CDStateEvent* ste = (const CDStateEvent*)&event[1];
        if(ste->type == CDStateEvent::Closed)
            closeWindow();
    }
}

void CDRenderer::eventInputHandle(const CIEvent *event)
{
    CSDL2Renderer::eventInputHandle(event);
    if(event->type==CIEvent::Keyboard){
        const CIKeyEvent* kev = (const CIKeyEvent*)&event[1];
//        cDebug("Key event: key=%i,mods=%i,scan=%i,char=%s",
//               kev->key,kev->mod,kev->scan,&kev->key);
        if(kev->key==CK_Escape)
            this->closeWindow();
        else if(kev->key==CK_Up&&kev->mod&CIKeyEvent::PressedModifier)
            game->transforms.cameras.d[0].position.y -= 0.05;
        else if(kev->key==CK_Down&&kev->mod&CIKeyEvent::PressedModifier)
            game->transforms.cameras.d[0].position.y += 0.05;
    }
    else if(event->type==CIEvent::MouseMove)
    {
        const CIMouseMoveEvent* mev = (const CIMouseMoveEvent*)&event[1];
        if(relativeMouse())
        {
            CInput::coffee_input_mouse_rotate(&(game->transforms.cameras.d[0].rotation),mev);
        }
    }
    else if(event->type==CIEvent::MouseButton)
    {
        const CIMouseButtonEvent* mev = (const CIMouseButtonEvent*)&event[1];
        if(mev->btn == CIMouseButtonEvent::LeftButton)
            setRelativeMouse(true);
        else if(mev->btn == CIMouseButtonEvent::RightButton)
            setRelativeMouse(false);
    }
    else if(event->type==CIEvent::Controller)
    {
        const CIControllerAtomicEvent* jev = (const CIControllerAtomicEvent*)&event[1];
        if(jev->axis)
        {
            if(CMath::fabs(jev->value)<0.1)
                return;
            switch(jev->index){
            case CK_AXIS_LEFT_X:{
                game->transforms.cameras.d[0].position.x -= jev->value/32000.f;
                break;
            }
            case CK_AXIS_LEFT_Y:{
                game->transforms.cameras.d[0].position.y -= jev->value/32000.f;
                break;
            }
            default:
                break;
            }
        }else{
            switch(jev->index)
            {
            case CK_BUTTON_RB:{
                if(!jev->button_state)
                    break;
                CIHapticEvent test;
                test.rumble_input.duration = 180;
                test.rumble_input.index = 0;
                test.rumble_input.strength = 0.6f;
                eventHapticHandle(&test);
                break;
            }
            }
        }
    }
    else if(event->type==CIEvent::Drop)
    {
        const CIDropEvent* dev = (const CIDropEvent*)&event[1];
        cDebug("File: %s",dev->text_data.text);
    }
}

} // namespace CDisplay
} // namespace Coffee
