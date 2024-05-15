#include "bus.h"
#include "report.h"
#include <stdarg.h>
#include <stdio.h>

void report_trace(const char *s, ...)
{
    fprintf(stdout, "[trace] ");
    va_list args;
    va_start(args, s);
    vfprintf(stdout, s, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void report_warn(const char *s, ...)
{
    fprintf(stderr, "[warning] ");
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void report_error(const char *s, ...)
{
    fprintf(stderr, "[error] ");
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    va_end(args);
    fprintf(stderr, "\n");
}

int main(int argc, char *argv[])
{
    report_init(report_trace, report_warn, report_error);

    Bus bus = {0};
    bus_init(&bus);

    printf("hello emustation !\n");
    deinit_report();
}
