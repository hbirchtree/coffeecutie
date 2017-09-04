#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Events sent from foreign code to Coffee's event loop
enum CoffeeEventHandleType
{
    // Basic functions, pre-determined handling
    CoffeeHandle_Setup = 1,
    CoffeeHandle_Loop,
    CoffeeHandle_Cleanup,
    
    // Lifecycle events
    CoffeeHandle_LowMem,
    
    CoffeeHandle_IsTerminating,
    CoffeeHandle_IsBackground,
    CoffeeHandle_IsForeground,
    CoffeeHandle_TransBackground,
    CoffeeHandle_TransForeground,
    
    CoffeeHandle_GeneralEvent,
};

// The int will be CoffeeEventHandleType
extern void(*CoffeeEventHandle)(void*, int);
// For non-atomic events
extern void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);
// User pointer for event handling, references Coffee's objects
// This is done such that the Coffee code does not need extra spice to work
// ... And we only need to link to a valid library for it.
extern void* coffee_event_handling_data;

// Wrapper for simpler usage, assuming that the foreign part supports C code
inline bool CoffeeEventHandleCall(int event)
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
inline bool CoffeeEventHandleNACall(int event, void* ptr1,
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

/* Now, for interaction from Coffee to foreign code */
/* May not run on the same thread at all times */
enum CoffeeForeignSignal
{
    /* No extra data */
    CoffeeForeign_DidFinish = 1, // Signaling that the other side got a sock
    /* Protocol for displaying a message:
     * ptr1 = title
     * ptr2 = message
     * ptr3 = (optional) CfMessageDisplay struct, for event handling
     */
    CoffeeForeign_DisplayMessage, // Will be coupled with a C-string or two
    /* ptr1 = pointer to 2-sized int array */
    CoffeeForeign_GetWinSize,
    
    /* Get EGL details */
    CoffeeForeing_GetEGLDisplay,
    CoffeeForeing_GetEGLSurface,
};

// Same rules, except from Coffee to foreign code
// This side does not have a user pointer, as it is not as interchangeable
extern void(*CoffeeForeignSignalHandle)(int);
extern void(*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);

/* For displaying a message
 * Data used by the message actions should be available even
 *  after event has been sent, because the actual events may
 *  be triggered much later.
 */
struct CfMessageAction
{
    const char* text;
    void(*action)(void*);
};
struct CfMessageDisplay
{
    void* user_ptr;
    void(*completion)(void*);
    struct CfMessageAction* actions;
};

#ifdef __cplusplus
}
#endif
