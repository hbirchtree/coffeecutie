#include "cdrenderer.h"

//#define LOAD_FILE

#include "coffee/cfunctional.h"
#include "coffee_impl/sample/base_case.h"
#include "coffee_impl/graphics/cframebuffer.h"
#include "coffee_impl/graphics/cgraphics_quirks.h"
#include "coffee_impl/assimp/assimpfun.h"

#include "plat/plat_wm.h"

using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CRendering::CTest;

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLBindingRenderer(parent)
{
    m_msg_filter = [](CGLReport* r){
//        if(r->severity==GL_DEBUG_SEVERITY_NOTIFICATION)
//            return false;
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

//    CResource mdata("ubw/models/grass.fbx");
//    mdata.read_data();
//    CAssimpData* d = CAssimpImporters::importResource(&mdata,mdata.resource());
//    for(szptr i=0;i<d->numMeshes;i++)
//        game->meshes.push_back(d->meshes[i]);
//    mdata.free_data();

    CResource gdata("ubw/models/ubw.blend");
    gdata.read_data();
    CAssimpData* d2 = CAssimpImporters::importResource(&gdata,gdata.resource());
    for(szptr i=0;i<d2->numMeshes;i++)
        game->meshes.push_back(d2->meshes[i]);
    gdata.free_data();

    if(!coffee_test_load(game))
        return;

//    CResourceTypes::coffee_assimp_free(d);
    CResourceTypes::coffee_assimp_free(d2);

    showWindow();

//#ifdef COFFEE_LINUX

//    CDWindow* sdlwin = this->window();

//    CoffeePlat::X11::set_alwaysontop(sdlwin->wininfo.x11.display,sdlwin->wininfo.x11.window,true);

//#endif

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.175f,0.175f,0.175f,1.f);
    glViewport(0,0,m_properties.size.w,m_properties.size.h);

    bigscalar mtime = 0.0;
    CElapsedTimerMicro *swap = new CElapsedTimerMicro;

    setSwapInterval(0);
    cMsg("Coffee","Init time: %fs",contextTime());

    coffee_prepare_test(game);

    CFramebuffer fb;
    CTexture fbtex;
    fb.create();
    coffee_graphics_alloc(&fbtex);
    fbtex.textureType = GL_TEXTURE_2D;
    fbtex.levels = 1;
    CTextureTools::CTextureData texd;
    texd.format = GL_RGBA8;
    coffee_graphics_tex_activate(&fbtex);
    fb.bind();
    fb.unbind();
    CTextureTools::coffee_create_texturesize(&texd,1280,720);
    game->funptrs.tex_define(&fbtex,&texd);
    fb.attach(&fbtex,GL_COLOR_ATTACHMENT0,0);
    fb.valid();

    game->transforms.transforms.d[0].rotation =
            CMath::quat(CMath::vec3(-90.f,0,0))*game->transforms.transforms.d[0].rotation;

    while(!closeFlag()){
        delta = contextTime();

        swap->start();
        //Rendering part


        game->transforms.transforms.d[0].rotation =
                CMath::normalize(
                    CMath::quat(2,0,-0.1*deltaT,0)*
                    game->transforms.transforms.d[0].rotation);
//        game->transforms.cameras.d[0].position.z = CMath::fmod(contextTime()*4,90.0);

//        fb.bind(GL_DRAW_FRAMEBUFFER);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        coffee_render_test(game,deltaT);
//        fb.unbind(GL_DRAW_FRAMEBUFFER);

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
            cDebug("Render time: %lldus, swap time: %lldus, "
                   "input: %lldus, queue: %lldus, frames: %lld, frametime: %lldus",
                   rendertime,swaptime,
                   inputtime,qtime,frames-1,
                   rendertime+swaptime+inputtime+qtime);
            mtime = contextTime()+1.0;
            frames = 0;
        }
    }

    CResource texture("test.png");
    CStbImageLib::CStbImage img;
    img.size = windowSize();
    img.bpp = 4;
    swap->start();
    coffee_graphics_tex_download_texture(&fbtex,0,4*windowSize().w*windowSize().h,GL_RGBA,&img);
    cDebug("Fetching time: %lld",swap->elapsed());
    swap->start();
    CStbImageLib::coffee_stb_image_flip_vertical(&img);
    cDebug("Flipping time: %lld",swap->elapsed());
    swap->start();
    CStbImageLib::coffee_stb_image_save_png(&texture,&img);
    texture.save_data();
    CStbImageLib::coffee_stb_image_free(&img);
    cDebug("File time: %lld",swap->elapsed());

    swap->start();
    coffee_unload_test(game);
    delete game;

    hideWindow();

    cMsg("Coffee","Termination time: %lldus",swap->elapsed());
    delete swap;
}

void CDRenderer::run(CDWindowProperties props)
{
    init(props);

    run();

    cleanup();
}

void CDRenderer::bindingCallback(void *report) const
{
    CGLReport* rep = (CGLReport*)report;
    if(!m_msg_filter(rep))
        return;
    CString out = glbinding::Meta::getString(rep->type)+":"
            +glbinding::Meta::getString(rep->severity)+":"
            +glbinding::Meta::getString(rep->source)+": "+rep->message;
    cDebug("OpenGL: %s",out.c_str());
    free(report);
}

void CDRenderer::eventWHandle(const CDEvent *event)
{
    if(event->type==CDEvent::Resize &&
            m_properties.contextProperties.flags&CGLContextProperties::GLAutoResize){
        const CDResizeEvent* resize = (const CDResizeEvent*)&event[1];
        glViewport(0,0,resize->w,resize->h);

        game->transforms.cameras.d[0].aspect =
                (float)resize->w/(float)resize->h;
    }
}

void CDRenderer::eventIHandle(const CIEvent *event)
{
    if(event->type==CIEvent::Keyboard){
        const CIKeyEvent* kev = (const CIKeyEvent*)&event[1];
        cDebug("Key event: key=%i,mods=%i,scan=%i,char=%s",
               kev->key,kev->mod,kev->scan,&kev->key);
        if(kev->key==CK_Escape)
            this->closeWindow();
        if(kev->key==CK_Up&&kev->mod&CIKeyEvent::PressedModifier)
            game->transforms.cameras.d[0].position.y -= 0.05;
        if(kev->key==CK_Down&&kev->mod&CIKeyEvent::PressedModifier)
            game->transforms.cameras.d[0].position.y += 0.05;
    }
    else if(event->type==CIEvent::MouseMove)
    {
        const CIMouseMoveEvent* mev = (const CIMouseMoveEvent*)&event[1];
        if(relativeMouse())
        {
            game->transforms.cameras.d[0].rotation =
                    CMath::normalize(
                        CMath::quat(glm::vec3(0.01*mev->rel.y,0,0))
                        *CMath::quat(glm::vec3(0,0.01*mev->rel.x,0))
                        *game->transforms.cameras.d[0].rotation);
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
    else if(event->type==CIEvent::ControllerEv)
    {
        const CIControllerAtomicUpdateEvent* jev =
                (const CIControllerAtomicUpdateEvent*)&event[1];
        if(jev->connected()&&!jev->remapped()){
            cDebug("New controller: idx=%i,name=%s",jev->controller(),jev->name);
            _controllers_handle(jev);
        }else if(!jev->connected()){
            cDebug("Removed controller: idx=%i,name=%s",jev->controller(),jev->name);
            _controllers_handle(jev);
        }
    }
    else if(event->type==CIEvent::Controller)
    {
        //TODO: Create joystick filters
        //Allow buttons to have repeated state
        //Filter out centering of joystick (that is, movement towards its center)

        const CIControllerAtomicEvent* jev = (const CIControllerAtomicEvent*)&event[1];
        if(jev->axis())
        {
            if(CMath::fabs(jev->value)<0.1)
                return;
            switch(jev->index()){
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
        }
    }
}

} // namespace CDisplay
} // namespace Coffee
