#pragma once

#include "caching.h"
#include <coffee/imgui/imgui_binding.h>

using BlamScriptManifest
    = Components::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename Version>
struct BlamScript
    : Components::RestrictedSubsystem<BlamScript<Version>, BlamScriptManifest>
{
    using type = BlamScript<Version>;

    using script_types = blam::hsc::types<typename Version::bytecode_type>;
    using script_state = typename blam::hsc::script_context<
        typename Version::bytecode_type>::script_state;

    using Proxy      = Components::proxy_of<BlamScriptManifest>;
    using time_point = Components::time_point;

    static constexpr u32 max_log_lines = 1024;

    template<typename T>
    static CString enum_to_string(T v)
    {
        auto val = magic_enum::enum_name(v);
        return CString(val.begin(), val.end());
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

    template<typename V>
    static CString layout_to_string(V const& v, blam::hsc::type_t type)
    {
        using blam::hsc::type_t;

        switch(type)
        {
        case type_t::bool_:
            return v.bytes[0] ? "true" : "false";
        case type_t::short_:
            return cast_pod(v.shorts[0]);
        case type_t::long_:
            return cast_pod(v.long_);
        case type_t::real_:
            return cast_pod(v.real);
        default:
            return enum_to_string(type) + "(" + cast_pod(v.to_ptr()) + ")";
        }
    }

    void log_line(CString const& line)
    {
        if(m_log.size() >= max_log_lines)
            m_log.erase(m_log.begin());

        m_log.push_back(line);
    }

    BlamScript(
        blam::map_container<Version> const& map,
        blam::scn::scenario<Version> const* scenario,
        blam::magic_data_t const&           magic) :
        m_map(&map),
        m_tags(map), m_magic(magic), m_scenario(scenario), m_running(true)
    {
        this->priority = 2048;

        auto string_segment = scenario->string_segment(magic);

        if(string_segment.has_error())
            Throw(undefined_behavior(string_segment.error()));
        m_strings = string_segment.value();

        auto globals = scenario->script.globals.data(magic);

        if(globals.has_error())
            Throw(undefined_behavior(globals.error()));

        m_env = {
            .strings = &m_strings,
            .scripts = scenario->function_table(magic),
            .globals = globals.value(),
        };

        m_script = script_types::bytecode_ptr::start_from(
            m_env, &scenario->bytecode(magic)[0]);

        typename script_types::opcode_handler hnd
            = [](typename script_types::bytecode_ptr&,
                 typename script_types::layout_t const&) {
                  return script_types::result_t::return_(
                      script_types::layout_t::void_());
              };

        m_script.init_globals(m_env.globals, m_strings, {hnd});
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        static typename script_types::opcode_handler hnd =
            [this](
                typename script_types::bytecode_ptr&   ptr,
                typename script_types::layout_t const& curr) {
                using op = typename script_types::opcode_t;
                using t  = blam::hsc::type_t;

                switch(curr.opcode)
                {
                case op::print_: {
                    auto output = CString(
                        m_strings.at(ptr.param(t::string_).to_ptr()).str());
                    log_line("print: " + output);
                    cDebug("Debug output: {0}", output);
                    break;
                }
                case op::inspect: {
                    auto inspect_target = ptr.param(t::any);
                    auto output         = layout_to_string(
                        inspect_target, inspect_target.ret_type);

                    std::string_view name;
                    if(inspect_target.to_ptr() != 0)
                        name = m_strings.at(inspect_target.to_ptr()).str();

                    log_line(Strings::fmt("inspect: {0} = {1}", name, output));
                    break;
                }
                case op::hud_set_help_text: {
                    typename script_types::layout_t help_text
                        = ptr.param(t::hud_msg);
                    auto hud_texts
                        = (*m_tags.find(m_scenario->ui_text.hud_text))
                              ->template data<blam::ui::hud_message>(m_magic)
                              .value();
                    auto sym  = m_strings.at(help_text.data_ptr).str();
                    auto text = hud_texts[0].symbol_find(m_magic, sym).value();

                    String text_(text.begin(), text.end());

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
            };

        using namespace blam::hsc;
        using namespace std::chrono_literals;

        if(ImGui::Begin("Console"))
        {
            if(ImGui::Button("Clear"))
                m_log.clear();

            ImGui::BeginChild("conlog");
            for(auto const& line : m_log)
                ImGui::TextDisabled("%s", line.c_str());
            ImGui::EndChild();
        }
        ImGui::End();

        if(ImGui::Begin("Bytecode"))
        {
            if(ImGui::Button("Start/Stop"))
                m_running = !m_running;
            ImGui::SameLine();
            if(ImGui::Button("Step") || m_running)
            {
//                m_script.execute_timestep(15ms, {hnd});
            }
        }
        ImGui::End();

        if(ImGui::Begin("Running scripts"))
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
        }
        ImGui::End();

        if(ImGui::Begin("Scripts"))
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
            ImGui::BeginChild("scripts_list");
            ImGui::Columns(5);
            for(auto const& script : m_script.context.scripts)
                display_script(script.second);

            ImGui::Columns();
            ImGui::EndChild();
        }
        ImGui::End();

        if(ImGui::Begin("Globals"))
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
        }
        ImGui::End();
    }
    void end_restricted(Proxy& e, time_point const&)
    {
    }

    blam::map_container<Version> const*                          m_map;
    blam::tag_index_view<Version>                                m_tags;
    blam::magic_data_t                                           m_magic;
    blam::scn::scenario<Version> const*                          m_scenario;
    blam::hsc::bytecode_pointer<typename Version::bytecode_type> m_script;
    blam::hsc::script_environment                                m_env;
    blam::string_segment_ref                                     m_strings;
    Vector<CString>                                              m_log;
    bool                                                         m_running;
};
