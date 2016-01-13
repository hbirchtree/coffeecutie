#ifndef COFFEE_CORE_STRINGS_H
#define COFFEE_CORE_STRINGS_H

#include <types/basetypes.h>

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
const constexpr cstring Graphics_GLeam_Renderer_FailLoad =
	"Failed to load OpenGL function pointers! Printer on fire! Prepare to crash!";

const constexpr cstring Graphics_GLeam_Library_Name = "GLeamShim";
const constexpr cstring Graphics_GLeam_Library_CoreVersionError =
        "Unsuitable core version detected: {0}, target was {1}";

const constexpr cstring Graphics_VR_Library_Name = "COpenVR";
const constexpr cstring Graphics_VR_Library_InitError =
        "Failed to initialize VR runtime: {0}";
const constexpr cstring Graphics_VR_Library_RModelsError =
        "Failed to acquire render model interface: {0}";
const constexpr cstring Graphics_VR_Library_CurrencyError =
        "Cannot replace existing thread context, please acquire the existing one first";
const constexpr cstring Graphics_VR_Library_CompositeError =
        "Failed to initialize compositor, see SteamVR logs for details";

const constexpr cstring Media_FF_Name = "CFFMedia";
const constexpr cstring Media_FF_SWR_InitError = "Failed to initialize software resampler: {0}";

}

#endif
