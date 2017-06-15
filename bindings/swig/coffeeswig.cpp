#include "coffeeswig.h"

#include <coffee/CCore>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/CSDL2>
#include <coffee/graphics/apis/CGLeam>
#include <coffee/sdl2/CSDL2GLRenderer>
#include <coffee/core/types/vector_types.h>

namespace Coffee{

void Init()
{
    CoffeeInit(true);
}

void Terminate()
{
    Profiling::ExitRoutine();
    CoffeeTerminate(false);
}

class CGLWindow : public Display::CSDL2Renderer
{
public:
    CGLWindow(CObject* p):
        CSDL2Renderer(p)
    {
    }
    void run();
};

void CGLWindow::run()
{
    /* Stub! Do what you want. */
}

namespace Window{
CGLWindow* CreateWindow(int w, int h,
                        int major, int minor,
                        bool debug,
                        CObject* p)
{
    Display::CDProperties o = Display::GetDefaultVisual(major,minor);
    o.size.w = w;
    o.size.h = h;
    if(debug)
        o.gl.flags = o.gl.flags|Display::GLProperties::GLDebug;

    CString err;

    CGLWindow* win = new CGLWindow(p);
    if(!win->init(o,&err))
        cDebug("Window creation error: {0}",err);

    return win;
}
void DestroyWindow(CGLWindow* p)
{
    p->closeWindow();
    p->cleanup();
    delete p;
}
void ShowWindow(CGLWindow* p)
{
    p->showWindow();
}
void HideWindow(CGLWindow* p)
{
    p->hideWindow();
}
bool ShouldClose(CGLWindow* p)
{
    return p->closeFlag();
}
void CloseWindow(CGLWindow* p)
{
    p->closeWindow();
}
void Process(CGLWindow* p)
{
    p->swapBuffers();
    p->pollEvents();
}

void SetWindowTitle(CGLWindow *p, const char *title)
{
    p->setWindowTitle(title);
}

void SetWindowSize(CGLWindow *p, int w, int h)
{
    p->setWindowSize(CSize(w,h));
}

void SetWindowPosition(CGLWindow *p, int x, int y)
{
    p->setWindowPosition(CPoint(x,y));
}

CGL::CGL_Context *GetContext(CGLWindow *p)
{
    return p->glContext();
}

}

#ifdef COFFEE_GLEAM_DESKTOP
using GL = CGL::CGL33;
#elif !defined(COFFEE_GLES20_MODE)
using GL = CGL::CGLES30;
#else
using GL = CGL::CGLES20;
#endif

void ClearBuffer(float dep, float color[4])
{
#if defined(COFFEE_GLES20_MODE)
    Vecf4 clearcol = {color[0], color[1], color[2], color[3]};
    GL::ClearBufferfv(false, 0, clearcol);
    GL::ClearBufferfv(&dep);
#else
    GL::ClearBufferfv(&dep);
    GL::ClearBufferfv(true,0,Vecf4(color[0],color[1],
                                   color[2],color[3]));
#endif
}

void Profile::Push(const char* name)
{
    Profiler::PushContext(name);
}
void Profile::Pop()
{
    Profiler::PopContext();
}
void Profile::Tag(const char* name)
{
    Profiler::Profile(name);
}
void Profile::LabelThread(const char* name)
{
    Profiler::LabelThread(name);
}
void Profile::Save(const char* outfile)
{
    CString log_data;
    Profiling::ExportProfilerData(log_data);
    Profiling::ExportStringToFile(log_data, outfile);
}
void Profile::PrintData()
{
    Profiling::PrintProfilerData();
}

void SetLogVerbosity(char level)
{
    Coffee::SetPrintingVerbosity(level);
}

char GetLogVerbosity()
{
    return Coffee::PrintingVerbosityLevel;
}

}
