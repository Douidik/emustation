#ifndef EMUSTATION_BUS_H
#define EMUSTATION_BUS_H

#include "cpu/cpu.h"
#include "spec.h"

typedef u8 Ram[RAM_SIZE];

typedef struct Bus
{
    Cpu cpu;
    Ram ram;
} Bus;

void bus_init(Bus *bus);

u8 bus_peek_byte(Bus *bus, u32 address);
u16 bus_peek_hword(Bus *bus, u32 address);
u32 bus_peek_word(Bus *bus, u32 address);
void bus_poke_byte(Bus *bus, u32 address, u8 byte);
void bus_poke_hword(Bus *bus, u32 address, u16 hword);
void bus_poke_word(Bus *bus, u32 address, u32 word);

#endif
