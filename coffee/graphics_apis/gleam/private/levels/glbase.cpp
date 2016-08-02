#include <coffee/graphics_apis/gleam/gleam.h>

#ifdef COFFEE_GLEAM_DESKTOP
#include <coffee/graphics_apis/gleam/levels/desktop/glbase.h>
#else
#include <coffee/graphics_apis/gleam/levels/es/glbase.h>
#endif
namespace Coffee{
namespace CGL{

Coffee::CString CGL_Shared_Debug::s_ExtensionList = "";
bool CGL_Shared_Debug::b_isDebugging = false;

#ifdef COFFEE_GLEAM_DESKTOP
Coffee::XML::Document* CGLXML::doc = nullptr;
Coffee::Mutex CGLXML::doc_mutex;
#else
Coffee::int32 CGL_Shared_Debug::Num_Internal_Formats = 0;
Coffee::int32* CGL_Shared_Debug::Internal_Formats = nullptr;
#endif

}
}
