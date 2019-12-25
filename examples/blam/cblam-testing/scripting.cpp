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

    auto scenario =
        map.tags->scenario(map.map).to_reflexive<blam::scn::scenario>().data(
            map.magic);

    auto const& bytecode_start = scenario[0].bytecode(map.magic);

    if(false)
        for(auto const& op : bytecode_start)
        {
            cDebug(
                "{0}: {1}/{2} {3} {4} {5}",
                &op - bytecode_start.data,
                op.opcode,
                op.param_type,
                op.exp_type,
                op.ret_type,
                op.next_op.ip,
                op.next_op.salt);

            if(op.opcode == blam::hsc::opcode_t::sentinel)
                break;
        }

    auto string_seg = scenario[0].string_segment(map.magic);

    for(auto const& function : scenario[0].function_table(map.magic))
    {
        cDebug(
            "{2} {0} {1}", function.name.str(), function.type, function.index);
        auto bytecode_pointer =
            blam::hsc::bytecode_pointer::start_from(bytecode_start.data);
        bytecode_pointer.call(function.index);

        while(!bytecode_pointer.finished())
        {
            cDebug(
                "> opcode {0} {1} params={2}, exp={3}",
                bytecode_pointer.current_ip,
                bytecode_pointer.current->opcode,
                bytecode_pointer.num_params(),
                bytecode_pointer.current->exp_type);

            using opc = blam::hsc::opcode_t;
            using t   = blam::hsc::type_t;
            auto curr = bytecode_pointer.current;

            switch(curr->opcode)
            {
            case opc::print_:
            {
                auto out_string =
                    string_seg.at(bytecode_pointer.param(t::string_).data_ptr)
                        .str();

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
                    string_seg.at(bytecode_pointer.param(t::string_).data_ptr)
                        .str();
                break;
            }
            default:
                break;
            }

            bytecode_pointer.advance();
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
