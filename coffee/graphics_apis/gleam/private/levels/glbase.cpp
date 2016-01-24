#include <levels/glbase.h>

Coffee::CString Coffee::CGL::CGL_Implementation::Debug::s_ExtensionList = "";
bool Coffee::CGL::CGL_Implementation::Debug::b_isDebugging = false;
Coffee::XML::Document* Coffee::CGL::CGLXML::doc = nullptr;
Coffee::Mutex Coffee::CGL::CGLXML::doc_mutex;