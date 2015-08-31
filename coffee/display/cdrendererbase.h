#ifndef COFFEE_CDISPLAY_CDRENDERERBASE_H
#define COFFEE_CDISPLAY_CDRENDERERBASE_H

#include "coffee.h"
#include "coffee/cobject.h"

using namespace Coffee::CInput;

namespace Coffee {
namespace CDisplay {

class CDRendererBase : public CObject
{
public:
    CDRendererBase(CObject* parent);

    //Window-related
    virtual CString windowTitle() const = 0;
    virtual void setWindowTitle(const CString &title) = 0;

    virtual bool showWindow() = 0; //bool: whether it succeeded or not, can be a static value
    virtual bool hideWindow() = 0;

    //Framebuffer size of context
    virtual CSize framebufferSize() const = 0;
    virtual void setFramebufferSize(const CSize &size) = 0;

    //Mouse controls, managed by context
    virtual bool isMouseGrabbed() const = 0;
    virtual void setMouseGrabbing(bool grab) = 0;

    //Render loop flow
    virtual void swapBuffers() = 0;
    virtual void pollEvents() = 0;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERERBASE_H
