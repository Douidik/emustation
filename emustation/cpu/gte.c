#include "gte.h"
#include <string.h>

void gte_init(Gte *gte)
{
    memset(gte, 0, sizeof(Gte));
}

void gte_execute_instruction(Gte *gte, u32 word)
{
    
}

u32 gte_poke(Gte *gte, u8 rd, u32 word) {}

u32 gte_peek(Gte *gte, u8 rd) {}
