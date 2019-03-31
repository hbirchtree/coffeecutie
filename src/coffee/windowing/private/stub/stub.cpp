#include <coffee/windowing/stub/stub.h>

namespace Coffee {
namespace Display {

StubWindowManagerClient::~StubWindowManagerClient()
{
}

Monitor StubWindowManagerClient::monitor()
{
    return {};
}

bool StubWindowManagerClient::showWindow()
{
    return false;
}

bool StubWindowManagerClient::hideWindow()
{
    return false;
}

bool StubWindowManagerClient::closeWindow()
{
    EventApplication* evapp = nullptr;
    if((evapp = C_DCAST<EventApplication>(this)))
    {
        return evapp->applyCloseFlag();
    }

    return false;
}

void StubWindowManagerClient::popErrorMessage(debug::Severity, cstring, cstring)
{
}

bool StubWindowManagerClient::windowPreInit(const Properties&, CString*)
{
    return true;
}

bool StubWindowManagerClient::windowInit(const Properties&, CString*)
{
    return true;
}

bool StubWindowManagerClient::windowPostInit(const Properties&, CString*)
{
    return true;
}

void StubWindowManagerClient::windowTerminate()
{
}

Window* StubWindowManagerClient::window()
{
    return nullptr;
}

u32 StubWindowManagerClient::windowState() const
{
    return 0;
}

void StubWindowManagerClient::setWindowState(const Properties::State&)
{
}

Size StubWindowManagerClient::windowSize() const
{
    return {};
}

void StubWindowManagerClient::setWindowSize(const Size&)
{
}

Point StubWindowManagerClient::windowPosition() const
{
    return {};
}

void StubWindowManagerClient::setWindowPosition(const Point&)
{
}

CString StubWindowManagerClient::windowTitle() const
{
    return {};
}

void StubWindowManagerClient::setWindowTitle(const CString&)
{
}

bool StubWindowManagerClient::screensaverMode()
{
    return false;
}

void StubWindowManagerClient::setScreensaverMode(bool)
{
}

StubGLApplication::~StubGLApplication()
{
}

bool StubGLApplication::contextPreInit(const GL::Properties&, CString*)
{
    return true;
}

bool StubGLApplication::contextInit(const GL::Properties&, CString*)
{
    return true;
}

bool StubGLApplication::contextPostInit(const GL::Properties&, CString*)
{
    return true;
}

void StubGLApplication::contextTerminate()
{
}

void StubGLApplication::swapBuffers()
{
}

Size StubGLApplication::framebufferSize() const
{
    return {};
}

int StubGLApplication::swapInterval() const
{
    return -1;
}

void StubGLApplication::setSwapInterval(const int&)
{
}

ContextBits StubGLApplication::context()
{
    return {};
}

ThreadId StubGLApplication::contextThread()
{
    return {};
}

CGL::CGL_Context* StubGLApplication::glContext()
{
    return nullptr;
}

CGL::CGL_ScopedContext StubGLApplication::scopedContext()
{
    return CGL::CGL_ScopedContext(nullptr);
}

void StubGLLoader::bindingCallback(const void*) const
{
}

StubGLLoader::StubGLLoader() : GLLoader(nullptr)
{
}

StubGLLoader::~StubGLLoader()
{
}

bool StubGLLoader::bindingPreInit(const GL::Properties&, CString*)
{
    return true;
}

bool StubGLLoader::bindingInit(const GL::Properties&, CString*)
{
    return true;
}

bool StubGLLoader::bindingPostInit(const GL::Properties&, CString*)
{
    return true;
}

void StubGLLoader::bindingTerminate()
{
}

StubEventApplication::~StubEventApplication()
{
}

} // namespace Display
} // namespace Coffee
