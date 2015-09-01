#include "cdrenderer.h"

#include "tests/CubeScape.h"

namespace Coffee {
namespace CDisplay {

CDRenderer::CDRenderer(CObject *parent) : CGLFWRenderer(parent)
{
}

void CDRenderer::run()
{
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

void CDRenderer::glbindingCallbackInternal(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg) const
{
    std::string smsg = msg;
    std::string out = "OpenGL: "
            +glbinding::Meta::getString(source)+"("
            +glbinding::Meta::getString(type)+"):"
            +glbinding::Meta::getString(severity)+": "+smsg;
    cDebug(0,"OpenGL: %s",out.c_str());
}

} // namespace CDisplay
} // namespace Coffee
