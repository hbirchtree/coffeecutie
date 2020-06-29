#pragma once

#include "cblam_bytecode.h"

namespace blam {
namespace hsc {

template<typename BC>
inline typename bytecode_pointer<BC>::result_t bytecode_pointer<BC>::evaluate(
    bytecode_pointer<BC>::opcode_layout_t const& op,
    opcode_handlers const&                       handler)
{
    result_t        out_ = result_t::empty();
    opcode_layout_t out  = opcode_layout_t::void_();

    switch(op.exp_type)
    {
    case expression_t::expression:
    {
        switch(op.param_type)
        {
        case type_t::real_:
            out.real = op.real;
            break;
        case type_t::long_:
            out.set(op.template get<i32>());
            break;
        case type_t::short_:
            out.set(op.template get<i16>());
            break;
        case type_t::bool_:
            out.set(op.template get<u8>());
            break;
        default:
            out.set_ptr(op.to_ptr());
            break;
        }
        out.ret_type = out.param_type = op.param_type;
        out_                          = result_t::return_(out);

        if(handler.pre)
            handler.pre(*this, op);

        return out_.end_at(current_ip);
    }
    case expression_t::group:
    {
        if(handler.pre)
            handler.pre(*this, op);
        auto start_params = value_stack.size() - current_params;
        auto param_state  = evaluate_params(handler, op);
        auto op_params    = value_stack.size() - start_params;

        if(handler.post)
            handler.post(*this, op);

        if(param_state.state != eval::running)
        {
            return param_state;
        }

        bool externally_handled = false;

        switch(op.opcode)
        {
        case BC::begin:
        {
            /* The structure of begin
             *
             * 0000 begin_random -----------| goto end of group
             * 0001 begin_random ---------| | goto body
             *                            | |
             * 0002 ...             <<----| |
             * 0003                         |
             * 0004 <<----------------------|
             *
             */

            auto redirect = op_at(current_ip + 1).value();
            out_          = result_t::branch_to(redirect->next_op.ip + 1);

            break;
        }
        case BC::begin_random:
        {
            /* TODO: Find the linked list */
            stl_types::Vector<u16> random_case;

            /* The structure of begin_random
             *
             * 0000 begin_random -----------| goto end of group
             * 0001 begin_random ---------| | goto first option
             *                            | |
             * 0002 ...    --|      <<----| | goto next option
             * 0003          |              |
             * 0004      <<--|              |
             * 0005                         |
             * 0006 <<----------------------|
             *
             */
            auto element = op_at(current_ip).value();
            random_case.push_back(element[1].next_op.ip + 1);
            element = op_at(random_case.back()).value();

            do
            {
                random_case.push_back(element->next_op.ip + 1);
                element = op_at(element->next_op.ip + 1).value();
            } while(element->next_op.ip != terminator);

            out_ = result_t::branch_to(random_case.at(0));

            break;
        }
        case BC::set_:
        {
            auto global_ =
                context.global_by_ptr(param(type_t::any, 1).data_ptr).value();

            auto convert =
                get_as<BC>(param(param_type()), param_type(), global_->type);
            global_->set(*evaluate(convert, handler));

            break;
        }
        case BC::wake:
        {
            context.set_script_state(
                param(type_t::any).data_ptr, script_status::running, {});

            break;
        }
        case BC::sleep_until:
        {
            u16 cond_idx  = 0;
            u16 tick_rate = 1;
            u32 timeout   = 0;

            switch(op_params)
            {
            case 1:
                break;
            case 2:
                cond_idx  = 1;
                tick_rate = param(type_t::short_, 0).to_u16();
                break;
            case 3:
                cond_idx  = 2;
                tick_rate = param(type_t::short_, 1).to_u16();
                timeout   = param(type_t::long_).to_u32();
                break;
            }

            if(!param(type_t::bool_, cond_idx).to_bool())
                out_ = result_t::sleep_condition(
                    param_addr(cond_idx), tick_rate, timeout);
            break;
        }
        case BC::sleep:
        {
            opcode_layout_t const* timeout = nullptr;
            opcode_layout_t const* script  = nullptr;

            switch(op_params)
            {
            case 1:
            {
                auto const& first = param(type_t::any);
                switch(first.param_type)
                {
                case type_t::real_:
                case type_t::short_:
                {
                    timeout = &first;
                    break;
                }
                default:
                    script = &first;
                    break;
                }
                break;
            }
            case 2:
                script  = &param(type_t::any, 0);
                timeout = &param(type_t::any, 1);
                break;
            default:
                Throw(script_error("unhandled sleep case"));
            }

            i32 time = -1;
            if(timeout)
                switch(timeout->param_type)
                {
                case type_t::real_:
                    time = deref_real(*timeout);
                    break;
                case type_t::long_:
                    time = deref_i32(*timeout);
                    break;
                case type_t::short_:
                    time = deref_i16(*timeout);
                    break;
                }

            if(script)
                context.set_script_state(
                    script->data_ptr,
                    script_status::sleeping,
                    {sleep_condition::timer,
                     terminator,
                     stl_types::Chrono::seconds(time),
                     0});
            else
                out_ = result_t::sleep_timeout(time);

            break;
        }
        case BC::if_:
        {
            /* TODO: What do the bits in the short fields mean */

            /* The structure of if
             *
             * 0000 if           -----------| goto end of group
             * 0001 if           ---------| | goto condition
             *                            | |
             * 0002 bool_    ---|   <<----| | goto body
             * 0003             |           |
             * 0004 ...    <<---|           |
             * 0005                         |
             * 0006 <<----------------------|
             *
             */

            /* Case for script references, branch into and return value */
            if(param_type() != type_t::bool_)
            {
                out_ = result_t::branch_to(op.template get<u16>() + 1);
                break;
            }

            auto cond = param(type_t::bool_);

            auto branch_taken = false;

            if(cond.exp_type == expression_t::expression)
                branch_taken = cond.to_bool();
            else
                branch_taken = true;

            if(bypass_conditions)
                branch_taken = false;

            auto condition_op = op_at(param_addr()).value();
            auto true_branch  = op_at(condition_op->next_op.ip + 1).value();

            if(branch_taken)
            {
                out_ = result_t::branch_to(condition_op->next_op.ip + 1);
            } else if(true_branch->branching())
            {
                out_ = result_t::branch_to(true_branch->next_op.ip + 1);
            }

            break;
        }
        case BC::equal:
        case BC::nequal:
        {
            auto left  = *evaluate(param(type_t::any, 1), handler);
            auto right = *evaluate(param(type_t::any, 0), handler);

            if(left.ret_type != right.ret_type)
                Throw(script_error("mismatching types for comparison"));

            out = opcode_layout_t::typed_(type_t::bool_);
            out.set(true);

            break;
        }
        case BC::and_:
        case BC::or_:
        {
            auto left  = param(type_t::bool_, 1);
            auto right = param(type_t::bool_, 0);

            out = opcode_layout_t::typed_(type_t::bool_);

            if(op.opcode == BC::and_)
                out.set(left.to_bool() && right.to_bool());
            else
                out.set(left.to_bool() || right.to_bool());

            break;
        }
        case BC::not_:
        {
            out = opcode_layout_t::typed_(type_t::bool_);
            out.set(!(*evaluate(param(type_t::bool_, 0), handler)).to_bool());
            break;
        }
        case BC::add_:
        case BC::sub_:
        case BC::mul_:
        case BC::div_:
        {
            out         = opcode_layout_t::typed_(op.ret_type);
            auto left_  = param(type_t::number, 1);
            auto right_ = param(type_t::number, 0);

            if(!is_number(left_.ret_type) || !is_number(right_.ret_type))
                Throw(script_error("invalid input to arithmetic"));

            auto left  = get_as<BC>(left_, left_.ret_type, op.ret_type);
            auto right = get_as<BC>(right_, right_.ret_type, op.ret_type);

            switch(op.opcode)
            {
            case BC::add_:
                out = left + right;
                break;
            case BC::sub_:
                out = left - right;
                break;
            case BC::mul_:
                out = left * right;
                break;
            case BC::div_:
                out = left / right;
                break;
            }

            break;
        }
        case BC::min_:
        case BC::max_:
        {
            auto left_  = param(type_t::number, 1);
            auto right_ = param(type_t::number, 0);

            if(!is_number(left_.ret_type) || !is_number(right_.ret_type))
                Throw(script_error("invalid input to comparison"));

            auto left =
                get_as<BC>(left_, left_.ret_type, type_t::real_).to_real();
            auto right =
                get_as<BC>(right_, right_.ret_type, type_t::real_).to_real();

            opcode_layout_t result = opcode_layout_t::typed_(type_t::real_);

            switch(op.opcode)
            {
            case BC::min_:
                result.set(stl_types::math::min(left, right));
                break;
            case BC::max_:
                result.set(stl_types::math::max(left, right));
                break;
            }

            out = get_as<BC>(result, type_t::real_, op.ret_type);
            break;
        }
        case BC::random_range:
        {
            auto min = param(type_t::short_, 1).to_u16();
            auto max = param(type_t::short_).to_u16();

            out = opcode_layout_t::typed_(type_t::short_);
            out.set(min);

            /* TODO: Add random */

            break;
        }
        case BC::real_random_range:
        {
            auto min = param(type_t::real_, 1).to_real();
            auto max = param(type_t::real_).to_real();

            out = opcode_layout_t::typed_(type_t::real_);
            out.set(min);

            /* TODO: Add random */

            break;
        }
        default:
        {
            externally_handled = true;
            auto grp           = handler.cmd(*this, op);
            out_               = grp;
            out                = grp.result;
            break;
        }
        }

        if(!externally_handled)
            handler.cmd(*this, op);

        if(param_count(op) == variable_length_params)
            pop_params(op_params);
        else
            pop_params(param_count(op));

        out_ = out_.end_at(param_state.end_addr);
        if(out_.state != eval::running || out_.next != terminator)
            return out_;

        return result_t::return_(out);
    }
    case expression_t::global_ref:
    {
        if(handler.pre)
            handler.pre(*this, op);

        auto global_opt = context.global_by_ptr(op.data_ptr);

        if(global_opt)
        {
            global_value* global_ = global_opt.value();
            out                   = global_->get_as<BC>(op.ret_type);
            out.data_ptr          = op.data_ptr;
            return result_t::return_(out).end_at(current_ip);
        } else
            return handler.cmd(*this, op);
    }
    case expression_t::script_ref:
    {
        if(handler.pre)
            handler.pre(*this, *(current + 1));

        auto proc_it = context.procedures.find((current + 1)->data_ptr);

        if(proc_it == context.procedures.end())
            Throw(script_error("failed to find procedure"));

        return result_t::branch_to(proc_it->second.script_start);
    }
    default:
        Throw(script_error("unhandled expression type"));
    }

    return out_.end_at(current_ip);
}

} // namespace hsc
} // namespace blam
