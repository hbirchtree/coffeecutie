namespace Coffee
{
/* Class declarations */
class CObject;

namespace CGL{
struct CGL_Context;
}

/* Wrapper types */
class CGLWindow;

extern void CoffeeInit();
extern void CoffeeTerminate();

/* Logging */
extern void cDebug(const char* text);
extern void cWarning(const char* text);
extern void cFatal(const char* text);
extern void cLog(const char* id, const char* msg);

/* Generating GL window */
namespace Window{
extern CGLWindow* CreateWindow(int w, int h,
                               int major = 3, int minor = 3,
                               bool debug = false,
                               CObject* p = 0);
extern void DestroyWindow(CGLWindow* p);

/* Controlling GL window */
extern void ShowWindow(CGLWindow* p);
extern void HideWindow(CGLWindow* p);
extern bool ShouldClose(CGLWindow* p);
extern void CloseWindow(CGLWindow* p);

extern void Process(CGLWindow* p);

extern void SetWindowTitle(CGLWindow* p, const char* title);
extern void SetWindowSize(CGLWindow* p, int w, int h);
extern void SetWindowPosition(CGLWindow* p, int x, int y);

extern CGL::CGL_Context* GetContext(CGLWindow* p);

}

namespace CGL{

extern void ClearBuffer(float dep, float color[4]);

}

}