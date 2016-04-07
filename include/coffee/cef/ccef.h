#pragma once

#include <coffee/core/types/subsystem.h>
#include <coffee/core/CInput>
#include <coffee/core/CDisplay>

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

enum class BAction
{
    Nothing,
    Back,Forward,
    Refresh,Stop,
};

extern CEFBrowser* CreateBrowser(cstring start_url = "google.com");

extern void SetUrl(CEFBrowser* b,cstring url);
extern void PerformAction(CEFBrowser* b, BAction action);

extern void AwaitLoaded(CEFBrowser* b);
extern bool IsLoaded(CEFBrowser* b);
extern void Resize(CEFBrowser* b, const CSize &size);

extern void ProcessEvent(CEFBrowser* b,CInput::CIEvent const&e,c_cptr data);
extern void ProcessEvent(CEFBrowser* b, Display::CDEvent const&e, c_cptr data);

extern void DestroyBrowser(CEFBrowser* b);

}
}