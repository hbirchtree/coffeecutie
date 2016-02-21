#include <coffee/graphics_apis/gleam/gleam.h>

#ifdef COFFEE_GLEAM_DESKTOP
#include <coffee/graphics_apis/gleam/levels/desktop/glbase.h>
#else
#include <coffee/graphics_apis/gleam/levels/es/glbase.h>
#endif

Coffee::CString Coffee::CGL::CGL_Implementation::Debug::s_ExtensionList = "";
bool Coffee::CGL::CGL_Implementation::Debug::b_isDebugging = false;

#ifdef COFFEE_GLEAM_DESKTOP
Coffee::XML::Document* Coffee::CGL::CGLXML::doc = nullptr;
Coffee::Mutex Coffee::CGL::CGLXML::doc_mutex;
#endif