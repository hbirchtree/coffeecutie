#include "cdrenderer.h"

//#define LOAD_FILE

#include "coffee_impl/graphics/cshader.h"
#include "coffee/cfiles.h"

using namespace Coffee::CResources;
using namespace Coffee::CGraphicsWrappers;

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLFWRenderer(parent)
{
}

void CDRenderer::run()
{
#ifndef LOAD_FILE
    CResource v = CResource("ubw/shaders/vsh.vs");
    CResource f = CResource("ubw/shaders/fsh.fs");
    CShader* vshdr = new CShader;
    CShader* fshdr = new CShader;
    cDebug("Compile status: %i",vshdr->compile(&v,GL_VERTEX_SHADER));
    cDebug("Compile status: %i",fshdr->compile(&f,GL_FRAGMENT_SHADER));
#endif

    CShaderProgram* prog = new CShaderProgram;
#ifndef LOAD_FILE
    prog->create();
    prog->attachShader(vshdr,GL_VERTEX_SHADER_BIT);
    prog->attachShader(fshdr,GL_FRAGMENT_SHADER_BIT);
    prog->link();
#else
    CResource progStore("shader.dmp");
    if(progStore.read_data())
        cDebug("Loaded program: %i",prog->fetchProgram(&progStore),true);
#endif

    CPipeline* pip = new CPipeline;
    pip->create();
    pip->attachProgram(prog,GL_VERTEX_SHADER_BIT|GL_FRAGMENT_SHADER_BIT);

#ifndef LOAD_FILE
    CResource progStore("shader.dmp");
    prog->storeProgram(&progStore);
    if(!progStore.save_data())
        cDebug("Failed to save shader!");
#endif

    showWindow();
    CString title = cStringFormat("GLFW OpenGL renderer (running for %fs)",contextTime());
    while(!closeFlag()){
        setWindowTitle(title);
        executeRunQueue();
        pollEvents();
        swapBuffers();
    }
    hideWindow();

    pip->free();
#ifndef LOAD_FILE
    v.free_data();
    f.free_data();
    delete vshdr;
    delete fshdr;
#endif
    delete prog;
    delete pip;
}

void CDRenderer::run(WindowState state, CSize resolution, int monitor)
{
    init(state,resolution,monitor);

    run();

    cleanup();
}

void CDRenderer::glbindingCallbackInternal(CGLReport *report) const
{
    std::string smsg = report->message;
    std::string out = glbinding::Meta::getString(report->type)+":"
            +glbinding::Meta::getString(report->severity)+":"
            +glbinding::Meta::getString(report->source)+": "+smsg;
    cDebug("OpenGL: %s",out.c_str());
    free(report);
}

} // namespace CDisplay
} // namespace Coffee
