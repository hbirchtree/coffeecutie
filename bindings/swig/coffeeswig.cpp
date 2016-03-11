#include <coffee/CCore>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/CSDL2>
#include <coffee/graphics_apis/CGLeam>

namespace Coffee{

class CGLWindow : public CDisplay::CGLeamRenderer
{
public:
    CGLWindow(CObject* p):
        CGLeamRenderer(p)
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
    CDisplay::CDProperties o = CDisplay::GetDefaultVisual(major,minor);
    o.size.w = w;
    o.size.h = h;
    if(debug)
        o.gl.flags = o.gl.flags|CDisplay::GLProperties::GLDebug;

    CGLWindow* win = new CGLWindow(p);
    win->init(o);
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
    GL::ClearBufferfv(false,0,&dep);
    GL::ClearBufferfv(true,0,color);
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