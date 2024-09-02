#include <coffee/imgui/imgui_binding.h>

#include <coffee/core/types/input/event_types.h>
#include <coffee/core/types/input/keymap.h>

#include <map>

#include <coffee/core/CDebug>

namespace imgui::detail {

using Coffee::cDebug;
using libc_types::u16;

using namespace typing::vector_types;
using namespace Coffee::Input;

void ImGuiSystem::setup_inputs(Proxy& e)
{
    auto ibus = e.service<comp_app::BasicEventBus<CIEvent>>();

    m_keymap = {
        {CK_HTab, ImGuiKey_Tab},
        {CK_Left, ImGuiKey_LeftArrow},
        {CK_Right, ImGuiKey_RightArrow},
        {CK_Up, ImGuiKey_UpArrow},
        {CK_Down, ImGuiKey_DownArrow},
        {CK_PgUp, ImGuiKey_PageUp},
        {CK_PgDn, ImGuiKey_PageDown},
        {CK_Home, ImGuiKey_Home},
        {CK_End, ImGuiKey_End},
        {CK_BackSpace, ImGuiKey_Backspace},
        {CK_Delete, ImGuiKey_Delete},
        {CK_EnterCR, ImGuiKey_Enter},
        {CK_EnterNL, ImGuiKey_Enter},
        {CK_Escape, ImGuiKey_Escape},
        {CK_a, ImGuiKey_A},
        {CK_c, ImGuiKey_C},
        {CK_v, ImGuiKey_V},
        {CK_x, ImGuiKey_X},
        {CK_y, ImGuiKey_Y},
        {CK_z, ImGuiKey_Z},
    };

    ibus->addEventFunction<CIScrollEvent>(
        0, [](CIEvent& e, CIScrollEvent* scroll) {
            ImGuiIO& io = ImGui::GetIO();
            io.AddMouseWheelEvent(scroll->delta.x, scroll->delta.y);
            if(io.WantCaptureMouseUnlessPopupClose)
                e.type = CIEvent::NoneType;
        });
    ibus->addEventFunction<CIMouseMoveEvent>(
        0, [](CIEvent& e, CIMouseMoveEvent* mouse) {
            ImGuiIO& io = ImGui::GetIO();
            io.AddMousePosEvent(
                (mouse->origin[0] + mouse->delta[0]) /
                    io.DisplayFramebufferScale.x,
                (mouse->origin[1] + mouse->delta[1]) /
                    io.DisplayFramebufferScale.x);
            if(io.WantCaptureMouseUnlessPopupClose)
                e.type = CIEvent::NoneType;
        });
    ibus->addEventFunction<CIMouseButtonEvent>(
        0, [](CIEvent& e, CIMouseButtonEvent* mouse) {
            ImGuiIO& io = ImGui::GetIO();

            u32 index = 0;
            switch(mouse->btn)
            {
            case CIMouseButtonEvent::LeftButton:
                index = 0;
                break;
            case CIMouseButtonEvent::MiddleButton:
                index = 1;
                break;
            case CIMouseButtonEvent::RightButton:
                index = 2;
                break;
            default:
                break;
            }
            io.AddMousePosEvent(
                mouse->pos[0] / io.DisplayFramebufferScale.x,
                mouse->pos[1] / io.DisplayFramebufferScale.x);
            io.AddMouseButtonEvent(
                index,
                static_cast<bool>(mouse->mod & CIMouseButtonEvent::Pressed));
            if(io.WantCaptureMouseUnlessPopupClose)
                e.type = CIEvent::NoneType;
        });
    ibus->addEventFunction<CIWriteEvent>(0, [](CIEvent&, CIWriteEvent* write) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharactersUTF8(write->text.c_str());
    });
    auto* keymap = &m_keymap;
    ibus->addEventFunction<CIKeyEvent>(
        0, [keymap](CIEvent& e, CIKeyEvent* key) {
            if(!keymap->contains(key->key))
                return;

            ImGuiIO& io = ImGui::GetIO();
            io.AddKeyEvent(
                (*keymap)[key->key], key->mod & CIKeyEvent::PressedModifier);

            if(io.WantCaptureMouseUnlessPopupClose)
                e.type = CIEvent::NoneType;
        });
}

} // namespace imgui::detail
