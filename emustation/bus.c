#include "bus.h"
#include <string.h>

void bus_init(Bus *bus)
{
    cpu_init(&bus->cpu, bus);
    memset(bus->ram, 0, RAM_SIZE);
}

u8 bus_peek_byte(Bus *bus, u32 address)
{
    return bus->ram[address];
}

u16 bus_peek_hword(Bus *bus, u32 address)
{
    assert(!(address & 1));
    return Byteswap16(*(u16 *)&bus->ram[address]);
}

u32 bus_peek_word(Bus *bus, u32 address)
{
    assert(!(address & 2));
    return Byteswap32(*(u32 *)&bus->ram[address]);
}

void bus_poke_byte(Bus *bus, u32 address, u8 byte)
{
    bus->ram[address] = byte;
}

void bus_poke_hword(Bus *bus, u32 address, u16 hword)
{
    assert(!(address & 1));
    *(u16 *)&bus->ram[address] = Byteswap16(hword);
}

void bus_poke_word(Bus *bus, u32 address, u32 word)
{
    assert(!(address & 3));
    *(u32 *)&bus->ram[address] = Byteswap32(word);
}
