#include "cdrenderer.h"

//#define LOAD_FILE

#include "coffee/cfunctional.h"
#include "coffee_impl/sample/base_case.h"
#include "coffee_impl/graphics/cgraphics_quirks.h"

#include "plat/plat_wm.h"

using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CRendering::CTest;

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLBindingRenderer(parent)
{
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

    //We set the function pointer that works for the set quirks
    if(quirks.render_multidraw)
        game->renderfun = coffee_multidraw_render;
    else
        game->renderfun = coffee_multidraw_render_safe;

    if(!coffee_test_load(game))
        return;

    showWindow();

#ifdef COFFEE_LINUX

    CDWindow* sdlwin = this->window();

    CoffeePlat::X11::set_alwaysontop(sdlwin->wininfo.x11.display,sdlwin->wininfo.x11.window,true);

#endif

    glCullFace(GL_BACK);
    glClearColor(0.175f,0.175f,0.175f,1.f);
    glViewport(0,0,m_properties.size.w,m_properties.size.h);

    bigscalar mtime = 0.0;
    CElapsedTimerMicro *swap = new CElapsedTimerMicro;

    setSwapInterval(0);
    setWindowTitle(cStringFormat("%s renderer (init time: %fs)",
                                 m_contextString.c_str(),
                                 contextTime()));
    cMsg("Coffee","Init time: %fs",contextTime());

    coffee_prepare_test(game);

    while(!closeFlag()){
        delta = contextTime();

        swap->start();
        //Rendering part

        game->transforms.transforms.d[0].rotation =
                CMath::normalize(
                    CMath::quat(2,0,0,-0.1*deltaT)*
                    game->transforms.transforms.d[0].rotation);
        game->transforms.cameras.d[0].position.z = CMath::fmod(contextTime()*4,90.0);

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
            cDebug("Render time: %lldus, swap time: %lldus, "
                   "input: %lldus, queue: %lldus, frames: %lld, frametime: %lldus",
                   rendertime,swaptime,
                   inputtime,qtime,frames-1,
                   rendertime+swaptime+inputtime+qtime);
            mtime = contextTime()+1.0;
            frames = 0;
        }
    }

    coffee_unload_test(game);

    swap->start();
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
    CString smsg = rep->message;
    CString out = glbinding::Meta::getString(rep->type)+":"
            +glbinding::Meta::getString(rep->severity)+":"
            +glbinding::Meta::getString(rep->source)+": "+smsg;
    cDebug("OpenGL: %s",out.c_str());
//    CGLState* state = _dump_state(); //Should provide a view of OpenGL state
//    delete state;
    free(report);
}

void CDRenderer::eventWHandle(const CDEvent *event)
{
    if(event->type==CDEvent::Resize){
        if(m_properties.contextProperties.flags&CGLContextProperties::GLAutoResize){
            const CDResizeEvent* resize = (const CDResizeEvent*)&event[1];
            glViewport(0,0,resize->w,resize->h);
        }
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

CGLState *CDRenderer::_dump_state() const
{
    CGLState *state = new CGLState;
    GLint t;

    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&t);
    state->vertex_array = t;

    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,&t);
    state->element_buffer = t;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&t);
    state->array_buffer = t;

    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING,&t);
    state->uniform_buffer = t;

    glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING,&t);
    state->indirect_buffer = t;

    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING,&t);
    state->pipeline_obj = t;

    glGetIntegerv(GL_TEXTURE_BINDING_2D,&t);
    state->texture_2d = t;

    glGetIntegerv(GL_TEXTURE_BINDING_3D,&t);
    state->texture_3d = t;

    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,&t);
    state->texture_cube = t;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING,&t);
    state->fb_all = t;

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&t);
    state->fb_draw = t;

    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&t);
    state->fb_read = t;

    glGetIntegerv(GL_VERTEX_ARRAY_BUFFER_BINDING,&t);
    state->vertex_binding = t;

    glGetFloatv(GL_COLOR_CLEAR_VALUE,reinterpret_cast<GLfloat*>(&state->clear_color));

    return state;
}

} // namespace CDisplay
} // namespace Coffee
