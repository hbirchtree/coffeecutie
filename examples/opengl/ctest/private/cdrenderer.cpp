#include "cdrenderer.h"

#include <coffee/CGraphics>
#include <coffee/core/plat/plat_wm.h>
#include <coffee/core/input/cinputfunctions.h>
#include "base_case.h"

static CIAxisFilter m_axisFilter[2];

using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CRendering::CTest;

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLBindingRenderer(parent)
{
    m_msg_filter = coffee_graphics_debug_filter_ignore_notifications;
}

CDRenderer::~CDRenderer()
{
}

void CDRenderer::run()
{
    CGraphicsQuirks::CFeatureSet quirks;
    {
        CString extensions = coffee_graphics_get_extensions();
        quirks.extensions = extensions.data();
        CGraphicsQuirks::coffee_quirks_set(&quirks);
    }

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

    CResource gdata("ubw/ubw.blend");
    coffee_file_pull(gdata);
    CAssimpData* d2 = CAssimpImporters::importResource(&gdata,gdata.resource());
    for(szptr i=0;i<d2->numMeshes;i++)
        game->meshes.push_back(d2->meshes[i]);
    coffee_file_free(gdata);

    CResource zdata("ubw/zelda_1.fbx");
    coffee_file_pull(zdata);
    CAssimpData* d1 = CAssimpImporters::importResource(&zdata,zdata.resource());
    for(szptr i=0;i<d1->numMeshes;i++)
        game->meshes.push_back(d1->meshes[i]);
    coffee_file_free(zdata);

    if(!coffee_test_load(game))
        return;

    CResourceTypes::CAssimp::coffee_assimp_free(d2);
    CResourceTypes::CAssimp::coffee_assimp_free(d1);

    showWindow();

    coffee_graphics_clear((CClearFlag)100);

    coffee_graphics_depth(true);
    coffee_graphics_blend(true);

    coffee_graphics_set_clearcolor(CVec4(0.175f,0.175f,0.175f,1.f));
    coffee_graphics_set_viewport(m_properties.size);

    bigscalar mtime = 0.0;
    CElapsedTimerMicro* swap = coffee_fun_alloc_timer_micro();

    setSwapInterval(0);
    cMsg("Coffee","Init time: {0}s",(double)contextTime());

    coffee_prepare_test(game);

    game->transforms.transforms.d[0].rotation =
            CQuat(CVec3(-90.f,0,0))*game->transforms.transforms.d[0].rotation;

    while(!closeFlag()){
        delta = contextTime();

        swap->start();

        //Rendering part

        game->transforms.transforms.d[0].rotation =
                normalize_quat(
                    CQuat(2,0,-0.1*deltaT,0)*
                    game->transforms.transforms.d[0].rotation);
//        game->transforms.cameras.d[0].position.z = CMath::fmod(contextTime()*4,90.0);

        coffee_graphics_clear(CClearFlag::Color|CClearFlag::Depth);

        coffee_render_test(game,deltaT);

        // END Rendering part
        rendertime = swap->elapsed();

        //Event handling
        swap->start();
        executeRunQueue();
        qtime = swap->elapsed();
        swap->start();
        pollEvents();
        inputtime = swap->elapsed();
        //Buffer swapping
        swap->start();
        swapBuffers();
        swaptime = swap->elapsed();
        frames++;

        //Info
        deltaT = contextTime()-delta;
        if(contextTime()>mtime){
            cDebug("Render time: {0}us, swap time: {1}us, "
                   "input: {2}us, queue: {3}us, frames: {4}, frametime: {5}us",
                   rendertime,swaptime,
                   inputtime,qtime,frames-1,
                   rendertime+swaptime+inputtime+qtime);
            mtime = contextTime()+1.0;
            frames = 0;
        }
    }

    swap->start();
    coffee_unload_test(game);
    delete game;

    hideWindow();

    cMsg("Coffee","Termination time: {0}us",swap->elapsed());
}

void CDRenderer::run(const CDWindowProperties& props)
{
    init(props);
    run();
    cleanup();
}

void CDRenderer::eventHandleD(const CDEvent &event, c_cptr data)
{
    CSDL2Renderer::eventHandleD(event,data);
    switch(event.type)
    {
    case CDEvent::Resize:
    {
        if(m_properties.contextProperties.flags&CGLContextProperties::GLAutoResize){
            const CDResizeEvent* resize = (const CDResizeEvent*)data;
            coffee_graphics_set_viewport(*resize);

            game->transforms.cameras.d[0].aspect =
                    (float)resize->w/(float)resize->h;
        }
        break;
    }
    default:break;
    }
}

void CDRenderer::eventHandleI(const CIEvent &event, c_cptr data)
{
    CSDL2Renderer::eventHandleI(event,data);
    switch(event.type)
    {
    case CIEvent::Keyboard:
    {
        const CIKeyEvent* kev = (const CIKeyEvent*)data;
        switch(kev->key)
        {
        case CK_Escape:
            this->closeWindow();
            break;
        case CK_Up:
            if(!(kev->mod&CIKeyEvent::PressedModifier))
                break;
            game->transforms.cameras.d[0].position.y() -= 0.05;
            break;
        case CK_Down:
            if(!(kev->mod&CIKeyEvent::PressedModifier))
                break;
            game->transforms.cameras.d[0].position.y() += 0.05;
            break;
        }
        break;
    }
    case CIEvent::MouseMove:
    {
        const CIMouseMoveEvent* mev = (const CIMouseMoveEvent*)data;
        if(relativeMouse())
        {
            coffee_input_mouse_rotate(&(game->transforms.cameras.d[0].rotation),mev);
        }
        break;
    }
    case CIEvent::MouseButton:
    {
        const CIMouseButtonEvent* mev = (const CIMouseButtonEvent*)data;
        if(mev->btn == CIMouseButtonEvent::LeftButton)
            setRelativeMouse(true);
        else if(mev->btn == CIMouseButtonEvent::RightButton)
            setRelativeMouse(false);
        break;
    }
    case CIEvent::Controller:
    {
        const CIControllerAtomicEvent* jev = (const CIControllerAtomicEvent*)data;
        if(jev->axis)
        {
            CIAxisFilter& filter = (jev->index == CK_AXIS_RIGHT_X)
                    ? m_axisFilter[0] : m_axisFilter[1];
            coffee_input_controller_rotate(
                        &(game->transforms.cameras.d[0].rotation),
                    jev,filter);
        }
        else{
            switch(jev->index)
            {
            case CK_BUTTON_RB:{
                CIHapticEvent test;
                test.rumble_input.duration = 180;
                test.rumble_input.index = 0;
                test.rumble_input.strength = 0.6f;
                eventHandle(test,nullptr);
                break;
            }
            }
        }
        break;
    }
    case CIEvent::ControllerUpdate:
    {
        const CIControllerAtomicUpdateEvent* jev = (const CIControllerAtomicUpdateEvent*)data;
        CIHapticEvent hev;
        hev.rumble_input.index = jev->controller;
        hev.rumble_input.duration = 100;
        hev.rumble_input.strength = 0.5;
        eventHandleH(hev,nullptr);
        break;
    }
    case CIEvent::Drop:
    {
        const CIDropEvent* dev = (const CIDropEvent*)data;
        cDebug("File: {0}",dev->text_data.text);
        break;
    }
    default:break;
    }
}

} // namespace CDisplay
} // namespace Coffee
