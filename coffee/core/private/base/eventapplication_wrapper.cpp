#include <stdint.h>
#include <stdio.h>

#include <coffee/core/plat/plat_quirks_toggling.h>
#include <coffee/core/base/renderer/eventapplication_wrapper.h>

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/input/cinput.h>

#if !defined(COFFEE_INJECTS_EVENTS_EXTERNALLY)
void* coffee_event_handling_data;

void(*CoffeeEventHandle)(void*, int);
void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);

void(*CoffeeForeignSignalHandle)(int);
void(*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);
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

void CfResizeHandler(EventApplication* app, int /*event*/,
                     void* /*p1*/, void* p2, void* /*p3*/)
{
    CfResizeEventData* evdata = C_FCAST<CfResizeEventData*>(p2);
    
    CDEvent dEv = {};
    dEv.type = CDEvent::Resize;
    
    CDResizeEvent rEv = {C_FCAST<i32>(evdata->w), C_FCAST<i32>(evdata->h)};
    
    app->injectEvent(dEv, &rEv);
}

void CfTouchHandler(EventApplication* app, int /*event*/,
                    void* /*p1*/, void* p2, void* /*p3*/)
{
    CfTouchEventData* evdata = C_FCAST<CfTouchEventData*>(p2);
    
    CIEvent iEv = {};
    
    CITouchTapEvent tap;
    CIMTouchMotionEvent pan;
    CITouchPinchEvent pinch;
    CITouchRotateEvent rotate;
    
    c_cptr data = nullptr;
    
    switch(evdata->type)
    {
        case CfTouchTap:
        {
            iEv.type = CIEvent::TouchTap;
            data = &tap;
            
            tap.pos.x = evdata->event.tap.x;
            tap.pos.y = evdata->event.tap.y;
            
            break;
        }
        case CfTouchPan:
        {
            iEv.type = CIEvent::TouchPan;
            data = &pan;
        
            pan.origin.x = evdata->event.pan.ox;
            pan.origin.y = evdata->event.pan.oy;
            
            pan.translation.x = evdata->event.pan.dx;
            pan.translation.y = evdata->event.pan.dy;
            
            pan.velocity.x = evdata->event.pan.vx;
            pan.velocity.y = evdata->event.pan.vy;
            
            pan.fingers = evdata->event.pan.fingerCount;
            break;
        }
        case CfTouchPinch:
        {
            iEv.type = CIEvent::TouchPinch;
            data = &pinch;
        
            pinch.origin.x = evdata->event.pinch.x;
            pinch.origin.y = evdata->event.pinch.y;
            
            pinch.factor = evdata->event.pinch.factor;
            break;
        }
        case CfTouchRotate:
        {
            iEv.type = CIEvent::TouchRotate;
            data = &rotate;
            
            rotate.origin.x = evdata->event.rotate.x;
            rotate.origin.y = evdata->event.rotate.y;
            
            rotate.radians = evdata->event.rotate.radians;
            break;
        }
    default:
        return;
    }
    
    app->injectEvent(iEv, data);
}

}
}
