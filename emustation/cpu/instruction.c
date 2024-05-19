#include "instruction.h"

#define COPROCESSOR_OPCODE_MASK (0x40000000)
#define COPROCESSOR_ID_MASK (0x0c000000)

const Cpu_Instruction_Type cpu_opcode_to_type_map[] = {
    [0x00] = CPU_INSTRUCTION_REGISTER,
    [0x02] = CPU_INSTRUCTION_JUMP,
    [0x03] = CPU_INSTRUCTION_JUMP,
    [0x04 ... 0x0f] = CPU_INSTRUCTION_IMMEDIATE,
    [0x14 ... 0x2b] = CPU_INSTRUCTION_IMMEDIATE,
};

Syscop_Opcode make_syscop_opcode(Syscop_Instruction instruction)
{
    return (instruction.opcode_0 << 5) | (instruction.opcode_1);
}

Gte_Opcode make_gte_opcode(Gte_Instruction instruction)
{
    if (instruction.is_command)
        return GTE_COMMAND;
    else if (instruction.pseudo_opcode & GTE_SWC)
        return GTE_SWC;
    else if (instruction.pseudo_opcode & GTE_LWC)
        return GTE_LWC;
    else
        return (instruction.move.opcode_0 << 5) | (instruction.move.opcode_1);
}

Cpu_Instruction syscop_instruction_decode(u32 word)
{
    Cpu_Instruction instruction;
    instruction.as_word = word;
    instruction.as_syscop.opcode = make_syscop_opcode(instruction.as_syscop);

    return instruction;
}

Cpu_Instruction gte_instruction_decode(u32 word)
{
    Cpu_Instruction instruction;
    instruction.as_word = word;
    instruction.as_gte.opcode = make_gte_opcode(instruction.as_gte);

    return instruction;
}

Cpu_Instruction coprocessor_instruction_decode(u32 word)
{
    switch ((word & COPROCESSOR_ID_MASK) >> 26) {
    case 0:
        return syscop_instruction_decode(word);
    case 2:
        return gte_instruction_decode(word);
    default:
        return (Cpu_Instruction){.type = CPU_INSTRUCTION_COPROCESSOR_UNKNOWN};
    }
}

Cpu_Instruction cpu_instruction_decode(u32 word)
{
    if (word & COPROCESSOR_OPCODE_MASK) {
        return coprocessor_instruction_decode(word);
    }

    Cpu_Instruction instruction = {0};
    instruction.as_word = word;
    instruction.type = cpu_opcode_to_type_map[instruction.opcode];

    return instruction;
}
