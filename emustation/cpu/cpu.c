#include "cpu.h"
#include "bus.h"
#include "report.h"
#include <stdio.h>
#include <string.h>

const Cpu_Instruction NOP = {
    .as_word = 0,
};

// todo! use the instruction bitfield instead
// built-in c sign extend with struct bitfields
#define Sign_Extend(type, name, bits) \
    type name(type x)                 \
    {                                 \
        struct                        \
        {                             \
            type x : bits;            \
        } s;                          \
        return s.x = x;               \
    }
Sign_Extend(u32, sign_extend_byte_to_word, 8);
Sign_Extend(u32, sign_extend_hword_to_word, 16);
Sign_Extend(u32, sign_extend_imm_address, 18);

void cpu_init(Cpu *cpu, struct Bus *bus)
{
    report->trace("cpu init");
    memset(cpu, 0, sizeof(Cpu));
    cpu->bus = bus;
    cpu->pc = 0xbfc00000;

    syscop_init(&cpu->syscop);
}

void cpu_hook_update_cb(Cpu *cpu, Cpu_Update_Cb update_cb, void *data)
{
    cpu->update_cb = update_cb;
    cpu->update_cb_data = data;
}

Cpu_Instruction cpu_fetch_instruction(Cpu *cpu)
{
    if (cpu->pc & 3) {
        // cannot fetch instruction with unaligned program counter
        cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        return NOP;
    }

    u32 word = bus_peek_word(cpu->bus, cpu->pc);
    return cpu_instruction_decode(word);
};

void cpu_update(Cpu *cpu)
{
    Cpu_Instruction instruction = cpu_fetch_instruction(cpu);
    cpu->next_pc = cpu->pc + 4;

    if (cpu->update_cb != NULL)
        cpu->update_cb(cpu, instruction, cpu->update_cb_data);

    switch (instruction.type) {
    case CPU_INSTRUCTION_TYPE_NONE:
        cpu_throw_exception(cpu, CPU_EXCEPTION_ILLEGAL_INSTRUCTION);
        return;

    case CPU_INSTRUCTION_COPROCESSOR_UNKNOWN:
        cpu_throw_exception(cpu, CPU_EXCEPTION_COPROCESSOR);
        return;

    case CPU_INSTRUCTION_GTE:
        cpu_execute_gte_instruction(cpu, instruction.as_gte);
        break;

    case CPU_INSTRUCTION_SYSCOP:
        cpu_execute_syscop_instruction(cpu, instruction.as_syscop);
        break;

    case CPU_INSTRUCTION_REGISTER:
        cpu_execute_register_instruction(cpu, instruction.as_register);
        break;

    case CPU_INSTRUCTION_IMMEDIATE:
        cpu_execute_immediate_instruction(cpu, instruction.as_immediate);
        break;

    case CPU_INSTRUCTION_JUMP:
        cpu_execute_jump_instruction(cpu, instruction.as_jump);
        break;
    }

    cpu->pc = cpu->next_pc;
    cpu->zero = 0;
}

// R -> unsigned cpu registers, SR -> signed cpu registers
#define R cpu->regs
#define SR cpu->sregs

void cpu_execute_syscop_instruction(Cpu *cpu, Syscop_Instruction instruction)
{
    Coprocessor_Move move = instruction.as_move;

    switch (instruction.opcode) {
    case SYSCOP_MFC:
        R[move.rt] = syscop_peek(&cpu->syscop, move.rd);
        break;
    case SYSCOP_MTC:
        syscop_poke(&cpu->syscop, move.rd, R[move.rt]);
        break;
    case SYSCOP_RFE:
        syscop_rfe(&cpu->syscop);
        break;
    default:
        cpu_throw_exception(cpu, CPU_EXCEPTION_ILLEGAL_INSTRUCTION);
        break;
    }
}

void cpu_execute_gte_instruction(Cpu *cpu, Gte_Instruction instruction)
{
    Coprocessor_Move move = instruction.as_move;
    Coprocessor_Lsw lsw = instruction.as_lsw;
    Gte_Command command = instruction.as_command;

    switch (instruction.opcode) {
    case GTE_MFC:
        R[move.rt] = gte_peek(&cpu->gte, move.rd);
        break;

    case GTE_CFC:
        R[move.rt] = gte_peek_control(&cpu->gte, move.rd);
        break;

    case GTE_MTC:
        gte_poke(&cpu->gte, move.rd, R[move.rt]);
        break;

    case GTE_CTC:
        gte_poke_control(&cpu->gte, move.rd, R[move.rt]);
        break;

    case GTE_COMMAND:

        break;

    case GTE_LWC: {
        u32 address = R[lsw.rs] + lsw.offset;

        if (!(address & 3)) {
            u32 word = bus_peek_word(cpu->bus, address);
            gte_poke(&cpu->gte, lsw.rt, word);
        } else {
            cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        }
    } break;

    case GTE_SWC: {
        u32 address = R[lsw.rs] + lsw.offset;
	
        if (!(address & 3)) {
            u32 word = gte_peek(&cpu->gte, lsw.rt);
            bus_poke_word(cpu->bus, address, word);
        } else {
            cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        }
    } break;

    default:
        cpu_throw_exception(cpu, CPU_EXCEPTION_ILLEGAL_INSTRUCTION);
        break;
    }
}

void cpu_execute_register_instruction(Cpu *cpu, Cpu_Instruction_Register instruction)
{
    u8 rd = instruction.rd;
    u8 rs = instruction.rs;
    u8 rt = instruction.rt;
    u8 shamt = instruction.shamt;

    switch (instruction.function) {
    case CPU_FUNCTION_SLL: // Shift left logical
        R[rd] = R[rt] << shamt;
        break;

    case CPU_FUNCTION_SRL: // Shift right logical
        R[rd] = R[rt] >> shamt;
        break;

    case CPU_FUNCTION_SRA: // Shift right logical (in C shifting a signed value will preserve the sign)
        SR[rd] = SR[rt] >> (i8)shamt;
        break;

    case CPU_FUNCTION_JR: // Jump register
        cpu->next_pc = R[rs];
        break;

    case CPU_FUNCTION_JALR: // Jump and link register
        R[rd] = cpu->pc + 4;
        cpu->next_pc = R[rs];
        break;

    case CPU_FUNCTION_MFHI: // 	Move from HI
        R[rd] = cpu->hi;
        break;

    case CPU_FUNCTION_MTHI: // Move to HI
        cpu->hi = R[rd];
        break;

    case CPU_FUNCTION_MFLO: // Move from LO
        R[rd] = cpu->lo;
        break;

    case CPU_FUNCTION_MTLO: // Move to LO
        cpu->lo = R[rd];
        break;

#define Div(lo, hi, x, y)       \
    if (y != 0)                 \
        lo = x / y, hi = x % y; \
    else                        \
        lo = -1, hi = 0 // division by zero, no exceptions thrown

    case CPU_FUNCTION_DIV: // Divide (signed)
        Div(cpu->slo, cpu->shi, SR[rs], SR[rt]);
        break;

    case CPU_FUNCTION_DIVU: // Divide (unsigned)
        Div(cpu->lo, cpu->hi, R[rs], R[rt]);
        break;
#undef Div

    case CPU_FUNCTION_MULT: // Multiply (signed)
        cpu->slo = SR[rs] * SR[rt];
        break;

    case CPU_FUNCTION_MULTU: // Multiply (unsigned)
        cpu->lo = R[rs] * R[rt];
        break;

    case CPU_FUNCTION_ADD: // Add (signed)
        SR[rd] = cpu_execute_signed_add(cpu, SR[rd], SR[rs], SR[rt]);
        break;

    case CPU_FUNCTION_ADDU: // Add (unsigned)
        R[rd] = R[rs] + R[rt];
        break;

    case CPU_FUNCTION_SUB: // Sub (signed)
        SR[rd] = cpu_execute_signed_add(cpu, SR[rd], SR[rs], -SR[rt]);
        break;

    case CPU_FUNCTION_SUBU: // Sub (unsigned)
        R[rd] = R[rs] - R[rt];
        break;

    case CPU_FUNCTION_AND: // Bitwise and
        R[rd] = R[rs] & R[rt];
        break;

    case CPU_FUNCTION_OR: // Bitwise or
        R[rd] = R[rs] | R[rt];
        break;

    case CPU_FUNCTION_XOR: // Bitwise xor
        R[rd] = R[rs] ^ R[rt];
        break;

    case CPU_FUNCTION_NOR: // Bitwise nor
        R[rd] = ~(R[rs] | R[rt]);
        break;

    case CPU_FUNCTION_SLT: // Set on less than (signed)
        SR[rd] = SR[rs] < SR[rt];
        break;

    case CPU_FUNCTION_SLTU: // Set on less than (unsigned)
        R[rd] = R[rs] < R[rt];
        break;

    default:
        cpu_throw_exception(cpu, CPU_EXCEPTION_ILLEGAL_INSTRUCTION);
        break;
    }
}

void cpu_execute_immediate_instruction(Cpu *cpu, Cpu_Instruction_Immediate instruction)
{
    i32 sx = instruction.sx;
    u32 x = instruction.x;
    u8 rt = instruction.rt;
    u8 rs = instruction.rs;
    i64 address = R[rs] + sx;
    bool address_overflow = address > WORD_MAX || address < 0;

    switch (instruction.opcode) {
    case CPU_OPCODE_SLTI: // Set on less than (signed)
        SR[rt] = SR[rs] < sx;
        break;

    case CPU_OPCODE_SLTIU: // Set on less than (unsigned)
        R[rt] = R[rs] < x;
        break;

    case CPU_OPCODE_ANDI: // Bitwise and
        R[rt] = R[rs] & x;
        break;

    case CPU_OPCODE_ORI: // Bitwise or
        R[rt] = R[rs] | x;
        break;

    case CPU_OPCODE_LUI: // Load upper
        R[rt] = x << 16;
        break;

    case CPU_OPCODE_BEQ: // Branch if equals
        cpu_branch_when(cpu, false, x, R[rs] == R[rt]);
        break;

    case CPU_OPCODE_BNE: // Branch if not equals
        cpu_branch_when(cpu, false, x, R[rs] != R[rt]);
        break;

    case CPU_OPCODE_BLEZ: // Branch if less or equals zero
        cpu_branch_when(cpu, instruction.rt & 1, x, R[rs] <= 0);
        break;

    case CPU_OPCODE_BGTZ: // Branch if greater or equals zero
        cpu_branch_when(cpu, instruction.rt & 1, x, R[rs] >= 0);
        break;

    case CPU_OPCODE_ADDI: // Add (signed)
        SR[rt] = cpu_execute_signed_add(cpu, SR[rt], SR[rs], sx);
        break;

    case CPU_OPCODE_ADDIU: // Add (unsigned)
        R[rt] = R[rs] + x;
        break;

    case CPU_OPCODE_LB: { // Load byte extend)
        if (address_overflow) {
            return cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        }
        u8 byte = bus_peek_byte(cpu->bus, address);
        R[rt] = sign_extend_byte_to_word(byte);
    } break;

    case CPU_OPCODE_LH: { // Load hword (sign extend)
        if ((address & 1) || address_overflow) {
            return cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        }
        u16 hword = bus_peek_hword(cpu->bus, address);
        R[rt] = sign_extend_hword_to_word(hword);
    } break;

    case CPU_OPCODE_LBU: // Load byte (unsigned)
        if (address_overflow)
            return cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        R[rt] = bus_peek_byte(cpu->bus, address);
        break;

    case CPU_OPCODE_LHU: // Load hword (unsigned)
        if ((address & 1) || address_overflow)
            return cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        R[rt] = bus_peek_hword(cpu->bus, address);
        break;

    case CPU_OPCODE_LW: // Load word
        if ((address & 3) || address_overflow)
            return cpu_throw_exception(cpu, CPU_EXCEPTION_LOAD_ADDRESS);
        R[rt] = bus_peek_byte(cpu->bus, address);
        break;

    case CPU_OPCODE_SB: // Store byte
        if (address_overflow)
            return cpu_throw_exception(cpu, CPU_EXCEPTION_STORE_ADDRESS);
        bus_poke_byte(cpu->bus, R[rs] + x, R[rt]);
        break;

    case CPU_OPCODE_SH: // Store hword
        if ((address & 2) || address_overflow)
            return cpu_throw_exception(cpu, CPU_EXCEPTION_STORE_ADDRESS);
        bus_poke_hword(cpu->bus, R[rs] + x, R[rt]);
        break;

    case CPU_OPCODE_SW: // Store word
        if ((address & 3) || address_overflow)
            return cpu_throw_exception(cpu, CPU_EXCEPTION_STORE_ADDRESS);
        bus_poke_word(cpu->bus, R[rs] + x, R[rt]);
        break;

    default:
        cpu_throw_exception(cpu, CPU_EXCEPTION_ILLEGAL_INSTRUCTION);
        break;
    }
}

void cpu_execute_jump_instruction(Cpu *cpu, Cpu_Instruction_Jump instruction)
{
    u32 address = (cpu->pc & (0xf0000000)) | (instruction.pseudo_address << 2);

    switch (instruction.opcode) {
    case CPU_OPCODE_J: // Jump
        cpu->next_pc = address;
        break;

    case CPU_OPCODE_JAL: // Jump and link
        cpu->ra = cpu->pc + 4;
        cpu->next_pc = address;
        break;

    default:
        break;
    }
}

void cpu_branch_when(Cpu *cpu, bool instruction_link, u32 imm_address, bool condition)
{
    // note! reduce branch misprediction (maybe benchmark)
    u32 branches_pc[2] = {
        cpu->pc,
        cpu->pc + 4 + sign_extend_imm_address(imm_address),
    };
    u32 branches_ra[2] = {
        cpu->ra,
        cpu->pc + 4,
    };

    cpu->ra = branches_ra[instruction_link & condition];
    cpu->next_pc = branches_pc[condition];
}

i32 cpu_execute_signed_add(Cpu *cpu, i32 r, i32 a, i32 b)
{
    i32 sum;
    if (!add_overflow_s32(&sum, a, b)) {
        cpu_throw_exception(cpu, CPU_EXCEPTION_OVERFLOW);
        return r;
    }
    return sum;
}

void cpu_throw_exception(Cpu *cpu, Cpu_Exception_Type cause)
{
    cpu->next_pc = syscop_throw_exception(&cpu->syscop, cpu->pc, cause);
}

// todo! cpu_print_instruction()
void *cpu_print_instruction(Cpu *cpu, Cpu_Instruction instruction, void *f)
{
    fprintf(f, "%.4x\n", instruction.as_word);
    return NULL;

    // format:
    // 0x0000: sll    r0, r0        | NOP
    // word: mnemonic operands        info

    /* switch (instruction.type) { */
    /* case CPU_INSTRUCTION_TYPE_NONE: */
    /*     fprintf(f, "%s unknown", info_padding); */
    /*     break; */

    /* case CPU_INSTRUCTION_COPROCESSOR: */
    /*     fprintf(f, "%s coprocessor", info_padding); */
    /*     break; */

    /* case CPU_INSTRUCTION_REGISTER: { */
    /*     Cpu_Instruction_Register ins = instruction.as_register; */

    /* } break; */

    /* case CPU_INSTRUCTION_IMMEDIATE: */
    /* case CPU_INSTRUCTION_JUMP: */
    /*     break; */
    /* } */
}
