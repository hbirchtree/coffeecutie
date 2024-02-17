#pragma once

#include <coffee/comp_app/subsystems.h>
#include <coffee/imgui/imgui_binding.h>

using InputDebugManifest = compo::SubsystemManifest<
    compo::empty_list_t,
    type_safety::empty_list_t,
    compo::type_list_t<
        comp_app::ControllerInput,
        comp_app::KeyboardInput,
        comp_app::MouseInput>>;

struct InputDebugWidget
    : compo::RestrictedSubsystem<InputDebugWidget, InputDebugManifest>
{
    using type  = InputDebugWidget;
    using Proxy = compo::proxy_of<InputDebugManifest>;

    InputDebugWidget()
    {
        priority = 2048;
    }

    static ImVec4 grey(libc_types::f32 color)
    {
        return ImVec4(color, color, color, 1);
    }

    static ImVec4 rgb(libc_types::f32 r, libc_types::f32 g, libc_types::f32 b)
    {
        return ImVec4(r, g, b, 1);
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        using libc_types::f32;
        using libc_types::i16;

        auto* controllers = e.service<comp_app::ControllerInput>();

        while(ImGui::Begin("Controller"))
        {
            if(!controllers || controllers->count() < 1)
                break;

            auto        controller = controllers->state(0);
            auto const& btns       = controller.buttons.e;

            constexpr f32      axis_scale = std::numeric_limits<i16>::max();
            std::array<f32, 6> axes       = {{
                controller.axes.e.l_x / axis_scale,
                controller.axes.e.l_y / axis_scale,
                controller.axes.e.r_x / axis_scale,
                controller.axes.e.r_y / axis_scale,
                controller.axes.e.t_l / axis_scale,
                controller.axes.e.t_r / axis_scale,
            }};

            ImGui::Columns();
            ImGui::SliderFloat("LX", &axes[0], -1, 1);
            ImGui::SliderFloat("LY", &axes[1], -1, 1);
            ImGui::SliderFloat("RX", &axes[2], -1, 1);
            ImGui::SliderFloat("RY", &axes[3], -1, 1);
            ImGui::SliderFloat("TL", &axes[4], -1, 1);
            ImGui::SliderFloat("TR", &axes[5], -1, 1);

            ImGui::Columns(3);
            ImGui::NextColumn();
            ImGui::ColorButton("pad_up", grey(btns.p_up ? .8 : .2));
            ImGui::NextColumn();
            ImGui::NextColumn();
            ImGui::ColorButton("pad_left", grey(btns.p_left ? .8 : .2));
            ImGui::NextColumn();
            ImGui::NextColumn();
            ImGui::ColorButton("pad_right", grey(btns.p_right ? .8 : .2));
            ImGui::NextColumn();
            ImGui::NextColumn();
            ImGui::ColorButton("pad_down", grey(btns.p_down ? .8 : .2));

            /* Face buttons */
            ImGui::Columns(4);

            ImGui::ColorButton("x", rgb(0, 0, btns.x ? .8 : .2));
            ImGui::NextColumn();
            ImGui::ColorButton("a", rgb(0, btns.a ? .8 : .2, 0));
            ImGui::NextColumn();
            ImGui::ColorButton("b", rgb(btns.b ? .8 : .2, 0, 0));
            ImGui::NextColumn();
            ImGui::ColorButton("y", rgb(btns.y ? .8 : .2, btns.y ? .8 : .2, 0));
            ImGui::NextColumn();

            /* Bumper buttons */
            ImGui::Columns(2);
            ImGui::ColorButton("bl", grey(btns.b_l ? .8 : .2));
            ImGui::NextColumn();
            ImGui::ColorButton("br", grey(btns.b_r ? .8 : .2));
            ImGui::NextColumn();

            /* Stick buttons */
            ImGui::ColorButton("sl", grey(btns.s_l ? .8 : .2));
            ImGui::NextColumn();
            ImGui::ColorButton("sr", grey(btns.s_r ? .8 : .2));
            ImGui::NextColumn();

            /* Menu buttons */
            ImGui::ColorButton("back", grey(btns.back ? .8 : .2));
            ImGui::NextColumn();
            ImGui::ColorButton("start", grey(btns.start ? .8 : .2));
            ImGui::NextColumn();

            break;
        }
        ImGui::End();
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }
};
