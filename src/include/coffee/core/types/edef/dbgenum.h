#pragma once

#include <peripherals/libc/types.h>

namespace Coffee{

enum class Severity : uint8
{
    Verbose,
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

}
