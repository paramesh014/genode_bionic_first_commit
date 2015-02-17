/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <machine/asm.h>

__FBSDID("$FreeBSD: release/8.2.0/lib/msun/i387/s_logbf.S 192760 2009-05-25 14:37:10Z attilio $");
/* RCSID("$NetBSD: s_logbf.S,v 1.3 1995/05/09 00:15:12 jtc Exp $") */

ENTRY(logbf)
	flds	4(%esp)
	fxtract
	fstp	%st
	ret
END(logbf)
