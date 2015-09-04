#include "cdrenderer.h"

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
    CResource t = CResource("ubw/shaders/vsh.vs");
    CShader* shdr = new CShader;
    cDebug("Compile status: %i",shdr->compile(&t,GL_VERTEX_SHADER,GL_VERTEX_SHADER_BIT));

    while(!closeFlag()){
        executeRunQueue();
        pollEvents();
        swapBuffers();
    }
}

void CDRenderer::run(WindowState state, CSize resolution, int monitor)
{
    init(state,resolution,monitor);

    showWindow();
    run();
    hideWindow();

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
