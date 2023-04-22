#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/libc_types.h>
#include <peripherals/stl/types.h>
#include <peripherals/stl/string_ops.h>

#include <coffee/graphics/apis/gleam/rhi_system.h>

#include <imgui.h>

namespace imgui {
namespace gfx {

using namespace gleam;

}
namespace detail {

using namespace Coffee::Components;
using type_safety::empty_list_t;
using type_safety::type_list_t;

struct ImGuiWidget
{
    using value_type = ImGuiWidget;
    using type       = alloc::VectorContainer<ImGuiWidget>;

    std::string_view name;
    std::function<void(EntityContainer&, time_point const&, duration const&)>
        func;
};

struct ImGuiData;
struct ImGuiDataDeleter
{
    void operator()(ImGuiData* p);
};

struct ImGuiGraphicsData;
struct ImGuiGraphicsDataDeleter
{
    void operator()(ImGuiGraphicsData* p);
};

using ImGuiManifest = SubsystemManifest<
    type_safety::type_list_t<ImGuiWidget>,
    type_safety::type_list_t<gfx::system>,
    type_safety::type_list_t<
        comp_app::DisplayInfo,
        comp_app::KeyboardInput,
        comp_app::MouseInput,
        comp_app::TouchInput,
        comp_app::Windowing,
        comp_app::BasicEventBus<Coffee::Input::CIEvent>>>;

struct ImGuiSystem : RestrictedSubsystem<ImGuiSystem, ImGuiManifest>,
                     comp_app::AppLoadableService
{
    ImGuiSystem(u32 priority = default_prio);

    using type = ImGuiSystem;

    virtual const ImGuiSystem& get() const final
    {
        return *this;
    }
    virtual ImGuiSystem& get() final
    {
        return *this;
    }

    virtual void start_restricted(Proxy& p, time_point const& t) final;
    virtual void end_restricted(Proxy& e, time_point const&) final;

    ImGuiSystem& addWidget(ImGuiWidget&& widget);

  protected:
    virtual void load(entity_container& e, comp_app::app_error& ec) final;
    virtual void unload(entity_container&, comp_app::app_error&) final;

  private:
    void setup_inputs(Proxy& e);
    void setup_graphics_data(Proxy& e);
    void submit_draws(Proxy& e);
    void free_graphics_data(Proxy& e);

    std::unique_ptr<ImGuiData, ImGuiDataDeleter>                 m_im_data;
    std::unique_ptr<ImGuiGraphicsData, ImGuiGraphicsDataDeleter> m_gfx_data;

    time_point m_previousTime;
    bool       m_textInputActive;

    std::string m_logFilename;
    std::string m_configFilename;
};

static_assert(is_restricted_subsystem<ImGuiSystem>);

} // namespace detail

namespace widgets {

extern detail::ImGuiWidget StatsMenu();

} // namespace widgets

using detail::ImGuiSystem;
using detail::ImGuiWidget;

} // namespace imgui
