#if !defined(COFFAPI)
#define COFFAPI
#endif

#if !defined(FORCEDINLINE)
#define FORCEDINLINE inline
#endif

#if defined(SWIG)
#include <coffee/core/types/cdef/geometry.h>
#endif

COFFAPI Coffee::CObject& GetRootObject();

COFFAPI void Init();
COFFAPI void Terminate();

/* Logging */
namespace Coffee{
COFFAPI void cDebug(const char* text);
COFFAPI void cWarning(const char* text);
COFFAPI void cFatal(const char* text);
COFFAPI void cLog(const char* id, const char* msg);
}

COFFAPI int GetLogVerbosity();
COFFAPI void SetLogVerbosity(int level);

/* Generating GL window */
COFFAPI SDL2Renderer* CreateWindow(Props const& props, Coffee::CObject* p = 0);
COFFAPI void DestroyWindow(SDL2Renderer* p);

/* Controlling GL window */
COFFAPI void ShowWindow(SDL2Renderer* p);
COFFAPI void HideWindow(SDL2Renderer* p);
COFFAPI bool ShouldClose(SDL2Renderer* p);
COFFAPI void CloseWindow(SDL2Renderer* p);

COFFAPI void Process(SDL2Renderer* p);

COFFAPI void SetWindowTitle(SDL2Renderer* p, const char* title);
COFFAPI void SetWindowSize(SDL2Renderer* p, int w, int h);
COFFAPI void SetWindowPosition(SDL2Renderer* p, int x, int y);

COFFAPI GLContext *GetContext(SDL2Renderer* p);

/* Rendering functions */
COFFAPI void ClearBuffer(float dep, float color[4]);

/* Profiling functions */
COFFAPI void Push(const char* name);
COFFAPI void Pop();
COFFAPI void Tag(const char* name);
COFFAPI void LabelThread(const char* name);
COFFAPI void Save(const char* outfile);
COFFAPI void PrintData();
