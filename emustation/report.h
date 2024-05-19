#ifndef EMUSTATION_REPORT_H
#define EMUSTATION_REPORT_H

#include "common.h"

typedef void (*_Reporter)(const char *s, ...);
#define Reporter _Reporter Printf_Like(1, 2)

typedef struct Report
{
    Reporter trace;
    Reporter warn;
    Reporter error;
} Report;

extern Report *report;

void report_init(Reporter trace, Reporter warn, Reporter error);
void report_deinit();

#endif
