#	$NetBSD: Makefile.inc,v 1.27 2005/10/07 17:16:40 tsutsui Exp $
# $FreeBSD: release/8.2.0/lib/libc/mips/gen/Makefile.inc 215938 2010-11-27 12:26:40Z jchandra $

SRCS+=	infinity.c fabs.c ldexp.c modf.c

# SRCS+=	flt_rounds.c fpgetmask.c fpgetround.c fpgetsticky.c fpsetmask.c \
#	fpsetround.c fpsetsticky.c

SRCS+=	_ctx_start.S _set_tp.c _setjmp.S makecontext.c setjmp.S signalcontext.c sigsetjmp.S
