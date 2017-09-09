#include <stdint.h>
#include <stdio.h>

#include <coffee/core/plat/plat_quirks_toggling.h>
#include <coffee/core/base/renderer/eventapplication_wrapper.h>

#include <coffee/core/base/renderer/eventapplication.h>

#if !defined(COFFEE_INJECTS_EVENTS_EXTERNALLY)
void(*CoffeeEventHandle)(void*, int);
void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);
void* coffee_event_handling_data;
#endif

bool CoffeeEventHandleCall(int event)
{
    if(CoffeeEventHandle && coffee_event_handling_data)
    {
        CoffeeEventHandle(coffee_event_handling_data, event);
        return true;
    }else{
        fprintf(stderr,
                "Event handler function called without valid configuration!\n");
        return false;
    }
}
bool CoffeeEventHandleNACall(int event, void* ptr1,
                                    void* ptr2, void* ptr3)
{
    if(CoffeeEventHandleNA && coffee_event_handling_data)
    {
        CoffeeEventHandleNA(coffee_event_handling_data,
                            event, ptr1, ptr2, ptr3);
        return true;
    }else{
        fprintf(stderr,
                "Event handler function called without valid configuration!\n");
        return false;
    }
}

namespace Coffee{
namespace CfAdaptors{

void CfResizeHandler(EventApplication* app, int event,
                     void* p1, void* p2, void* p3)
{
    CfResizeEventData* evdata = C_FCAST<CfResizeEventData*>(p2);
    
    CDEvent dEv = {};
    dEv.type = CDEvent::Resize;
    
    CDResizeEvent rEv = {C_FCAST<i32>(evdata->w), C_FCAST<i32>(evdata->h)};
    
    app->injectEvent(dEv, &rEv);
}

}
}
