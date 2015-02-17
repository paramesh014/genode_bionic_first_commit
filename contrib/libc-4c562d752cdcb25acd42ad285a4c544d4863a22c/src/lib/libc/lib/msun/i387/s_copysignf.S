/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <machine/asm.h>

__FBSDID("$FreeBSD: release/8.2.0/lib/msun/i387/s_copysignf.S 192760 2009-05-25 14:37:10Z attilio $");
/* RCSID("$NetBSD: s_copysignf.S,v 1.3 1995/05/08 23:53:25 jtc Exp $") */

ENTRY(copysignf)
	movl	8(%esp),%edx
	andl	$0x80000000,%edx
	movl	4(%esp),%eax
	andl	$0x7fffffff,%eax
	orl	%edx,%eax
	movl	%eax,4(%esp)
	flds	4(%esp)
	ret
END(copysignf)
