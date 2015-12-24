#ifndef COFFEE_CDISPLAY_CDRENDERER_H
#define COFFEE_CDISPLAY_CDRENDERER_H

#include <coffee/graphics_apis/COpenGL>

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

    using CSDL2Renderer::eventHandle;

    void eventHandleD(const CDEvent &event, c_cptr data);
    void eventHandleI(const CIEvent &event, c_cptr data);

private:
    game_context* game = nullptr;
};

} // namespace CDisplay
} // namespace Coffee

#endif // COFFEE_CDISPLAY_CDRENDERER_H
