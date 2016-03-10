#ifndef COFFEE_CORE_STRINGS_H
#define COFFEE_CORE_STRINGS_H

#include "types/tdef/integertypes.h"

namespace CFStrings
{

//TODO: Move all library strings here

using Coffee::cstring;

const constexpr cstring Plat_Unix_Core_Lib = "UNIX core";
const constexpr cstring Plat_Cpu_Affinity_Error = "Failed to set thread affinity!";

const constexpr cstring Plat_File_Open_Error = "Failed to open file: {0}";
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

const constexpr cstring ASIO_Library_Name = "CASIO";
const constexpr cstring ASIO_Library_MakeCurrent_Error = "Failed to set thread context: Existing context in place";

const constexpr cstring CNect_Library_Name = "CNect";
const constexpr cstring CNect_Loaded = "Opened device with ID {0}";
const constexpr cstring CNect_Log_Msg = "{0}";
const constexpr cstring CNect_NumDevices = "Number of devices: {0}";
const constexpr cstring CNect_NoDevs = "No devices detected";
const constexpr cstring CNect_IndexFlt = "Invalid device index";
const constexpr cstring CNect_AsyncStart = "CNect loop started";
const constexpr cstring CNect_AsyncStop = "CNect loop closed";

const constexpr cstring SDL2_Library_Name = "SDL2";
const constexpr cstring SDL2_Library_FailureInit = "Failed to initialize SDL2 context: {0}";

}

#endif
