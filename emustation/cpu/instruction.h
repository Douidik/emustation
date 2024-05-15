#ifndef EMUSTATION_CPU_INSTRUCTION_H
#define EMUSTATION_CPU_INSTRUCTION_H

#include "common.h"

typedef enum Cpu_Instruction_Type
{
    CPU_INSTRUCTION_TYPE_NONE,
    CPU_INSTRUCTION_COPROCESSOR,
    CPU_INSTRUCTION_REGISTER,
    CPU_INSTRUCTION_IMMEDIATE,
    CPU_INSTRUCTION_JUMP,
} Cpu_Instruction_Type;

typedef enum Cpu_Opcode
{
    CPU_OPCODE_REGISTER = 0x00,
    CPU_OPCODE_SLTI = 0x0a,
    CPU_OPCODE_SLTIU = 0x0b,
    CPU_OPCODE_ANDI = 0x0c,
    CPU_OPCODE_ORI = 0x0d,
    CPU_OPCODE_LUI = 0x0f,
    CPU_OPCODE_J = 0x02,
    CPU_OPCODE_SW = 0x2b,
    CPU_OPCODE_JAL = 0x03,
    CPU_OPCODE_BEQ = 0x04,
    CPU_OPCODE_BNE = 0x05,
    CPU_OPCODE_BLEZ = 0x06,
    CPU_OPCODE_BGTZ = 0x07,
    CPU_OPCODE_ADDI = 0x08,
    CPU_OPCODE_ADDIU = 0x09,
    CPU_OPCODE_LB = 0x20,
    CPU_OPCODE_LH = 0x21,
    CPU_OPCODE_LW = 0x23,
    CPU_OPCODE_LBU = 0x24,
    CPU_OPCODE_LHU = 0x25,
    CPU_OPCODE_SB = 0x28,
    CPU_OPCODE_SH = 0x29,
} Cpu_Opcode;

typedef enum Cpu_Function
{
    CPU_FUNCTION_SLL = 0x00,
    CPU_FUNCTION_SRL = 0x02,
    CPU_FUNCTION_SRA = 0x03,
    CPU_FUNCTION_JR = 0x08,
    CPU_FUNCTION_JALR = 0x09,
    CPU_FUNCTION_MFHI = 0x10,
    CPU_FUNCTION_MTHI = 0x11,
    CPU_FUNCTION_MFLO = 0x12,
    CPU_FUNCTION_MTLO = 0x13,
    CPU_FUNCTION_MULT = 0x18,
    CPU_FUNCTION_DIV = 0x1a,
    CPU_FUNCTION_DIVU = 0x1b,
    CPU_FUNCTION_MULTU = 0x19,
    CPU_FUNCTION_ADD = 0x20,
    CPU_FUNCTION_ADDU = 0x21,
    CPU_FUNCTION_SUB = 0x22,
    CPU_FUNCTION_SUBU = 0x23,
    CPU_FUNCTION_AND = 0x24,
    CPU_FUNCTION_OR = 0x25,
    CPU_FUNCTION_XOR = 0x26,
    CPU_FUNCTION_NOR = 0x27,
    CPU_FUNCTION_SLT = 0x2a,
    CPU_FUNCTION_SLTU = 0x2b,
} Cpu_Function;

// cpu processor extended opcode: opcode + coprocessor_id + instruction_type
#define E(coprocessor_id, pseudo_opcode, function) \
    (((pseudo_opcode & 0xf) << 7) | ((coprocessor_id & 0x4) << 5) | ((function & 0x1f)))
typedef enum Cpu_Coprocessor_Extended_Opcode
{
    CPU_SYS_RFE = E(0, 0x04, 0x10), // tlb instructions are not supported, this mask should be enough

    CPU_SYS_PEEK = E(0, 0x04, 0x00),
    CPU_SYS_POKE = E(0, 0x04, 0x02),
    CPU_GTE_PEEK = E(2, 0x04, 0x00),
    CPU_GTE_POKE = E(2, 0x04, 0x02),

    CPU_GTE_CONTROL_PEEK = E(2, 0x04, 0x4),
    CPU_GTE_CONTROL_POKE = E(2, 0x06, 0x4),
} Cpu_Coprocessor_Extended_Opcode;
#undef E

typedef enum Gte_Extended_Opcode
{
    GTE_RTPS = 0x00,
    GTE_NCLIP = 0x08,
    GTE_OP = 0x06,
    GTE_DPCS = 0x10,
    GTE_INTPL = 0x11,
    GTE_MVMVA = 0x12,
    GTE_NCDS = 0x13,
    GTE_CDP = 0x14,
    GTE_NCDT = 0x16,
    GTE_NCCS = 0x1b,
    GTE_CC = 0x1c,
    GTE_NCS = 0x1e,
    GTE_NCT = 0x20,
    GTE_SQR = 0x28,
    GTE_DCPL = 0x29,
    GTE_DPCT = 0x2a,
    GTE_AVSZ3 = 0x2d,
    GTE_AVSZ4 = 0x2e,
    GTE_RTPT = 0x30,
    GTE_GPF = 0x3d,
    GTE_GPL = 0x3e,
    GTE_NCCT = 0x3f,
} Gte_Extended_Opcode;

typedef struct Gte_Instruction
{
    u32 opcode : 6;
    u32 : 4;
    u32 lm : 1;
    u32 : 2;
    u32 mvma_tsl : 2;
    u32 mvma_mul_vec : 2;
    u32 mvma_mul_mat : 2;
    u32 sf;
    u32 : 12;
} Gte_Instruction;

typedef struct Cpu_Instruction_Coprocessor
{
    union {
        struct
        {
            u8 opcode : 6;
            u8 funct : 5;
        };
        struct
        {
            u8 pseudo_opcode : 4;
            u8 coprocessor_id : 2;
        };

        Cpu_Coprocessor_Extended_Opcode extended_opcode : 11;
    };

    union {
        struct // move coprocessor type
        {
            u8 rt : 5;
            u8 rd : 5;
        };
    };
} Cpu_Instruction_Coprocessor;

typedef struct Cpu_Instruction_Register
{
    Cpu_Opcode opcode : 6;
    u8 rs : 5;
    u8 rt : 5;
    u8 rd : 5;
    u8 shamt : 5;
    u8 function : 6;
} Cpu_Instruction_Register;

typedef struct Cpu_Instruction_Immediate
{
    Cpu_Opcode opcode : 6;
    u8 rs : 5;
    u8 rt : 5;
    union {
        u16 x : 16;
        i16 sx : 16;
    };
} Cpu_Instruction_Immediate;

typedef struct Cpu_Instruction_Jump
{
    Cpu_Opcode opcode : 6;
    u32 pseudo_address : 26;
} Cpu_Instruction_Jump;

typedef struct Cpu_Instruction
{
    Cpu_Instruction_Type type;
    union {
        Cpu_Opcode opcode : 6;
        u32 as_word;
        Cpu_Instruction_Coprocessor as_coprocessor;
        Cpu_Instruction_Register as_register;
        Cpu_Instruction_Immediate as_immediate;
        Cpu_Instruction_Jump as_jump;
    };
} Cpu_Instruction;

Cpu_Instruction cpu_instruction_decode(u32 word);

#endif
