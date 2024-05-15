#ifndef EMUSTATION_GTE_H
#define EMUSTATION_GTE_H

#include "HandmadeMath.h"
#include "common.h"

typedef struct Gte
{
    union {
        u32 regs[64];

        struct
        {
        };
    };

    HMM_Mat3 rtn;
    HMM_Vec3 tln;
    HMM_Mat3 ls;
    i32 rbk;
    i32 gbk;
    i32 bbk;
    HMM_Mat3 ls_color;
    i32 fc[3];
    i32 ofx;
    i32 ofy;
    u16 h;
    i16 dqa;
    i32 dqb;
    i16 zsf3;
    i16 zsf4;
} Gte;

void gte_init(Gte *gte);
void gte_execute_instruction(Gte *gte, u32 word);
u32 gte_poke(Gte *gte, u8 rd, u32 word);
u32 gte_peek(Gte *gte, u8 rd);

#endif
