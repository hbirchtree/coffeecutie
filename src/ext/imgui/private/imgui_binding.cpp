// ImGui SDL2 binding with OpenGL3
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture
// identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See
// main.cpp for an example of using this. If you use this binding you'll need to
// call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(),
// ImGui::Render() and ImGui_ImplXXXX_Shutdown(). If you are new to ImGui, see
// examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <coffee/imgui/imgui_binding.h>

#include <coffee/core/CProfiling>
#include <coffee/core/base_state.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/types/chunk.h>
#include <coffee/core/types/display/event.h>
#include <peripherals/libc/memory_ops.h>
#include <peripherals/typing/vectors/vectors.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace imgui::detail;

using Coffee::DProfContext;
using libc_types::f32;
using libc_types::f64;
using libc_types::u16;
using libc_types::u32;
using semantic::mem_chunk;
using semantic::RSCA;
using stl_types::Range;
using stl_types::ShPtr;

using namespace typing::vector_types;
using namespace Coffee::Input;

using Coffee::cDebug;
using Coffee::cWarning;

enum class ImError
{
    /* Graphics errors */
    ShaderCompilation,
    ShaderAttach,

    /* State errors */
    AlreadyLoaded,
    AlreadyUnloaded,
    InvalidDisplaySize,

    GlobalStateFailure,
};

namespace imgui::detail {

struct ImGuiData
{
    f64 time;
    f32 scroll;

    u32 _pad;
};

// bool Init(EntityContainer& container)
//{
//     DProfContext _(IM_API "Initializing state");

//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO();

//    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
//    io.ConfigDockingWithShift    = true;
//    io.ConfigDockingAlwaysTabBar = true;

//    /* io is statically allocated, this is safe */
//    container.service<comp_app::BasicEventBus<CIEvent>>()->addEventData(
//        {100, ImGui_InputHandle});

//    for(auto const& p : ImKeyMap)
//    {
//        io.KeyMap[p.first] = p.second;
//    }

//    SetStyle();

//    return true;
//}

// void Shutdown()
//{
//     DProfContext _(IM_API "Shutting down");

//    InvalidateDeviceObjects();
//    ImGui::DestroyContext();
//}

static void SetStyle()
{
    DProfContext _(IM_API "Applying custom style");
    ImGuiStyle&  style      = ImGui::GetStyle();
    style.WindowRounding    = 3.f;
    style.FrameRounding     = 3.f;
    style.ScrollbarRounding = 0;
    style.FramePadding      = {3.f, 3.f};

    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(.3f, .3f, .9f, 1.f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(.3f, .3f, .9f, 1.f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(.2f, .2f, .9f, 1.f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(.2f, .2f, .9f, 1.f);

    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(.2f, .2f, .9f, 1.f);
    style.Colors[ImGuiCol_SliderGrabActive]    = ImVec4(.2f, .2f, .9f, 1.f);
    style.Colors[ImGuiCol_CheckMark]           = ImVec4(1.f, 1.f, 1.f, 1.f);

    style.Colors[ImGuiCol_Button]        = ImVec4(.2f, .2f, .9f, 1.f);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(.3f, .3f, .9f, 1.f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(.5f, .5f, .9f, 1.f);

    style.Colors[ImGuiCol_FrameBg]        = ImVec4(.2f, .2f, .5f, 1.f);
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(.3f, .3f, .5f, 1.f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(.4f, .4f, .5f, 1.f);

    return;

    style.Colors[ImGuiCol_Text]         = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    style.Colors[ImGuiCol_WindowBg]     = ImVec4(0.00f, 0.00f, 0.00f, 0.98f);
    //   style.Colors[ImGuiCol_ChildWindowBg]  = ImVec4(1.00f, 1.00f, 1.00f,
    //   0.01f);
    style.Colors[ImGuiCol_PopupBg]        = ImVec4(0.00f, 0.00f, 0.00f, 0.99f);
    style.Colors[ImGuiCol_Border]         = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    style.Colors[ImGuiCol_BorderShadow]   = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]        = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
    style.Colors[ImGuiCol_TitleBg]        = ImVec4(0.00f, 0.00f, 0.00f, 0.99f);
    style.Colors[ImGuiCol_TitleBgCollapsed]
        = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.99f);
    style.Colors[ImGuiCol_MenuBarBg]     = ImVec4(0.00f, 0.00f, 0.00f, 0.98f);
    style.Colors[ImGuiCol_ScrollbarBg]   = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]
        = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]
        = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
    //    style.Colors[ImGuiCol_ComboBg]    =
    //    ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]
        = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Button]        = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_Header]        = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    style.Colors[ImGuiCol_HeaderActive]  = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);
    //    style.Colors[ImGuiCol_Column]        = ImVec4(1.00f, 1.00f, 1.00f,
    //    0.00f); style.Colors[ImGuiCol_ColumnHovered] =
    //    ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    //    style.Colors[ImGuiCol_ColumnActive]  = ImVec4(1.00f, 1.00f, 1.00f,
    //    0.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);
    style.Colors[ImGuiCol_ResizeGripHovered]
        = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_ResizeGripActive]
        = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    //    style.Colors[ImGuiCol_CloseButton] = ImVec4(1.00f, 1.00f, 1.00f,
    //    0.20f); style.Colors[ImGuiCol_CloseButtonHovered] =
    //        ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    //    style.Colors[ImGuiCol_CloseButtonActive] =
    //        ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]
        = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_PlotHistogramHovered]
        = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ModalWindowDimBg]
        = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);

    for(auto i : Range<>(ImGuiCol_COUNT))
    {
        auto alpha = style.Colors[i].w;

        auto col = ImVec4(0.f, 0.f, 0.f, alpha);

        if(style.Colors[i].x > 0.5f)
            col = ImVec4(0.3f, 0.3f, 1.f, alpha);

        style.Colors[i] = col;
    }

    style.Colors[ImGuiCol_ResizeGrip]        = ImVec4(0.2f, 0.2f, 1.f, 1.f);
    style.Colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.4f, 0.4f, 1.f, 1.f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.3f, 0.3f, 1.f, 1.f);

    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(.9f, .9f, .9f, 1.f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(.9f, .9f, .9f, 1.f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.f, 1.f, 1.f, 1.f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.f, 1.f, 1.f, 1.f);

    //    style.Colors[ImGuiCol_ComboBg] = ImVec4(.0f, .0f, .0f, 1.00f);

    style.Colors[ImGuiCol_Border] = ImVec4(.9f, .9f, .9f, 1.f);
}

void ImGuiDataDeleter::operator()(ImGuiData* p)
{
    delete p;
}

ImGuiSystem::ImGuiSystem(u32 priority)
{
    this->priority = priority;
}

void ImGuiSystem::load(entity_container& e, comp_app::app_error&)
{
    using namespace platform::url::constructors;

    DProfContext _(IM_API "Initializing state");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags = 0;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigMacOSXBehaviors     = compile_info::platform::is_macos;
    io.ConfigDockingWithShift    = true;
    io.ConfigDockingAlwaysTabBar = true;

    m_logFilename = *"imgui.log"_tmp;
    m_configFilename = *"imgui.ini"_config;

    io.LogFilename = m_logFilename.c_str();
    io.IniFilename = m_configFilename.c_str();

    /* io is statically allocated, this is safe */
    //    e.service<comp_app::BasicEventBus<CIEvent>>()->addEventData(
    //        {100, ImGui_InputHandle});

    SetStyle();

    m_textInputActive = false;

    e.register_component_inplace<ImGuiWidget>();

    m_im_data = std::unique_ptr<ImGuiData, ImGuiDataDeleter>(new ImGuiData{});
    m_im_data->time = 0.;

    auto proxy = Proxy(e);
    setup_inputs(proxy);
    setup_graphics_data(proxy);
}

void ImGuiSystem::unload(entity_container& e, comp_app::app_error&)
{
    DProfContext _(IM_API "Shutting down");
    auto         proxy = Proxy(e);
    free_graphics_data(proxy);
}

void ImGuiSystem::start_restricted(Proxy& p, time_point const& t)
{
    DProfContext _(IM_API "Preparing frame data");

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    auto s = p.service<comp_app::Windowing>()->size();

    f32 uiScaling = 1.f;

    if(auto display = p.service<comp_app::DisplayInfo>())
        uiScaling = display->dpi(display->currentDisplay());

    io.DisplaySize             = ImVec2(s.w / uiScaling, s.h / uiScaling);
    io.DisplayFramebufferScale = ImVec2(uiScaling, uiScaling);

    // Setup time step
    f64 time        = ImGui::GetTime();
    io.DeltaTime    = time > 0.0 ? time - m_im_data->time : 1.0 / 60.0;
    m_im_data->time = time;

    // Setup inputs
    if(auto mouse = p.service<comp_app::MouseInput>())
    {
        auto pos        = mouse->position();
        io.MousePosPrev = io.MousePos;
        io.MousePos     = ImVec2(pos.x / uiScaling, pos.y / uiScaling);
        //    if(!io.MouseClicked[0])
        //        io.MouseClicked[0] = mouse->buttons() &
        //        CIMouseButtonEvent::LeftButton;
        //    if(!io.MouseClicked[1])
        //        io.MouseClicked[1]
        //            = mouse->buttons() & CIMouseButtonEvent::MiddleButton;
        //    if(!io.MouseClicked[2])
        //        io.MouseClicked[2] = mouse->buttons() &
        //        CIMouseButtonEvent::RightButton;
        //    for(auto i : Range<>{5})
        //        if(io.MouseClicked[i])
        //        {
        //            io.MouseDown[i]        = true;
        //            io.MouseClickedPos[i]  = io.MousePos;
        //            io.MouseClickedTime[i] = time;
        //        } else
        //        {
        //            io.MouseDown[i] = false;
        //        }
        io.MouseWheel     = m_im_data->scroll;
        m_im_data->scroll = 0.0f;
    }

    // Start the frame
    DProfContext __(IM_API "Running ImGui::NewFrame()");
    ImGui::NewFrame();

    if(auto keyboard = p.service<comp_app::KeyboardInput>())
    {
        if(io.WantTextInput && !m_textInputActive)
        {
            m_textInputActive = true;
            keyboard->openVirtual();
        } else if(!io.WantTextInput && m_textInputActive)
        {
            m_textInputActive = false;
            keyboard->closeVirtual();
        }
    }

    auto delta = duration_cast<duration>(t - m_previousTime);

    {
        for(auto& widget : p.select<ImGuiWidget>())
            p.get<ImGuiWidget>(widget.id)->func(get_container(p), t, delta);
    }

    m_previousTime = t;
}

void ImGuiSystem::end_restricted(Proxy& e, time_point const&)
{
    DProfContext _(IM_API "Rendering UI");

    if(auto window = e.service<comp_app::Windowing>();
       window->size().area() == 0)
        return;

    ImGui::Render();
    submit_draws(e);
}

} // namespace imgui::detail
