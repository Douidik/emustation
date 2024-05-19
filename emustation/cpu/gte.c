#include "gte.h"
#include <string.h>

void gte_init(Gte *gte)
{
    memset(gte, 0, sizeof(Gte));
}

void gte_poke(Gte *gte, u8 rd, u32 word) {}

void gte_poke_control(Gte *gte, u8 rd, u32 word) {}

u32 gte_peek(Gte *gte, u8 rd) {}

u32 gte_peek_control(Gte *gte, u8 rd) {}

void gte_execute_command(Gte *gte, Gte_Command command) {}
