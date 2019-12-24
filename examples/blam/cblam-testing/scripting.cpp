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

    for(auto const& op : bytecode_start)
    {
        cDebug(
            "{0}/{1} {2} {3} {4}",
            op.opcode,
            op.param_type,
            op.exp_type,
            op.ret_type,
            op.next_op.ip);

        if(op.opcode == blam::hsc::opcode_t::sentinel)
            break;
    }

    for(auto const& function : scenario[0].function_table(map.magic))
    {
        cDebug(
            "{2} {0} {1}", function.name.str(), function.type, function.index);
        auto bytecode_pointer =
            blam::hsc::bytecode_pointer::start_from(bytecode_start.data);
        bytecode_pointer.jump(function.index);
        while(!bytecode_pointer.finished())
        {
            cDebug(
                "> opcode {0} {1}",
                bytecode_pointer.current_ip,
                bytecode_pointer.current->opcode);
            cDebug(">> type {0}", bytecode_pointer.current->param_type);
            bytecode_pointer.advance();
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
