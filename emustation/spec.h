#ifndef EMUSTATION_SPEC_H
#define EMUSTATION_SPEC_H

#include <stdint.h>

#define Mb(x) (x * 1000000)
#define Kb(x) (x * 1000)

#define BIOS_SIZE Kb(512)
#define RAM_SIZE Mb(2)
#define CPU_INSTRUCTION_CACHE_SIZE Kb(4)
#define CPU_DATA_CACHE_SIZE Kb(1)

#define WORD_MAX (UINT32_MAX)
#define HWORD_MAX (UINT16_MAX)
#define BYTE_MAX (UINT8_MAX)
#define SWORD_MIN (INT32_MIN)
#define SHWORD_MIN (INT16_MIN)
#define SBYTE_MIN (INT8_MIN)
#define SWORD_MAX (INT32_MAX)
#define SHWORD_MAX (INT16_MAX)
#define SBYTE_MAX (INT8_MAX)

#endif
