#pragma once

#include <coffee/comp_app/subsystems.h>
#include <coffee/imgui/imgui_binding.h>

#include <fmt/format.h>

#include <deque>

using InputDebugManifest = compo::SubsystemManifest<
    compo::empty_list_t,
    type_safety::empty_list_t,
    compo::type_list_t<
        comp_app::ControllerInput,
        comp_app::KeyboardInput,
        comp_app::MouseInput,
        comp_app::BasicEventBus<Coffee::Input::CIEvent>>>;

struct InputDebugWidget
    : compo::RestrictedSubsystem<InputDebugWidget, InputDebugManifest>
{
    using type  = InputDebugWidget;
    using Proxy = compo::proxy_of<InputDebugManifest>;

    InputDebugWidget()
    {
        priority = 2048;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    static ImVec4 grey(libc_types::f32 color)
    {
        return ImVec4(color, color, color, 1);
    }

    static ImVec4 rgb(libc_types::f32 r, libc_types::f32 g, libc_types::f32 b)
    {
        return ImVec4(r, g, b, 1);
    }

    /* Code points, as the debug font has no glyphs past Latin */
    static std::string code_points(std::string_view utf8)
    {
        std::string out;
        for(size_t i = 0; i < utf8.size();)
        {
            auto const     lead  = static_cast<unsigned char>(utf8[i]);
            size_t const   count = lead < 0x80 ? 1 : lead < 0xE0 ? 2 : lead < 0xF0 ? 3 : 4;
            char32_t       point = count == 1 ? lead : lead & (0x7F >> count);
            for(size_t j = 1; j < count && i + j < utf8.size(); j++)
                point = (point << 6) | (static_cast<unsigned char>(utf8[i + j]) & 0x3F);
            out += fmt::format("U+{:04X} ", static_cast<libc_types::u32>(point));
            i += count;
        }
        return out;
    }

    /* What input methods sent: written text and compositions */
    void text_input(Proxy& e)
    {
        using namespace Coffee::Input;

        if(!m_listening)
            if(auto bus = e.service<comp_app::BasicEventBus<CIEvent>>())
            {
                m_listening = true;
                bus->addEventFunction<CIWriteEvent>(0, [this](CIEvent&, CIWriteEvent* write) {
                    m_composition.clear();
                    log(fmt::format("write {}", code_points(write->text)));
                });
                bus->addEventFunction<CIWEditEvent>(0, [this](CIEvent&, CIWEditEvent* edit) {
                    m_composition = edit->text;
                    log(fmt::format("edit  {}caret {}", code_points(edit->text), edit->cursor));
                });
            }

        if(ImGui::Begin("Text input"))
        {
            ImGui::InputText("##field", m_field.data(), m_field.size());
            ImGui::Text("Composing: %s", code_points(m_composition).c_str());
            if(ImGui::Button("Clear"))
            {
                m_log.clear();
                m_field.fill('\0');
            }
            for(auto const& line : m_log)
                ImGui::TextUnformatted(line.c_str());
        }
        ImGui::End();
    }

    void log(std::string line)
    {
        m_log.push_front(std::move(line));
        if(m_log.size() > 32)
            m_log.pop_back();
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        using libc_types::f32;
        using libc_types::i16;

        text_input(e);

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

  private:
    std::array<char, 256>   m_field{};
    std::string             m_composition;
    std::deque<std::string> m_log;
    bool                    m_listening{false};
};
