#undef SWIG
#include "coffeeswig.h"

#include <coffee/CCore>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/CSDL2>
#include <coffee/graphics/apis/CGLeam>
#include <coffee/windowing/renderer/renderer.h>
#include <coffee/core/types/vector_types.h>


using namespace Coffee;

CObject& GetRootObject()
{
    static CObject root_object;
    return root_object;
}

void Init()
{
    CoffeeInit(true);
}

void Terminate()
{
    Profiling::ExitRoutine();
    CoffeeTerminate();
}

SDL2Renderer* CreateWindow(Props const& props, CObject *p)
{
    CString err;

    SDL2Renderer* win = new SDL2Renderer();
    if(!win->init(props,&err))
        cDebug("Window creation error: {0}",err);

    win->setParent(p);

    return win;
}
void DestroyWindow(SDL2Renderer* p)
{
    p->closeWindow();
    p->cleanup();
    delete p;
}
void ShowWindow(SDL2Renderer* p)
{
    p->showWindow();
}
void HideWindow(SDL2Renderer* p)
{
    p->hideWindow();
}
bool ShouldClose(SDL2Renderer* p)
{
    return p->closeFlag();
}
void CloseWindow(SDL2Renderer* p)
{
    p->closeWindow();
}
void Process(SDL2Renderer* p)
{
    p->swapBuffers();
    p->pollEvents();
}

void SetWindowTitle(SDL2Renderer *p, const char *title)
{
    p->setWindowTitle(title);
}

void SetWindowSize(SDL2Renderer *p, int w, int h)
{
    p->setWindowSize(CSize(w,h));
}

void SetWindowPosition(SDL2Renderer *p, int x, int y)
{
    p->setWindowPosition(CPoint(x,y));
}

GLContext *GetContext(SDL2Renderer *p)
{
    return C_DCAST<GLContext>(p->glContext());
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

void Push(const char* name)
{
    Coffee::Profiler::PushContext(name);
}
void Pop()
{
    Coffee::Profiler::PopContext();
}
void Tag(const char* name)
{
    Coffee::Profiler::Profile(name);
}
void LabelThread(const char* name)
{
    Coffee::Profiler::LabelThread(name);
}
void Save(const char* outfile)
{
    CString log_data;
    Profiling::ExportProfilerData(log_data);
    Profiling::ExportStringToFile(log_data, outfile);
}
void PrintData()
{
    Profiling::PrintProfilerData();
}

void SetLogVerbosity(int level)
{
    Coffee::SetPrintingVerbosity(C_FCAST<u8>(level));
}

int GetLogVerbosity()
{
    return Coffee::PrintingVerbosityLevel;
}
