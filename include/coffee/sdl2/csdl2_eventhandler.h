#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/base/renderer/hapticapplication.h>
#include "csdl2_context.h"

namespace Coffee{
namespace CDisplay{

class SDL2EventHandler :
        public virtual EventApplication,
        public HapticApplication,
        public virtual SDL2ContextUser
{
    // InputApplication interface
public:
    void inputPreInit();
    void inputInit();
    void inputPostInit();
    void inputTerminate();

    virtual void eventHandleD(const CDEvent &, c_cptr);
    virtual void eventHandleI(const CIEvent &event, c_cptr data);

    CIControllerState getControllerState(uint16 index);

    bool isMouseGrabbed() const;
    void setMouseGrabbing(bool m);

    CPoint mousePosition() const;
    void setMousePosition(const CPoint &p);

    bool relativeMouse() const;
    void setRelativeMouse(bool enable);

    void setKeyboardRepeat(bool m);

    bool textInputMode() const;
    void setTextInputMode(bool m);

    void setTextArea(const CRect &area);

    void eventHandle(const CIHapticEvent& haptic, c_cptr data)
    {
        this->hapticInsert(haptic,data);
    }
    void eventHandle(const CIEvent &event, c_cptr data)
    {
        this->eventHandleI(event,data);
    }
    void eventHandle(const CDEvent &event, c_cptr data)
    {
        this->eventHandleD(event,data);
    }

    // HapticApplication interface
public:
    void hapticInsert(const CIHapticEvent &e, c_cptr data);

    // EventApplication interface
public:
    void pollEvents();
    bigscalar contextTime() const;
    bool closeFlag() const;
};

}
}