#ifndef COFFEE_CORE_STRINGS_H
#define COFFEE_CORE_STRINGS_H

#include <coffee/core/types/basetypes.h>

namespace CFStrings
{

//TODO: Move all library strings here

using namespace Coffee;

const constexpr cstring Plat_File_Native_Error = "Native file error: {0}";
const constexpr cstring Plat_File_Native_SizeErr = "Read error: expected {0} bytes, got {1}";

const constexpr cstring Lib_Identifier = "CObjectLoader";

const constexpr cstring Lib_load_error_format = "Native library loading error: {0}";
const constexpr cstring Lib_symb_error_format = "Native symbol resolution error: {0}";

const constexpr cstring Graphics_GLeam_Renderer_Name = "GLeamRenderer";
const constexpr cstring Graphics_GLeam_Renderer_FailLoad = "Failed to load OpenGL function pointers! Prepare to crash!";

}

#endif
