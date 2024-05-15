#include "instruction.h"

const Cpu_Instruction_Type opcode_to_type_map[] = {
    [0x00] = CPU_INSTRUCTION_REGISTER,
    [0x02] = CPU_INSTRUCTION_JUMP,
    [0x03] = CPU_INSTRUCTION_JUMP,
    [0x10 ... 0x13] = CPU_INSTRUCTION_COPROCESSOR,
    [0x04 ... 0x0f] = CPU_INSTRUCTION_IMMEDIATE,
    [0x14 ... 0x2b] = CPU_INSTRUCTION_IMMEDIATE,
};

Cpu_Instruction cpu_instruction_decode(u32 word)
{
    Cpu_Instruction instruction = {0};
    instruction.as_word = word;
    instruction.type = opcode_to_type_map[instruction.opcode];

    return instruction;
}
