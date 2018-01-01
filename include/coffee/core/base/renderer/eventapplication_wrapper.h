#pragma once

#include <stdlib.h>
#include <stdint.h>

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
    CoffeeHandle_SensorEvent,
};

// The int will be CoffeeEventHandleType
extern void(*CoffeeEventHandle)(void*, int);
// For non-atomic events
extern void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);
// User pointer for event handling, references Coffee's objects
// This is done such that the Coffee code does not need extra spice to work
// ... And we only need to link to a valid library for it.
extern void* coffee_event_handling_data;

enum CfGeneralEventType
{
    CfNullEvent,
    CfResizeEvent = 1,
    
    CfTouchEvent,
};

enum CfTouchType
{
    CfTouch_None,
    CfTouchTap,
    CfTouchPan,
    CfTouchPinch,
    CfTouchRotate,
};
    
struct CfGeneralEvent
{
        enum CfGeneralEventType type;
        uint32_t pad;
};
    
struct CfTouchEventData
{
    enum CfTouchType type;

    union {
        struct CfTouchTapEventData
        {
            uint32_t x, y;
            uint8_t doubleTap;
            uint32_t pad:24;
        } tap;
        struct CfTouchPanEventData
        {
            uint32_t ox, oy;
            int32_t dx, dy;
            int32_t vx, vy;
            uint32_t fingerCount;
        } pan;
        struct CfTouchPinchEventData
        {
            uint32_t x, y;
            float factor;
        } pinch;
        struct CfTouchRotateEventData
        {
            uint32_t x, y;
            float radians;
        } rotate;
    } event;
};
    
struct CfResizeEventData
{
    uint32_t w, h;
};

enum CfSensorType
{
    CfSensor_Accel = 0x1,
    CfSensor_Gyro = 0x2,
    
    CfSensor_Orientation = 0x4,
    CfSensor_Gravity = 0x8,
    
    CfSensor_Magnetometer = 0x10,
    
    CfSensor_Lux = 0x20,
};

struct CfSensorEvent
{
    union {
        struct {
            int32_t x;
            int32_t y;
            int32_t z;
            int32_t w;
        } ivec4;
        struct {
            float x;
            float y;
            float z;
            float w;
        } fvec4;
    };
};

// Wrapper for simpler usage, assuming that the foreign part supports C code
extern bool CoffeeEventHandleCall(int event);
extern bool CoffeeEventHandleNACall(int event, void* ptr1, void* ptr2, void* ptr3);

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
    /* ptr1 = pointer to 4-sized float array, [left, top, right, bottom] */
    CoffeeForeign_GetSafeMargins,
    
    CoffeeForeign_ActivateMotion,

    CoffeeForeign_RequestPlatformData, /* Platform-specific data requests */
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

#ifdef __cplusplus
namespace Coffee{
namespace Display{
class EventApplication;
}

namespace CfAdaptors{

using namespace Display;

struct CfAdaptor
{
    CfGeneralEventType type;
    void(*func)(EventApplication*, int event, void*, void*, void*);
};

extern void CfResizeHandler(EventApplication* app, int event,
                     void* p1, void* p2, void* p3);

extern void CfTouchHandler(EventApplication* app, int event,
                           void* p1, void* p2, void* p3);

}
}
#endif
