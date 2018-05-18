#include <coffee/windowing/stub/stub.h>

namespace Coffee{
namespace Display{

StubWindowManagerClient::~StubWindowManagerClient()
{
}

CDMonitor StubWindowManagerClient::monitor()
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

void StubWindowManagerClient::popErrorMessage(Severity, cstring, cstring)
{
}

bool StubWindowManagerClient::windowPreInit(const CDProperties &, CString *)
{
    return true;
}

bool StubWindowManagerClient::windowInit(const CDProperties &, CString *)
{
    return true;
}

bool StubWindowManagerClient::windowPostInit(const CDProperties &, CString *)
{
    return true;
}

void StubWindowManagerClient::windowTerminate()
{
}

CDWindow *StubWindowManagerClient::window()
{
    return nullptr;
}

uint32 StubWindowManagerClient::windowState() const
{
    return 0;
}

void StubWindowManagerClient::setWindowState(const CDProperties::State &)
{
}

CSize StubWindowManagerClient::windowSize() const
{
    return {};
}

void StubWindowManagerClient::setWindowSize(const CSize &)
{
}

CPoint StubWindowManagerClient::windowPosition() const
{
    return {};
}

void StubWindowManagerClient::setWindowPosition(const CPoint &)
{
}

CString StubWindowManagerClient::windowTitle() const
{
    return {};
}

void StubWindowManagerClient::setWindowTitle(const CString &)
{
}

bool StubWindowManagerClient::setWindowIcon(CBitmap &)
{
    return false;
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

bool StubGLApplication::contextPreInit(const GLProperties &, CString *)
{
    return true;
}

bool StubGLApplication::contextInit(const GLProperties &, CString *)
{
    return true;
}

bool StubGLApplication::contextPostInit(const GLProperties &, CString *)
{
    return true;
}

void StubGLApplication::contextTerminate()
{

}

void StubGLApplication::swapBuffers()
{
}

CSize StubGLApplication::framebufferSize() const
{
    return {};
}

int StubGLApplication::swapInterval() const
{
    return -1;
}

void StubGLApplication::setSwapInterval(const int &)
{
}

CDContextBits StubGLApplication::context()
{
    return {};
}

ThreadId StubGLApplication::contextThread()
{
    return {};
}

CGL::CGL_Context *StubGLApplication::glContext()
{
    return nullptr;
}

CGL::CGL_ScopedContext StubGLApplication::scopedContext()
{
    return CGL::CGL_ScopedContext(nullptr);
}

void StubGLLoader::bindingCallback(const void *) const
{
}

StubGLLoader::StubGLLoader():
    GLLoader(nullptr)
{

}

StubGLLoader::~StubGLLoader()
{

}

bool StubGLLoader::bindingPreInit(const GLProperties &, CString *)
{
    return true;
}

bool StubGLLoader::bindingInit(const GLProperties &, CString *)
{
    return true;
}

bool StubGLLoader::bindingPostInit(const GLProperties &, CString *)
{
    return true;
}

void StubGLLoader::bindingTerminate()
{
}

StubEventApplication::~StubEventApplication()
{
}

}
}
