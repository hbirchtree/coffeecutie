#include <blam/cblam.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;

i32 coffee_main(i32, cstring_w*)
{
    auto                map_file = "c10.map"_rsc;
    blam::map_container map(map_file, blam::pc_version);

    using script_types = blam::hsc::types<blam::hsc::pc_opcode_t>;

    auto scenario =
        map.tags->scenario(map.map)
            .to_reflexive<blam::scn::scenario<script_types::opcode_t>>()
            .data(map.magic);

    auto const& bytecode_start = scenario[0].bytecode(map.magic);
    auto        string_seg     = scenario[0].string_segment(map.magic);

    script_types::opcode_handler opcode_executor =
        [&](script_types::bytecode_ptr&   ptr,
            script_types::layout_t const& curr) {
            using opc = script_types::opcode_t;
            using t   = blam::hsc::type_t;

            script_types::layout_t out = script_types::layout_t::void_();

            out.ret_type = script_types::signature(curr).return_type;

            cDebug("    > {0}", curr.opcode);

            switch(curr.opcode)
            {
            case opc::print_:
            {
                auto out_string =
                    string_seg.at(ptr.param(t::string_).data_ptr).str();

                cDebug(" >> {0}", out_string);
                break;
            }
            case opc::if_:
            {
                break;
            }
            case opc::begin:
            {
                auto func_name =
                    string_seg.at((ptr.current + 1)->data_ptr).str();
                cDebug("Function call to {0}", func_name);
                break;
            }
            case opc::unit:
            {
                out            = ptr.param(t::object);
                out.param_type = t::unit;
                out.ret_type   = t::unit;
                break;
            }
            case opc::list_count:
            {
                out               = script_types::layout_t::typed_(t::short_);
                out.data_short[0] = 69;
                break;
            }
            case opc::list_get:
            {
                out = script_types::layout_t::typed_(t::object);
                break;
            }
            case opc::players:
            {
                out.ret_type = t::obj_list;
                break;
            }
            case opc::not_:
            {
                out               = script_types::layout_t::typed_(t::bool_);
                out.data_bytes[0] = !ptr.param(t::bool_).data_bytes[0];
                break;
            }
            case opc::game_reverted:
            case opc::game_saving:
                out               = script_types::layout_t::typed_(t::bool_);
                out.data_bytes[0] = 1;
                break;
            default:
                break;
            }

            return script_types::result_t::return_(out);
        };

    blam::hsc::script_environment env = {&string_seg,
                                         scenario[0].function_table(map.magic),
                                         scenario[0].globals.data(map.magic)};

    auto func_table = scenario[0].function_table(map.magic);

    auto script = script_types::bytecode_ptr::start_from(
        env, scenario[0].bytecode(map.magic).data);

    for(auto& s : script.context.scripts)
        if(s.second.function->schedule == blam::hsc::script_type_t::startup)
        {
            cDebug(" > Startup {0}", s.second.name());
            script.execute_state(s.second, opcode_executor);
            cDebug(" >> End state {0}", s.second.status);
        }

    while(true)
    {
        cDebug(" ---- Tick ----");
        for(auto& s : script.context.scripts)
        {
            if(s.second.is_inactive())
            {
                cDebug(" > Inactive {0}", s.second.name());
                continue;
            }

            cDebug(" > Ready {0}", s.second.name());
            cDebug(" >> Start from {0}", str::print::pointerify(s.second.ip));

            script.execute_state(s.second, opcode_executor);

            cDebug(" >> End state {0}", s.second.status);
        }
        CurrentThread::sleep_for(Chrono::milliseconds(500));
    }

    return 0;

    for(auto const& function : scenario[0].function_table(map.magic))
    {
        cDebug(
            "{2} script {3} {0} {1}",
            function.name.str(),
            function.type,
            function.index,
            function.schedule);

        if(function.schedule != blam::hsc::script_type_t::startup)
            continue;

        auto bytecode_pointer =
            script_types::bytecode_ptr::start_from(env, bytecode_start.data);
        bytecode_pointer.call(function.index);

        while(!bytecode_pointer.finished())
        {
            cDebug(
                "> opcode {0} {1} params={2}, exp={3}",
                bytecode_pointer.current_ip,
                bytecode_pointer.current->opcode,
                bytecode_pointer.num_params(),
                bytecode_pointer.current->exp_type);

            auto out = bytecode_pointer.evaluate(
                *bytecode_pointer.current, opcode_executor);

            bytecode_pointer.advance();
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
