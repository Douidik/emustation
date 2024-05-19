#ifndef EMUSTATION_GTE_H
#define EMUSTATION_GTE_H

#include "common.h"
#include "instruction.h"

typedef union Gte_Error_Flag {
    struct
    {
        u16 : 12;
        u8 ir0_saturated : 1;
        u8 sy2_saturated : 1;
        u8 sx2_saturated : 1;
        u8 mac0_overflow_positive : 1;
        u8 mac0_overflow_negative : 1;
        u8 divide_overflow : 1;
        u8 sz3_or_otz_saturated : 1;
        u8 color_b_queue_saturated : 1;
        u8 color_g_queue_saturated : 1;
        u8 color_r_queue_saturated : 1;
        u8 ir3_satured : 1;
        u8 ir2_satured : 1;
        u8 ir1_satured : 1;
        u8 mac3_overflow_negative : 1;
        u8 mac2_overflow_negative : 1;
        u8 mac1_overflow_negative : 1;
        u8 mac3_overflow_positive : 1;
        u8 mac2_overflow_positive : 1;
        u8 mac1_overflow_positive : 1;
        // [30..23] | [18..13]
        // indicates if any error occured, updates when the flag is accessed
        u8 any_error_occured : 1;
    };
    u32 as_word;
} Gte_Error_Flag;

typedef struct Gte
{
    // data registers //
    // vectors
    i16 v0[3];
    i16 v1[3];
    i16 v2[3];

    // color value
    u8 rgbc;
    // average z value
    u16 otz;

    // interoplate accumulator
    i16 ir0;
    // interpolate vector
    i16 ir123[3];

    // screen x-y coordinate queue
    i16 sc0[2];
    i16 sc1[2];
    i16 sc2[2];
    i16 scp[2];

    // screen z coordinate queue
    i16 sz0[2];
    i16 sz1[2];
    i16 sz2[2];
    i16 sz3[2];

    // unused
    u8 res1[4];

    // scalar math accumulator
    i32 mac0;
    // vector math accumulator
    i32 mac123[3];

    // convert rgb color (?)
    i16 rgb;

    // count leading zeroes, ones (?)
    i32 lzcs, lzcr;

    // control registers //
    // rotation matrix
    i16 rt[9];
    // translation vector
    u32 tr[3];
    // light source matrix
    i16 l[9];

    // background color
    u32 bk[3];
    // light color matrix source
    i64 lr[9];
    // far color
    u32 fc;

    // screen offset
    u32 of;
    // projection plane distance
    u16 h;

    // depth queing parameter a, coefficient
    i16 dqa;
    // depth queing parameter b, offset
    u32 dqb;

    // average z scale factors
    i16 zsf[2];

    // calculation error flag
    u32 flag;
} Gte;

void gte_init(Gte *gte);

void gte_swc(Gte *gte, u8 rs, u8 rt, u16 hword);
void gte_lwc(Gte *gte, u8 rs, u8 rt, u16 hword);
void gte_poke(Gte *gte, u8 rd, u32 word);
void gte_poke_control(Gte *gte, u8 rd, u32 word);
u32 gte_peek(Gte *gte, u8 rd);
u32 gte_peek_control(Gte *gte, u8 rd);

void gte_execute_command(Gte *gte, Gte_Command command);

#endif
