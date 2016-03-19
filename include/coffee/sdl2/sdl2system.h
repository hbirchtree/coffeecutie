#pragma once
#include <coffee/core/types/subsystem.h>

namespace Coffee{
namespace SDL2{

struct SDL2 : Subsystem
{
    static const constexpr cstring SubsystemName = "SDL2";

    static bool Init();
    static bool Deinit();

    static bool Process();
};

}
}