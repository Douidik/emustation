#include "syscop.h"
#include "report.h"
#include <string.h>

const u32 exception_vectors[2] = {
    0x80000080, // from rom
    0xbfc00180, // from ram
};

void syscop_init(Syscop *syscop)
{
    memset(syscop, 0, sizeof(Syscop));
}

void syscop_poke(Syscop *syscop, u8 rd, u32 word)
{
    switch (rd) {
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
        if (word != 0) {
            report->error("syscop cannot poke 0 to breakpoint register #%hhu", rd);
        }
        syscop->regs[rd] = word;
        break;

    case 12:
        syscop->regs[rd] = word;
        break;

    default:
        report->error("syscop cannot poke register #%hhu", rd);
        return;
    }
}

u32 syscop_peek(Syscop *syscop, u8 rd)
{
    switch (rd) {
    case 3:
    case 5:
    case 6:
    case 7:
    case 9:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        return syscop->regs[rd];

    default:
        report->error("syscop cannot peek register #%hhu", rd);
        return 0xdeadbeef;
    }
}

void syscop_rfe(Syscop *syscop)
{
    // pop the kerner user mode stack
    syscop->status.ku_stack >>= 2;
}

u32 syscop_throw_exception(Syscop *syscop, u32 pc, Cpu_Exception_Type type)
{
    // when throwing an exception:
    //  - set the error source pc (epc)
    //  - set the cause of the exception
    //  - disable interrupts (status.iec)
    //  - push kernel user mode stack (status.ku_stack)

    syscop->epc = pc;
    syscop->cause.type = type;
    syscop->status.iec = 0;
    syscop->status.ku_stack <<= 2;
    return exception_vectors[syscop->status.bev];
}
