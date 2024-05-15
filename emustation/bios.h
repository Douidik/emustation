#ifndef EMUSTATION_BIOS_H
#define EMUSTATION_BIOS_H

#include "common.h"
#include "spec.h"
#include <stdio.h>

typedef u8 Bios_Rom[BIOS_SIZE];

typedef struct Bios
{
    Bios_Rom rom;
} Bios;

void bios_init(Bios *bios, FILE *f);

#endif
