#pragma once

#include <coffee/core/types/subsystem.h>

namespace Coffee{
namespace CEF{

struct CEFSubsystem : Subsystem
{
    static const constexpr cstring SubsystemName = "CEF";

    static bool Init();
    static bool Deinit();

    static bool Process();
};

class CEFBrowser;

extern CEFBrowser* CreateBrowser();
extern void DestroyBrowser(CEFBrowser* b);

}
}