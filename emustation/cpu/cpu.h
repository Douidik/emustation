#ifndef EMUSTATION_CPU_H
#define EMUSTATION_CPU_H

#include "common.h"
#include "gte.h"
#include "instruction.h"
#include "syscop.h"

// note! maybe need to enchance the r0 design
// the r0 always needs to be equal to zero
// some instruction may corrupt it's value
// a solution could be to interface the poke of registers
// through a function that doesn't record the pokes to r0
// this would be mandatory if an instruction can poke then peek r0

typedef struct Cpu Cpu;
typedef void *(*Cpu_Update_Cb)(struct Cpu *, Cpu_Instruction, void *);

struct Cpu
{
    struct Bus *bus;
    Syscop syscop;
    Gte gte;
    Cpu_Update_Cb update_cb;
    void *update_cb_data;

    struct
    {
        union {
            i32 sregs[32];
            u32 regs[32];
        };
        u32 zero;
        u32 at;
        u32 v0, v1;
        u32 a0, a1, a2, a3;
        u32 t0, t1, t2, t3, t4, t5, t6, t7;
        u32 s0, s1, s2, s3, s4, s5, s6, s7;
        u32 t8, t9;
        u32 k0, k1;
        u32 gp;
        u32 sp;
        u32 fp;
        u32 ra;
    };
    union {
        u32 lo;
        i32 slo;
    };
    union {
        u32 hi;
        i32 shi;
    };
    u32 pc;
    u32 next_pc;
};

void cpu_init(Cpu *cpu, struct Bus *bus);
void cpu_hook_update_cb(Cpu *cpu, Cpu_Update_Cb update_cb, void *data);
void cpu_update(Cpu *cpu);

void cpu_execute_syscop_instruction(Cpu *cpu, Syscop_Instruction instruction);
void cpu_execute_gte_instruction(Cpu *cpu, Gte_Instruction instruction);

void cpu_execute_register_instruction(Cpu *cpu, Cpu_Instruction_Register instruction);
void cpu_execute_immediate_instruction(Cpu *cpu, Cpu_Instruction_Immediate instruction);
void cpu_execute_jump_instruction(Cpu *cpu, Cpu_Instruction_Jump instruction);

void cpu_branch_when(Cpu *cpu, bool instruction_link, u32 imm_address, bool condition);
void cpu_throw_exception(Cpu *cpu, Cpu_Exception_Type type);
i32 cpu_execute_signed_add(Cpu *cpu, i32 r, i32 x, i32 y);

static const char *Cpu_Regs_Name[] = {
    "r0", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra",
};

static const char *Cpu_Opcode_Mnemonic[] = {
    [0x0a] = "stli", [0x0b] = "stliu", [0x0c] = "andi", [0x0d] = "ori", [0x0f] = "lui",  [0x02] = "j",
    [0x2b] = "sw",   [0x03] = "jal",   [0x04] = "beq",  [0x05] = "bne", [0x06] = "blez", [0x07] = "bgtz",
    [0x08] = "addi", [0x09] = "addiu", [0x20] = "lb",   [0x21] = "lh",  [0x23] = "lw",   [0x24] = "lbu",
    [0x25] = "lhu",  [0x28] = "sb",    [0x29] = "sh",
};

static const char *Cpu_Function_Mnemonic[] = {
    [0x00] = "sll",   [0x02] = "srl",  [0x03] = "sra",  [0x08] = "jr",   [0x09] = "jalr", [0x10] = "mfhi",
    [0x11] = "mthi",  [0x12] = "mflo", [0x13] = "mtlo", [0x18] = "mult", [0x1a] = "div",  [0x1b] = "divu",
    [0x19] = "multu", [0x20] = "add",  [0x21] = "addu", [0x22] = "sub",  [0x23] = "subu", [0x24] = "and",
    [0x25] = "or",    [0x26] = "xor",  [0x27] = "nor",  [0x2a] = "slt",  [0x2b] = "sltu",
};

void *cpu_print_instruction(Cpu *cpu, Cpu_Instruction instruction, void *f);

#endif
