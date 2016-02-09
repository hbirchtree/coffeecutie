#include <coffee/CCore>
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
void ClearBuffer(float dep, float color[4])
{
    CGL33::ClearBufferfv(false,0,&dep);
    CGL33::ClearBufferfv(true,0,color);
}

}
}