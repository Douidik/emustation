#ifndef EMUSTATION_CPU_INSTRUCTION_H
#define EMUSTATION_CPU_INSTRUCTION_H

#include "common.h"

typedef enum Cpu_Instruction_Type
{
    CPU_INSTRUCTION_TYPE_NONE,
    CPU_INSTRUCTION_REGISTER,
    CPU_INSTRUCTION_IMMEDIATE,
    CPU_INSTRUCTION_JUMP,
    CPU_INSTRUCTION_COPROCESSOR_UNKNOWN,
    CPU_INSTRUCTION_SYSCOP,
    CPU_INSTRUCTION_GTE,
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

// coprocessor instructions
// cop0:
// mfc0 mtf0 rfe
// cop2:
// mfc2 mfc2 imm25 (command) ctc2 cfc2

// note! make sure the Cpu_Instruction.word doesn't get corrupted
// because the coprocessor instructions are embedded in a union

typedef struct Coprocessor_Move
{
    u8 : 6;
    u8 opcode_0 : 5;
    u8 rt : 5;
    u8 rd : 5;
    u8 : 5;
    u8 opcode_1 : 6;
} Coprocessor_Move;

// load / store word coprocessor
typedef struct Coprocessor_Lsw
{
    u8 rs : 5;
    u8 rt : 5;
    i16 offset : 16;
} Coprocessor_Lsw;

// note! commented opcodes are not implemented on ps1 hardware
// throw exception or ignore if encounted ?
typedef enum Syscop_Opcode
{
    SYSCOP_MFC = 0x000,
    SYSCOP_MTC = 0x100,
    SYSCOP_RFE = 0x410,
} Syscop_Opcode;

typedef struct Syscop_Instruction
{
    // mfc, mtc
    Coprocessor_Move as_move;
    Syscop_Opcode opcode;
} Syscop_Instruction;

typedef enum Gte_Opcode
{
    // move opcodes
    GTE_MFC = 0x000,
    GTE_CFC = 0x080,
    GTE_MTC = 0x100,
    GTE_CTC = 0x180,

    // note! opcodes below are decoded through masks
    GTE_COMMAND = 0x400,
    GTE_LWC = 0x00c,
    GTE_SWC = 0x00e,
} Gte_Opcode;

typedef enum Gte_Command_Opcode
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
} Gte_Command_Opcode;

typedef struct Gte_Command
{
    u8 : 1;
    u8 opcode : 5;
    u8 : 3;
    u8 lm : 1; // saturate ir1, ir2, ir3
    u8 : 2;
    u8 mvmva_tr : 2; // mvmva translaton vector
    u8 mvmva_mv : 2; // mvmva multiply vector
    u8 mvmva_mm : 2; // mvmva multiply matrix
    u8 sf : 1;       // shift fraction in ir registers
} Gte_Command;

typedef struct Gte_Instruction
{
    u8 pseudo_opcode : 4;
    u8 : 2;
    union {
        u8 is_command : 1;

        // lwc, swc
        Coprocessor_Lsw as_lsw;

        // mfc, cfc, mtc, ctc
        Coprocessor_Move as_move;

        // imm25 (gte command)
        Gte_Command as_command;
    };

    Gte_Opcode opcode;
} Gte_Instruction;

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
        Syscop_Instruction as_syscop;
        Gte_Instruction as_gte;
        Cpu_Instruction_Immediate as_immediate;
        Cpu_Instruction_Register as_register;
        Cpu_Instruction_Jump as_jump;
    };
} Cpu_Instruction;

Cpu_Instruction cpu_instruction_decode(u32 word);
Syscop_Opcode make_syscop_opcode(Syscop_Instruction instruction);
Gte_Opcode make_gte_opcode(Gte_Instruction instruction);

#endif
