/* Generated by gensyscalls.py. Do not edit. */

#include <private/bionic_asm.h>

ENTRY(sched_yield)
    mov     x8, __NR_sched_yield
    svc     #0

    cmn     x0, #(MAX_ERRNO + 1)
    cneg    x0, x0, hi
    b.hi    __set_errno

    ret
END(sched_yield)
