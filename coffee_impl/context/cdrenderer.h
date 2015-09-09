#ifndef COFFEE_CDISPLAY_CDRENDERER_H
#define COFFEE_CDISPLAY_CDRENDERER_H

#include "cglbindingrenderer.h"

namespace Coffee {

namespace CGraphicsWrappers{
struct CGLState;
}

namespace CDisplay {

class CDRenderer : public CGLBindingRenderer
{
public:
    CDRenderer(CObject* parent = nullptr);

    void run();

    void run(WindowState state, CSize resolution, int monitor = 0);

    void bindingCallback(CGLReport* report) const;

    void eventWHandle(CDEvent *event);
    void eventIHandle(CIEvent *event);
private:
    CGLState* _dump_state() const;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERER_H
