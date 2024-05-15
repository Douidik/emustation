#include "report.h"
#include <stdlib.h>

Report *report = NULL;

void report_init(Reporter trace, Reporter warn, Reporter error)
{
    report = calloc(1, sizeof(Report));
    report->trace = trace;
    report->warn = warn;
    report->error = error;
}

void report_deinit()
{
    free(report);
}
