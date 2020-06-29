#include <blam/cblam.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace std::literals;

i32 coffee_main(i32, cstring_w*)
{
    auto                 map_file = "a10.map"_rsc;
    blam::map_container  map(map_file, blam::pc_version);
    blam::tag_index_view tags(map);

    using script_types = blam::hsc::types<blam::hsc::pc_opcode_t>;

    auto scenario =
        map.tags->scenario(map.map)
            .to_reflexive<blam::scn::scenario<script_types::opcode_t>>()
            .data(map.magic);

    auto const& bytecode_start = scenario[0].bytecode(map.magic);
    auto        string_seg     = scenario[0].string_segment(map.magic);

    auto val_to_string =
        [&string_seg](script_types::layout_t const& v) -> CString {
        using t = blam::hsc::type_t;
        using blam::hsc::expression_t;

        if(v.exp_type == expression_t::global_ref ||
           v.exp_type == expression_t::script_ref)
            return string_seg.at(v.data_ptr).str();

        switch(v.ret_type)
        {
        case t::bool_:
            return v.to_bool() ? "true" : "false";
        case t::short_:
            return std::to_string(v.to_u16());
        case t::long_:
            return std::to_string(v.to_u32());
        case t::real_:
            return std::to_string(v.to_real());
        default:
            if(v.data_ptr != 0)
                return string_seg.at(v.data_ptr).str();
            else
                return Strings::to_string(v);
        }
    };

    script_types::opcode_handler pre_op =
        [&val_to_string](
            script_types::bytecode_ptr&   ptr,
            script_types::layout_t const& curr) {
            using blam::hsc::expression_t;

            switch(curr.exp_type)
            {
            case expression_t::group:
                cBasicPrintNoNL(" ( {0}", curr.opcode);
                break;
            default:
                cBasicPrintNoNL(" {0}", val_to_string(curr));
                break;
            }

            return script_types::result_t::return_({});
        };
    script_types::opcode_handler post_op =
        [](script_types::bytecode_ptr&   ptr,
           script_types::layout_t const& curr) {
            cBasicPrintNoNL(" )");

            return script_types::result_t::return_({});
        };

    script_types::opcode_handler opcode_executor =
        [&](script_types::bytecode_ptr&   ptr,
            script_types::layout_t const& curr) {
            using opc = script_types::opcode_t;
            using t   = blam::hsc::type_t;
            using namespace blam::hsc;

            script_types::layout_t out = script_types::layout_t::void_();

            out.param_type = out.ret_type =
                script_types::signature(curr).return_type;

            switch(curr.opcode)
            {
            case opc::print_:
            {
                cBasicPrint("");
                cDebug(" >> {0}", val_to_string(ptr.param(t::string_)));
                break;
            }
            case opc::game_is_cooperative:
                out.set(0);
                break;
            case opc::unit:
            {
                out            = ptr.param(t::object);
                out.param_type = out.ret_type = t::unit;
                break;
            }
            case opc::list_count:
            {
                out.set(0);
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
            case opc::inspect:
            {
                auto object = ptr.param(t::any);
                cDebug("INSPECT: {0}", object);
                break;
            }
            case opc::game_reverted:
            case opc::game_saving:
                out.set(false);
                break;
            case opc::volume_test_objects_all:
            case opc::volume_test_objects:
            case opc::volume_test_object:
                out.set(blam::hsc::bypass_conditions);
                break;
            case opc::device_get_position:
                out.set(0.f);
                break;
            case opc::game_difficulty_get:
                out = script_types::layout_t::typed_(t::game_difficulty);
                out.set(1);
                break;
            default:
                break;
            }

            return script_types::result_t::return_(out);
        };

    blam::hsc::script_environment env = {&string_seg,
                                         scenario[0].function_table(map.magic),
                                         scenario[0].globals.data(map.magic)};

    C_UNUSED(auto func_table) = scenario[0].function_table(map.magic);

    auto script = script_types::bytecode_ptr::start_from(
        env, scenario[0].bytecode(map.magic).data);

    script.init_globals(
        scenario[0].globals.data(map.magic), string_seg, {opcode_executor});

    for(auto const& function : script.context.scripts)
    {
        cDebug(
            "Script: {0} : {1}-{2}",
            function.second.name(),
            str::print::pointerify(function.second.script_start),
            str::print::pointerify(function.second.script_end));
    }

    while(true)
    {
        cBasicPrint("");
        cDebug(" ---- Tick ----");
        for(auto& s : script.context.scripts)
        {
            if(s.second.is_inactive())
            {
                continue;
            }

            cDebug(" > Ready {0}", s.second.name());
            cDebug(" >> Start from {0}", str::print::pointerify(s.second.ip));

            script.execute_state(s.second, {opcode_executor, pre_op, post_op});

            cBasicPrint("");
            cDebug(
                " >> End state {0} at {1} ( ",
                s.second.status,
                str::print::pointerify(s.second.ip));
            for(auto ip : s.second.link_register)
                cBasicPrint("{0} ", str::print::pointerify(ip));
            cBasicPrint(")");
        }
        script.update_sleepers(15ms, {opcode_executor, pre_op, post_op});
        CurrentThread::sleep_for(15ms);
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
