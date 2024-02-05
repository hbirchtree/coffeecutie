#include "script_component.h"

#include <blam/volta/blam_stl.h>
#include <blam/volta/hsc/blam_bytecode.h>
#include <blam/volta/hsc/bytecode_eval.h>

#include <coffee/core/debug/formatting.h>
#include <coffee/imgui/imgui_binding.h>
#include <peripherals/stl/type_list.h>

#include "selected_version.h"
#include "data.h"

using libc_types::f32;
using libc_types::u32;
using type_safety::empty_list_t;
using type_safety::type_list_t;

using Coffee::cDebug;

using namespace std::chrono_literals;

using BlamScriptManifest = compo::
    SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename Ver>
struct BlamScript
    : compo::RestrictedSubsystem<BlamScript<Ver>, BlamScriptManifest>
{
    using type = BlamScript<Ver>;

    using script_types = blam::hsc::types<typename Ver::bytecode_type>;
    using script_state = typename blam::hsc::script_context<
        typename Ver::bytecode_type>::script_state;

    using Proxy      = compo::proxy_of<BlamScriptManifest>;
    using time_point = compo::time_point;

    static constexpr u32 max_log_lines = 1024;

    template<typename T>
    static std::string enum_to_string(T v)
    {
        auto val = magic_enum::enum_name(v);
        return std::string(val.begin(), val.end());
    }

    static ImVec4 status_to_color(blam::hsc::script_status s)
    {
        using status = blam::hsc::script_status;

        switch(s)
        {
        case status::ready:
            return {0, .5f, 0, 1};
        case status::dormant:
            return {0, 0, 1, 1};
        case status::running:
            return {0, 1, 0, 1};
        case status::sleeping:
            return {1, 0, 0, 1};
        case status::finished:
            return {1, 0, 1, 1};
        }
        __builtin_unreachable();
    }

    static void display_script(script_state const& state)
    {
        auto color = status_to_color(state.status);

        blam::hsc::function_declaration const& def = *state.function;
        ImGui::TextColored(color, "%s", def.name.str().data());
        ImGui::NextColumn();
        ImGui::TextColored(color, "%s", enum_to_string(def.schedule).c_str());
        ImGui::NextColumn();
        ImGui::TextColored(color, "%s", enum_to_string(state.status).c_str());
        ImGui::NextColumn();
        ImGui::TextColored(
            color, "%04x - %04x", state.script_start, state.script_end);
        ImGui::NextColumn();
        ImGui::TextColored(color, "%04x", state.ip);
        ImGui::NextColumn();
    }

    static void display_script(blam::hsc::function_declaration const& decl)
    {
        ImGui::Text("%s", decl.name.str().data());
        ImGui::NextColumn();
        ImGui::Text("%s", enum_to_string(decl.schedule).c_str());
        ImGui::NextColumn();
        ImGui::Text("%04x", decl.index);
        ImGui::NextColumn();
    }

    template<typename V>
    static std::string layout_to_string(V const& v, blam::hsc::type_t type)
    {
        using blam::hsc::type_t;

        switch(type)
        {
        case type_t::bool_:
            return v.bytes[0] ? "true" : "false";
        case type_t::short_:
            return std::to_string(v.shorts[0]);
        case type_t::long_:
            return std::to_string(v.long_);
        case type_t::real_:
            return std::to_string(v.real);
        default:
            return fmt::format(
                "{}({})", enum_to_string(type), std::to_string(v.to_ptr()));
        }
    }

    void log_line(std::string const& line)
    {
        if(m_log.size() >= max_log_lines)
            m_log.erase(m_log.begin());

        m_log.push_back(line);
    }

    BlamScript()
    {
        this->priority = 2048;
    }

    void load_from(
        blam::map_container<halo_version> const& map,
        blam::scn::scenario<halo_version> const* scenario)
    {
        m_map      = &map;
        m_magic    = map.magic;
        m_tags     = blam::tag_index_view(map);
        m_scenario = scenario;

        m_current_disasm = nullptr;

        auto string_segment = scenario->string_segment(map.magic);

        if(string_segment.has_error())
            Throw(undefined_behavior(string_segment.error()));
        m_strings = string_segment.value();

        auto globals = scenario->script.globals.data(map.magic);

        if(globals.has_error())
            Throw(undefined_behavior(globals.error()));

        m_env = {
            .strings = &m_strings,
            .scripts = scenario->function_table(map.magic),
            .globals = globals.value(),
        };

        m_script = script_types::bytecode_ptr::start_from(
            m_env, &scenario->bytecode(map.magic)[0]);

        typename script_types::opcode_handler hnd
            = [](typename script_types::bytecode_ptr&,
                 typename script_types::layout_t const&) {
                  return script_types::result_t::return_(
                      script_types::layout_t::void_());
              };

        m_script.init_globals(m_env.globals, m_strings, {hnd});
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        using namespace blam::hsc;
        using namespace std::chrono_literals;

        if(!m_scenario)
            return;

        if(ImGui::Begin("Scripts"))
        {
            if(ImGui::BeginTabBar("scripting"))
            {
                render_tabs(p);
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void render_tabs(Proxy& /*p*/)
    {
        if(ImGui::BeginTabItem("Console"))
        {
            if(ImGui::Button("Clear"))
                m_log.clear();

            ImGui::BeginChild("conlog");
            for(auto const& line : m_log)
                ImGui::TextDisabled("%s", line.c_str());
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Bytecode"))
        {
            if(ImGui::Button("Start/Stop"))
                m_running = !m_running;
            ImGui::SameLine();
            if(ImGui::Button("Step") || m_running)
            {
                m_script.execute_timestep(
                    15ms,
                    {
                        .cmd =
                            [this](auto& ptr, const auto& curr) {
                                return script_func_handler(ptr, curr);
                            },
                    });
            }
            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Running scripts"))
        {
            ImGui::Columns(5);

            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::Text("Schedule");
            ImGui::NextColumn();
            ImGui::Text("State");
            ImGui::NextColumn();
            ImGui::Text("Range");
            ImGui::NextColumn();
            ImGui::Text("IP");
            ImGui::NextColumn();

            ImGui::Columns();
            ImGui::BeginChild("scripts_running");
            ImGui::Columns(5);
            for(auto const& script : m_script.context.scripts)
            {
                if(script.second.status != blam::hsc::script_status::running)
                    continue;

                display_script(script.second);
            }
            ImGui::Columns();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Scripts"))
        {
            ImGui::Columns(3);

            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::Text("Schedule");
            ImGui::NextColumn();
            ImGui::Text("Entrypoint");
            ImGui::NextColumn();

            ImGui::Columns();
            ImGui::BeginChild("scripts_list");
            ImGui::Columns(3);
            for(auto const& script : m_env.scripts)
                display_script(script);

            ImGui::Columns();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Globals"))
        {
            ImGui::Columns(3);

            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::Text("Type");
            ImGui::NextColumn();
            ImGui::Text("Value");
            ImGui::NextColumn();

            ImGui::Columns();
            ImGui::BeginChild("globals");
            ImGui::Columns(3);
            for(blam::hsc::global const& global : m_env.globals)
            {
                auto store = *m_script.context.global_by_ptr(
                    m_strings.get_index(global.name));

                ImGui::Text("%s", global.name.str().data());
                ImGui::NextColumn();
                ImGui::Text("%s", enum_to_string(global.type).c_str());
                ImGui::NextColumn();
                ImGui::Text(
                    "%s", layout_to_string(*store, global.type).c_str());
                ImGui::NextColumn();
            }
            ImGui::Columns();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        if(m_current_disasm && ImGui::BeginTabItem("Script view"))
        {
            ImGui::Text(
                "Disassembly of %s", m_current_disasm->name.str().data());
            ImGui::Columns(3);
            ImGui::Text("S");
            ImGui::NextColumn();
            ImGui::Text("OpCode");
            ImGui::NextColumn();
            ImGui::Text("Operands");
            ImGui::NextColumn();
            ImGui::Columns();

            ImGui::BeginChild("disasm");
            ImGui::Columns(3);
            // TODO: List entire script + mark current IP
            ImGui::Columns();
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
    }

    auto script_func_handler(
        typename script_types::bytecode_ptr&   ptr,
        typename script_types::layout_t const& curr)
    {
        using op = typename script_types::opcode_t;
        using t  = blam::hsc::type_t;

        switch(curr.opcode)
        {
        case op::print_: {
            auto output = std::string(
                m_strings.at(ptr.param(t::string_).to_ptr()).str());
            log_line("print: " + output);
            cDebug("Debug output: {0}", output);
            break;
        }
        case op::inspect: {
            auto inspect_target = ptr.param(t::any);
            auto output
                = layout_to_string(inspect_target, inspect_target.ret_type);

            std::string_view name;
            if(inspect_target.to_ptr() != 0)
                name = m_strings.at(inspect_target.to_ptr()).str();

            log_line(fmt::format("inspect: {0} = {1}", name, output));
            break;
        }
        case op::hud_set_help_text: {
            typename script_types::layout_t help_text = ptr.param(t::hud_msg);
            auto hud_texts = (*m_tags.find(m_scenario->ui_text.hud_text))
                                 .template data<blam::ui::hud_message>(m_magic)
                                 .value();
            auto sym  = m_strings.at(help_text.data_ptr).str();
            auto text = hud_texts[0].symbol_find(m_magic, sym).value();

            std::string text_(text.begin(), text.end());

            cDebug(" - sym: {0} -> {1}", sym, text_);
            log_line("HUD text: " + text_);

            break;
        }
        default:
            break;
        }

        if(curr.ret_type == blam::hsc::type_t::bool_)
        {
            auto out = script_types::layout_t::typed_(curr.ret_type);
            out.set(true);
            return script_types::result_t::return_(out);
        }

        return script_types::result_t::return_(
            script_types::layout_t::typed_(curr.ret_type));
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }

    /* Read-only map data */
    blam::map_container<Ver> const* m_map{nullptr};
    blam::tag_index_view<Ver>       m_tags{};
    blam::magic_data_t              m_magic{};
    blam::scn::scenario<Ver> const* m_scenario{nullptr};
    blam::string_segment_ref        m_strings{};

    /* Runtime data */
    blam::hsc::bytecode_pointer<typename Ver::bytecode_type> m_script;
    blam::hsc::script_environment                            m_env;

    /* View info */
    blam::hsc::function_declaration const* m_current_disasm{nullptr};

    std::vector<std::string> m_log;
    bool                     m_running{false};
};

void alloc_scripting(compo::EntityContainer& e)
{
    auto& script = e.register_subsystem_inplace<BlamScript<halo_version>>();

    auto& gbus = e.subsystem_cast<GameEventBus>();
    gbus.addEventFunction<MapLoadEvent>(
        0, [&script](auto&, auto*) { script.m_scenario = nullptr; });
    gbus.addEventFunction<MapChangedEvent<halo_version>>(
        0, [&script](auto&, MapChangedEvent<halo_version>* changed) {
            script.load_from(changed->container, changed->scenario);
        });
}
