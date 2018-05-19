#include <coffee/graphics/apis/gleam/gleam.h>

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
#include <coffee/graphics/apis/gleam/levels/desktop/glbase.h>
#else
#include <coffee/graphics/apis/gleam/levels/es/glbase.h>
#endif

namespace Coffee{
namespace CGL{

Coffee::CString CGL_Shared_Debug::s_ExtensionList = "";
bool CGL_Shared_Debug::b_isDebugging = false;

Coffee::int32 CGL_Shared_Debug::Num_Internal_Formats = 0;
Coffee::int32* CGL_Shared_Debug::Internal_Formats = nullptr;

}
}
