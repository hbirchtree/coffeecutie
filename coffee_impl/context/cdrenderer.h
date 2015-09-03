#ifndef COFFEE_CDISPLAY_CDRENDERER_H
#define COFFEE_CDISPLAY_CDRENDERER_H

#include "coffee_impl/display/cglfwrenderer.h"
#include "coffee_impl/functional/cqueuerunner.h"

namespace Coffee {
namespace CDisplay {

class CDRenderer : public CGLFWRenderer
{
public:
    CDRenderer(CObject* parent = nullptr);

    void run();

    void run(WindowState state, CSize resolution, int monitor = 0);

    void glbindingCallbackInternal(CGLReport* report) const;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERER_H
