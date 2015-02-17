/*
 * Written by Alexander Kabaev <kan@FreeBSD.org>
 * The file is in public domain.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD: release/8.2.0/lib/libc/sys/stack_protector_compat.c 212290 2010-09-07 12:06:41Z kib $");

void __stack_chk_fail(void);

#ifdef PIC
void
__stack_chk_fail_local_hidden(void)
{

	__stack_chk_fail();
}

__sym_compat(__stack_chk_fail_local, __stack_chk_fail_local_hidden, FBSD_1.0);
#endif
