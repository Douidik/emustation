#include "bios.h"
#include "report.h"

void bios_init(Bios *bios, FILE *f)
{
    if (fread(bios->rom, sizeof(u8), BIOS_SIZE, f) != BIOS_SIZE) {
        report->error("cannot read bios file");
        return;
    }
}
