#include <shader_proc/compiler_passes.h>

#include <optional>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/any_of.h>

#include <source/opt/ir_context.h>
#include <source/opt/module.h>
#include <source/opt/pass.h>

#include "spirv.hpp"
#include <spirv-tools/optimizer.hpp>
#include <spirv/unified1/spirv.h>

namespace shader_proc::opt::passes {

namespace {

using spvtools::opt::Instruction;

class LegacyCompatibilityPass : public spvtools::opt::Pass
{
  public:
    LegacyCompatibilityPass(compatibility_options&& options) :
        options(std::move(options))
    {
    }

    const char* name() const
    {
        return "shader_proc::opt::passes::LegacyCompatibilityPass";
    }

  protected:
    struct BufferInformation
    {
        std::string buffer_name;
        uint32_t    id;
    };

    Status Process()
    {
        Status out = Status::SuccessWithoutChange;
        get_module()->ForEachInst([this, &out](Instruction* instruction) {
            if(out == Status::Failure)
                return;
            auto current_status = per_instruction(instruction);
            if(current_status == Status::SuccessWithoutChange)
                return;
            out = current_status;
        });
        return out;
    }

    auto create_instruction(SpvOp op)
    {
        return std::make_unique<Instruction>(context(), op);
    }

    auto operand(spv_operand_type_t type, uint32_t a)
    {
        return spvtools::opt::Operand(
            type, spvtools::opt::Operand::OperandData({a}));
    }

    std::optional<Status> translate_ssbo_as_ubo(Instruction* instruction)
    {
        switch(instruction->opcode())
        {
        case SpvOpDecorate: {
            auto& type_op = instruction->GetInOperand(1);
            if(type_op.words[0] != spv::DecorationBufferBlock)
                return std::nullopt;
            auto decorator     = instruction->GetInOperand(1);
            decorator.words[0] = spv::DecorationBlock;
            instruction->RemoveInOperand(1);
            instruction->AddOperand(std::move(decorator));
            return Status::SuccessWithChange;
        }
        case SpvOpTypeRuntimeArray: {
            auto identifier = instruction->GetInOperand(0);
            instruction->SetOpcode(SpvOpTypeArray);

            spv_parsed_instruction_t            const_type{};
            std::array<spv_parsed_operand_t, 3> type_operands = {{
                spv_parsed_operand_t{
                    0, 1, SPV_OPERAND_TYPE_RESULT_ID, SPV_NUMBER_NONE, 32},
                spv_parsed_operand_t{
                    1,
                    1,
                    SPV_OPERAND_TYPE_LITERAL_INTEGER,
                    SPV_NUMBER_UNSIGNED_INT,
                    32},
                spv_parsed_operand_t{
                    2,
                    1,
                    SPV_OPERAND_TYPE_LITERAL_INTEGER,
                    SPV_NUMBER_UNSIGNED_INT,
                    32},
            }};

            const_type.opcode       = SpvOpTypeInt;
            const_type.type_id      = SPV_OPERAND_TYPE_TYPE_ID;
            const_type.result_id    = context()->TakeNextId();
            const_type.operands     = type_operands.data();
            const_type.num_operands = 3;
            std::array<uint32_t, 3> type_words
                = {{const_type.result_id, 32, 1}};
            const_type.words     = type_words.data();
            const_type.num_words = 3;

            spv_parsed_instruction_t            constant{};
            std::array<spv_parsed_operand_t, 3> constant_operands = {{
                spv_parsed_operand_t{
                    0, 1, SPV_OPERAND_TYPE_TYPE_ID, SPV_NUMBER_NONE, 32},
                spv_parsed_operand_t{
                    1, 1, SPV_OPERAND_TYPE_RESULT_ID, SPV_NUMBER_NONE, 32},
                spv_parsed_operand_t{
                    2,
                    1,
                    SPV_OPERAND_TYPE_LITERAL_INTEGER,
                    SPV_NUMBER_UNSIGNED_INT,
                    32},
            }};

            constant.opcode       = SpvOpSpecConstant;
            constant.type_id      = SPV_OPERAND_TYPE_RESULT_ID;
            constant.result_id    = context()->TakeNextId();
            constant.operands     = constant_operands.data();
            constant.num_operands = 3;
            std::array<uint32_t, 3> constant_words
                = {{const_type.result_id, constant.result_id, 1}};
            constant.words     = constant_words.data();
            constant.num_words = 3;

            instruction->InsertBefore(
                std::make_unique<Instruction>(context(), const_type));
            instruction->InsertBefore(
                std::make_unique<Instruction>(context(), constant));
            instruction->AddOperand(
                operand(SPV_OPERAND_TYPE_RESULT_ID, constant.result_id));

            /* Find the referenced buffer's name,
             * and use it to create the length name */
            Instruction* next = instruction->NextNode();
            while(next)
            {
                auto op = next->opcode();
                if(op == SpvOpTypeStruct
                   && next->GetInOperand(0).words[0]
                          == instruction->result_id())
                    break;
                next = next->NextNode();
            }

            if(!next)
                return Status::SuccessWithChange;

            auto names = context()->GetNames(next->result_id());

            if(names.empty())
                return Status::SuccessWithChange;

            auto name = names.begin()->second->GetInOperand(1).AsString();

            std::string len_name = name + "_length";

            if(len_name.size() & 0b11)
            {
                len_name.resize(
                    len_name.size() + 4 - (len_name.size() & 0b11), '\0');
            }
            uint16_t name_word_count = len_name.size() / sizeof(uint32_t);

            spv_parsed_instruction_t            constant_name{};
            std::array<spv_parsed_operand_t, 2> name_operands = {{
                spv_parsed_operand_t{
                    0, 1, SPV_OPERAND_TYPE_RESULT_ID, SPV_NUMBER_NONE, 32},
                spv_parsed_operand_t{
                    1,
                    name_word_count,
                    SPV_OPERAND_TYPE_LITERAL_STRING,
                    SPV_NUMBER_NONE,
                    0},
            }};
            std::vector<uint32_t>    name_words = {constant.result_id};
            semantic::Span<uint32_t> name_cast(
                reinterpret_cast<uint32_t*>(len_name.data()), name_word_count);
            name_words.insert(
                name_words.end(), name_cast.begin(), name_cast.end());

            constant_name.opcode       = SpvOpName;
            constant_name.num_operands = 2;
            constant_name.operands     = name_operands.data();
            constant_name.num_words    = name_words.size();
            constant_name.words        = name_words.data();

            next->InsertBefore(
                std::make_unique<Instruction>(context(), constant_name));

            return Status::SuccessWithChange;
        }
        default:
            return std::nullopt;
        }
    }

    std::optional<Status> remove_flat_qualifier(Instruction* instruction)
    {
        switch(instruction->opcode())
        {
        case SpvOpDecorate: {
            if(instruction->GetInOperand(1).words[0] != spv::DecorationFlat)
                return std::nullopt;
            instruction->ToNop();
            return Status::SuccessWithChange;
        }
        case SpvOpMemberDecorate: {
            if(instruction->GetInOperand(2).words[0] != spv::DecorationFlat)
                return std::nullopt;
            instruction->ToNop();
            return Status::SuccessWithChange;
        }
        default:
            return std::nullopt;
        }
    }

    std::optional<Status> unsigned_to_signed_int(Instruction* instruction)
    {
        switch(instruction->opcode())
        {
        case SpvOpTypeInt: {
            if(instruction->GetInOperand(1).words[0])
                return std::nullopt;
            instruction->GetInOperand(1).words[0] = 1;
            return Status::SuccessWithChange;
        }
        default:
            return std::nullopt;
        }
    }

    std::optional<Status> float64_to_float32(Instruction* instruction)
    {
        switch(instruction->opcode())
        {
        case SpvOpTypeFloat: {
            if(instruction->GetInOperand(0).words[0] == 32)
                return std::nullopt;
            instruction->GetInOperand(0).words[0] = 32;
            return Status::SuccessWithChange;
        }
        default:
            return std::nullopt;
        }
    }

    std::optional<Status> int_stage_input_to_float(Instruction* instruction)
    {
//        Instruction* type_def = nullptr;
        switch(instruction->opcode())
        {
        case SpvOpAccessChain: {
            Instruction* type = context()->get_def_use_mgr()->GetDef(
                instruction->GetOperand(0).words[0]);
            if(!stl_types::any_of(
                   type->GetSingleWordInOperand(0),
                   spv::StorageClassInput,
                   spv::StorageClassOutput))
                return std::nullopt;
            break;
        }
        default:
            return std::nullopt;
        }

        // TODO: Rewrite int structures to float
        //        throw std::runtime_error("unimplemented cast between int and
        //        float");

        return Status::SuccessWithChange;
    }

    Status per_instruction(Instruction* instruction)
    {
        Status out = Status::SuccessWithoutChange;
        if(options.ssbo_as_ubo && out != Status::Failure)
            out = translate_ssbo_as_ubo(instruction).value_or(out);
        /* TODO: UBO data as uniforms */
        if(options.remove_flat_qualifier && out != Status::Failure)
            out = remove_flat_qualifier(instruction).value_or(out);
        if(options.unsigned_to_signed_int && out != Status::Failure)
            out = unsigned_to_signed_int(instruction).value_or(out);
        if(options.float64_to_float32 && out != Status::Failure)
            out = float64_to_float32(instruction).value_or(out);
        if(options.int_stage_input_to_float && out != Status::Failure)
            out = int_stage_input_to_float(instruction).value_or(out);
        return out;
    }

  private:
    compatibility_options options;
};

class RenameEntrypointPass : public spvtools::opt::Pass
{
  public:
    RenameEntrypointPass(std::string_view new_name)
    {
        auto size = new_name.size();
        auto padded = size & 0b11;
        size += 4;
        if(padded > 0)
            size -= padded;
        m_entrypoint_name.resize(size / 4);
        auto out = semantic::mem_chunk<char>::ofContainer(m_entrypoint_name);
        std::copy(new_name.begin(), new_name.end(), out.begin());
    }

    const char* name() const
    {
        return "shader_proc::opt::passes::RenameEntrypointPass";
    }

  protected:
    Status Process()
    {
        Status result = Status::SuccessWithoutChange;
        get_module()->ForEachInst([this, &result](Instruction* instruction)
        {
            auto current_status = per_instruction(instruction);
            if(current_status == Status::SuccessWithoutChange)
                return;
            result = current_status;
        });

        return result;
    }

    Status per_instruction(Instruction* instruction)
    {
        using OperandData = spvtools::opt::Operand::OperandData;
        if(instruction->opcode() == SpvOpEntryPoint)
        {
            instruction->SetInOperand(2, OperandData(m_entrypoint_name));
            return Status::SuccessWithChange;
        }
        return Status::SuccessWithoutChange;
    }

  private:
    std::vector<uint32_t> m_entrypoint_name;
};

} // namespace

spvtools::Optimizer::PassToken CreateLegacyCompatibilityPass(
    compatibility_options&& options)
{
    return spvtools::Optimizer::PassToken(
        std::unique_ptr<spvtools::opt::Pass>(dynamic_cast<spvtools::opt::Pass*>(
            new LegacyCompatibilityPass(std::move(options)))));
}

spvtools::Optimizer::PassToken CreateRenameEntrypointPass(
    std::string_view new_name)
{
    return spvtools::Optimizer::PassToken(
        std::unique_ptr<spvtools::opt::Pass>(dynamic_cast<spvtools::opt::Pass*>(
            new RenameEntrypointPass(new_name))));
}

} // namespace shader_proc::opt::passes
