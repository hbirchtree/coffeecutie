#pragma once

#include <coffee/windowing/stub/stub.h>

#if defined(COFFEE_GEKKO)

namespace Coffee{
namespace Display{

class OGC_Window :
        public StubEventApplication,
        public StubWindowManagerClient,
        public StubGLLoader,
        public StubGLApplication
{
    void* fifoBuffer = nullptr;

public:
    OGC_Window() : GLLoader(this)
    {}

    virtual CString windowLibrary() const;

    virtual CSize framebufferSize() const;

    virtual bool windowPreInit(const CDProperties&, CString*);

    virtual void swapBuffers();
};

}
}
#endif
