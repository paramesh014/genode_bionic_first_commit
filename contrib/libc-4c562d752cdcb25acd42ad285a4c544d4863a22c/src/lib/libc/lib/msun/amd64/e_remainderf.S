/*
 * Based on the i387 version written by J.T. Conklin <jtc@netbsd.org>.
 * Public domain.
 */

#include <machine/asm.h>

RCSID("from: $NetBSD: e_remainderf.S,v 1.2 1995/05/08 23:49:47 jtc Exp $")
__FBSDID("$FreeBSD: release/8.2.0/lib/msun/amd64/e_remainderf.S 176227 2008-02-13 06:01:48Z bde $")

ENTRY(remainderf)
	movss	%xmm0,-4(%rsp)
	movss	%xmm1,-8(%rsp)
	flds	-8(%rsp)
	flds	-4(%rsp)
1:	fprem1
	fstsw	%ax
	testw	$0x400,%ax
	jne	1b
	fstps	-4(%rsp)
	movss	-4(%rsp),%xmm0
	fstp	%st
	ret
