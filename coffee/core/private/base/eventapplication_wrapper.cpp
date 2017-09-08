#include <stdint.h>
#include <stdio.h>

#include <coffee/core/plat/plat_quirks_toggling.h>
#include <coffee/core/base/renderer/eventapplication_wrapper.h>

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
    if(CoffeeEventHandle && coffee_event_handling_data)
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
