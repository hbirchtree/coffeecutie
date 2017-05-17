#ifndef COFFEE_CDISPLAY_CDRENDERERBASE_H
#define COFFEE_CDISPLAY_CDRENDERERBASE_H

#include "../types/cobject.h"

#include "eventapplication.h"
#include "glapplication.h"
#include "hapticapplication.h"
#include "initapplication.h"
#include "inputapplication.h"
#include "windowmanagerclient.h"

namespace Coffee {

using namespace CInput;

namespace Display {

/*!
 * \brief The base class for creating a rendering context. It will handle window events, input events and creating a GL context for the user. The user will then work on top of this stack with Coffee-defined events and classes.
 */
class CDRendererBase :
        public virtual EventApplication,
        public virtual InitApplication,
        public CObject
{
protected:
    CDRendererBase(CObject *parent) : CObject(parent)
    {
    }
public:
    virtual ~CDRendererBase()
    {
    }
};

template<class T>
FORCEDINLINE Future<void> LaunchAsync(T* r, CDProperties const& p)
{
    Function<void()> fn = [r,p]()
    {
        CString err;
        r->init(p,&err);
        r->run();
        r->cleanup();
    };
    return Threads::RunAsync(fn);
}

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERERBASE_H
