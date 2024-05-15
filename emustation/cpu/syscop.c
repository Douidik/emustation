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
    if (rd != 12) { // note! i don't know if other cop0 registers can be poked
        report->error("syscop cannot poke register #%hhu", rd);
    }

    syscop->regs[rd] = word;
}

u32 syscop_peek(Syscop *syscop, u8 rd)
{
    if (rd != 12) { // note! i don't know if other cop0 registers can be peeked
        report->error("syscop cannot peek register #%hhu", rd);
    }

    return syscop->regs[rd];
}

void syscop_rfe(Syscop *syscop)
{
    syscop->status.ku_stack >>= 1;
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
    syscop->status.ku_stack <<= 1;
    return exception_vectors[syscop->status.bev];
}
