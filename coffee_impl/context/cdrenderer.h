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
    ~CDRenderer();

    void run();

    void run(CDWindowProperties props);

    void bindingCallback(CGLReport* report) const;

    void eventWHandle(const CDEvent *event);
    void eventIHandle(const CIEvent *event);
private:
    CGLState* _dump_state() const;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERER_H
