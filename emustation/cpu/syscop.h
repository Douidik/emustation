#ifndef EMUSTATION_SYSCOP_H
#define EMUSTATION_SYSCOP_H

#include "common.h"

typedef enum Cpu_Exception_Type
{
    CPU_EXCEPTION_INTERRUPT = 0x0,
    CPU_EXCEPTION_LOAD_ADDRESS = 0x4,
    CPU_EXCEPTION_STORE_ADDRESS = 0x5,
    CPU_EXCEPTION_SYSCALL = 0x8,
    CPU_EXCEPTION_BREAK = 0x9,
    CPU_EXCEPTION_ILLEGAL_INSTRUCTION = 0xa,
    CPU_EXCEPTION_COPROCESSOR = 0xb,
    CPU_EXCEPTION_OVERFLOW = 0xc,
} Cpu_Exception_Type;

typedef struct Cpu_Cause
{
    u8 _0 : 2;
    Cpu_Exception_Type type : 4;
    u8 _1 : 1;
    u8 ip : 7;
    u16 _3 : 11;
    u8 ce : 2;
    u8 _4 : 1;
    u8 bd : 1;
} Cpu_Cause;

typedef struct Cpu_Status
{
    union {
        struct
        {
            u8 iec : 1;
            u8 kuc : 1;
            u8 iep : 1;
            u8 kup : 1;
            u8 ieo : 1;
            u8 kuo : 1;
        };
        u8 ku_stack : 6; // kernel user mode as a 6 deep bit stack
    };

    u8 _1 : 2;
    u8 im : 8;
    u8 isc : 1;
    u8 swc : 1;
    u8 pz : 1;
    u8 cm : 1;
    u8 pe : 1;
    u8 ts : 1;
    u8 bev : 1;
    u8 _2 : 2;
    u8 re : 1;
    u8 _3 : 2;
    u8 cu0 : 1;
    u8 cu1 : 1;
    u8 cu2 : 1;
    u8 cu3 : 1;
} Cpu_Status;

// cop0: the system coprocessor
// handles interruptions and exceptions
typedef struct Syscop
{
    union {
        u32 regs[64];
        struct
        {
            u32 r0, r1, r2;
            u32 bpc;
            u32 r4;
            u32 bda;
            u32 jump_dest;
            u32 dcid;
            u32 bad_vaddr;
            u32 bdam;
            u32 r10;
            u32 bpcm;
            Cpu_Status status;
            Cpu_Cause cause;
            u32 epc;
            u32 cpid;
            // ...
        };
    };
} Syscop;

void syscop_init(Syscop *syscop);
void syscop_poke(Syscop *syscop, u8 rd, u32 word);
u32 syscop_peek(Syscop *syscop, u8 rd);
void syscop_rfe(Syscop *syscop);
u32 syscop_throw_exception(Syscop *syscop, u32 pc, Cpu_Exception_Type type);

#endif
