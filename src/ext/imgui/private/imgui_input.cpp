#include <coffee/imgui/imgui_binding.h>

#include <coffee/core/types/input/event_types.h>
#include <coffee/core/types/input/keymap.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

namespace imgui::detail {

using Coffee::cDebug;
using libc_types::u16;

using namespace typing::vector_types;
using namespace Coffee::Input;

static constexpr std::array<std::pair<ImGuiKey, u16>, 19> ImKeyMap = {{
    {ImGuiKey_Tab, CK_HTab},
    {ImGuiKey_LeftArrow, CK_Left},
    {ImGuiKey_RightArrow, CK_Right},
    {ImGuiKey_UpArrow, CK_Up},
    {ImGuiKey_DownArrow, CK_Down},
    {ImGuiKey_PageUp, CK_PgUp},
    {ImGuiKey_PageDown, CK_PgDn},
    {ImGuiKey_Home, CK_Home},
    {ImGuiKey_End, CK_End},
    {ImGuiKey_Backspace, CK_BackSpace},
    {ImGuiKey_Delete, CK_Delete},
    {ImGuiKey_Enter, CK_EnterNL},
    {ImGuiKey_Escape, CK_Escape},
    {ImGuiKey_A, CK_a},
    {ImGuiKey_C, CK_c},
    {ImGuiKey_V, CK_v},
    {ImGuiKey_X, CK_x},
    {ImGuiKey_Y, CK_y},
    {ImGuiKey_Z, CK_z},
}};

C_UNUSED(STATICINLINE ImGuiKey CfToImKey(u32 k))
{
    auto it = std::find_if(
        ImKeyMap.begin(),
        ImKeyMap.end(),
        [k](std::pair<ImGuiKey, u16> const& p) { return p.second == k; });
    if(it != ImKeyMap.end())
        return it->first;
    return ImGuiKey::ImGuiKey_None;
}

C_UNUSED(STATICINLINE u32 ImToCfKey(ImGuiKey k))
{
    auto it = std::find_if(
        ImKeyMap.begin(),
        ImKeyMap.end(),
        [k](std::pair<ImGuiKey, u16> const& p) { return p.first == k; });
    if(it != ImKeyMap.end())
        return it->second;
    return 0;
}

// template<typename T>
// inline T const& C(c_cptr d)
//{
//     return *(C_FCAST<T const*>(d));
// }

// void ImGui_InputHandle(CIEvent& ev, c_ptr data)
//{
//     auto io      = &ImGui::GetIO();
//     bool handled = true;

//    switch(ev.type)
//    {
//    case CIEvent::TouchPan: {
//        CIMTouchMotionEvent const& pan = C<CIMTouchMotionEvent>(data);

//        io->MousePos = {
//            (pan.origin.x + pan.translation.x) /
//            io->DisplayFramebufferScale.x, (pan.origin.y +
//            pan.translation.y) / io->DisplayFramebufferScale.y};

//        auto btn = CIMouseButtonEvent::LeftButton;

//        io->MouseDown[btn - 1]       = true;
//        io->MouseClickedPos[btn - 1] = io->MousePos;

//        break;
//    }
//    case CIEvent::TouchTap: {
//        CITouchTapEvent const& tap = C<CITouchTapEvent>(data);

//        io->MouseDown[CIMouseButtonEvent::LeftButton - 1]       =
//        tap.pressed; io->MouseClickedPos[CIMouseButtonEvent::LeftButton -
//        1] = {
//            tap.pos.x / io->DisplayFramebufferScale.x,
//            tap.pos.y / io->DisplayFramebufferScale.y};

//        break;
//    }
//    case CIEvent::MouseButton: {
//        auto ev = C<CIMouseButtonEvent>(data);
//        if(ev.btn < 5 && ev.btn > 0)
//        {
//            bool flag                 = ev.mod &
//            CIMouseButtonEvent::Pressed; io->MouseDown[ev.btn - 1] = flag;
//            io->MouseClickedPos[ev.btn - 1] = {ev.pos.x, ev.pos.y};
//        }
//        break;
//    }
//    case CIEvent::MouseMove: {
//        auto ev      = C<CIMouseMoveEvent>(data);
//        io->MousePos = {
//            (ev.origin.x + ev.delta.x) / io->DisplayFramebufferScale.x,
//            (ev.origin.y + ev.delta.y) / io->DisplayFramebufferScale.y};

//        break;
//    }
//    case CIEvent::Scroll: {
//        auto ev        = C<CIScrollEvent>(data);
//        io->MouseWheel = ev.delta.y;
//        break;
//    }
//    case CIEvent::Keyboard: {
//        auto ev = C<CIKeyEvent>(data);
//        if(ev.key < 512)
//        {
//            if(((ev.key >= CK_a && ev.key <= CK_z) ||
//                (ev.key >= CK_A && ev.key <= CK_Z) ||
//                (ev.key >= CK_0 && ev.key <= CK_9)) &&
//               (ev.mod & CIKeyEvent::RepeatedModifier ||
//                ev.mod & CIKeyEvent::PressedModifier))
//                io->AddInputCharacter(C_CAST<ImWchar>(ev.key));

//            io->KeysDown[ev.key] = ev.mod & CIKeyEvent::PressedModifier;

//            //            io->KeyAlt   = ev.mod &
//            CIKeyEvent::LAltModifier;
//            //            io->KeyCtrl  = ev.mod &
//            CIKeyEvent::LCtrlModifier;
//            //            io->KeyShift = ev.mod &
//            CIKeyEvent::LShiftModifier;
//            //            io->KeySuper = ev.mod &
//            CIKeyEvent::SuperModifier;

//            switch(ev.key)
//            {
//            case CK_LShift:
//            case CK_RShift:
//                io->KeyShift = true;
//                break;
//            case CK_LCtrl:
//            case CK_RCtrl:
//                io->KeyCtrl = true;
//                break;
//            case CK_AltGr:
//            case CK_LAlt:
//                io->KeyAlt = true;
//                break;
//            case CK_LSuper:
//            case CK_RSuper:
//                io->KeyShift = true;
//                break;
//            }
//        }
//        break;
//    }
//    case CIEvent::TextInput: {
//        auto ev = C<CIWriteEvent>(data);
//        io->AddInputCharactersUTF8(ev.text);
//        break;
//    }
//    case CIEvent::TextEdit: {
//        auto ev = C<CIWEditEvent>(data);
//        io->AddInputCharactersUTF8(ev.text);
//        break;
//    }
//    default:
//        handled = false;
//        break;
//    }

//    if(handled && (io->WantCaptureMouse || io->WantCaptureKeyboard))
//    {
//        /* Discard the event for all other handlers */
//        ev.type = CIEvent::NoneType;
//    }
//}

void ImGuiSystem::setup_inputs(Proxy& e)
{
    auto& io = ImGui::GetIO();
    for(auto const& p : ImKeyMap)
    {
        io.KeyMap[p.first] = p.second;
    }

    auto ibus = e.service<comp_app::BasicEventBus<CIEvent>>();

    ibus->addEventFunction<CIScrollEvent>(
        0, [](CIEvent& e, CIScrollEvent* scroll) {
            ImGuiIO& io = ImGui::GetIO();

            io.MouseWheel = scroll->delta.x();
            e.type        = CIEvent::NoneType;
        });
    ibus->addEventFunction<CIMouseMoveEvent>(
        0, [](CIEvent& e, CIMouseMoveEvent* mouse) {
            ImGuiIO& io = ImGui::GetIO();
            io.AddMousePosEvent(
                mouse->origin.x() + mouse->delta.x(),
                mouse->origin.y() + mouse->delta.y());
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

            bool pressed
                = static_cast<bool>(mouse->mod & CIMouseButtonEvent::Pressed);
            //            bool double_press = static_cast<bool>(
            //                mouse->mod & CIMouseButtonEvent::DoubleClick);

            //            io.MouseDown[index] = pressed;

            //            if(pressed)
            //            {
            //                io.MouseClicked[index]     = pressed;
            //                io.MouseClickedPos[index]  = io.MousePos;
            //                io.MouseClickedTime[index] = ImGui::GetTime();
            //                cDebug("Clicked {0}", index);
            //            }

            //            if(double_press)
            //            {
            //                io.MouseDoubleClicked[index] = double_press;
            //                io.MouseDoubleClickTime      = ImGui::GetTime();
            //            }
            io.AddMousePosEvent(mouse->pos.x(), mouse->pos.y());
            io.AddMouseButtonEvent(index, pressed);
            if(io.WantCaptureMouseUnlessPopupClose)
                e.type = CIEvent::NoneType;
        });
}

} // namespace imgui::detail
