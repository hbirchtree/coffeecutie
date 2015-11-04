#ifndef COFFEE_CDISPLAY_CDRENDERER_H
#define COFFEE_CDISPLAY_CDRENDERER_H

#include <coffee/core/context/cglbindingrenderer.h>

namespace Coffee {


namespace CMemoryManagement{
struct game_context;
}

using namespace CMemoryManagement;

namespace CDisplay {


class CDRenderer : public CGLBindingRenderer
{
public:
    CDRenderer(CObject* parent = nullptr);
    ~CDRenderer();

    void run();

    /*!
     * \brief Launches the full context, init, run, cleanup
     * \param props
     */
    void run(const CDWindowProperties &props);

    void eventWindowsHandle(const CDEvent *event);
    void eventInputHandle(const CIEvent *event);

private:
    game_context* game = nullptr;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERER_H
