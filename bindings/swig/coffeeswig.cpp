#include <coffee/CCore>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/CSDL2>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/sdl2/CSDL2GLRenderer>

namespace Coffee{

void Init()
{
    CoffeeInit(true);
}

void Terminate()
{
    Profiling::ExitRoutine(0,nullptr);
    CoffeeTerminate(false);
}

class CGLWindow : public Display::CSDL2Renderer
{
public:
    CGLWindow(CObject* p):
        CSDL2Renderer(p)
    {
    }
    void run()
    {
        /* Stub! Do what you want. */
    }
};

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

namespace CGL{

#ifdef COFFEE_GLEAM_DESKTOP
using GL = CGL33;
#else
using GL = CGLES30;
#endif

void ClearBuffer(float dep, float color[4])
{
    GL::ClearBufferfv(&dep);
    GL::ClearBufferfv(true,0,Vecf4(color[0],color[1],
                                   color[2],color[3]));
}

}

namespace Profiling{

void exPrintProfilerData()
{
    PrintProfilerData();
}

void exExportProfilerData(const char* outfile)
{
    ExportProfilerData(outfile,GetInitArgs().argc,GetInitArgs().argv);
}

void Perf_PushContext(const char *name)
{
    Profiler::PushContext(name);
}

void Perf_PopContext()
{
    Profiler::PopContext();
}

void Perf_Profile(const char *name)
{
    Profiler::Profile(name);
}

void Perf_LabelThread(const char *name)
{
    Profiler::LabelThread(name);
}

}

}
