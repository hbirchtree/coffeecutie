#include "cdrenderer.h"

#include "coffee_impl/graphics/cgraphicswrappers.h"

using namespace Coffee::CGraphicsWrappers;

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

void CDRenderer::glbindingCallbackInternal(CGLReport *report) const
{
    std::string smsg = report->message;
    std::string out = "OpenGL: "
            +glbinding::Meta::getString(report->source)+"("
            +glbinding::Meta::getString(report->type)+"):"
            +glbinding::Meta::getString(report->severity)+": "+smsg;
    cDebug(0,"OpenGL: %s",out.c_str());
}

} // namespace CDisplay
} // namespace Coffee
