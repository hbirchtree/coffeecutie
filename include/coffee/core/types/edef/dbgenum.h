#pragma once

#include "../tdef/integertypes.h"

namespace Coffee{

enum class Severity : uint8
{
    Information,
    Debug,

    Low,
    Medium,
    High,

    Critical,
    Fatal,
};

enum class DebugType : uint8
{
    UndefinedBehavior,
    Information,
    Performance,
    Compliance,
    Deprecated,
    Compatibility,
    Marker,
    Other,
};

enum class DebugComponent : uint8
{
    Core,
    GraphicsAPI,
    Audio,
    Interface,
    ShaderCompiler,
    LibraryLoader,
    Media,

    Extension,
};

enum class ProfilingTerm
{
    ElapsedTime,
    ScreenSamples,
    ScreenPrimitives,
    TransformedPrimitives,
};

}