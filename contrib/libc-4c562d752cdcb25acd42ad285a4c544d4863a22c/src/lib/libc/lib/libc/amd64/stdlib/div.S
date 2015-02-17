/*	$NetBSD: div.S,v 1.1 2001/06/19 00:25:04 fvdl Exp $	*/

/*-
 * Written by Frank van der Linden (fvdl@wasabisystems.com)
 * Public domain.
 */

#include <machine/asm.h>
__FBSDID("$FreeBSD: release/8.2.0/lib/libc/amd64/stdlib/div.S 184547 2008-11-02 01:10:54Z peter $");

ENTRY(div)
	movl	%edi,%eax
	cltd
	idivl	%esi
	salq	$32,%rdx
	orq	%rdx,%rax
	ret
END(div)
