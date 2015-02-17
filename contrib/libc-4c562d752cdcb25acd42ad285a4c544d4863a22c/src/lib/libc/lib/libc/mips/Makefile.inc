#	$NetBSD: Makefile.inc,v 1.7 2005/09/17 11:49:39 tsutsui Exp $
# $FreeBSD: release/8.2.0/lib/libc/mips/Makefile.inc 215938 2010-11-27 12:26:40Z jchandra $

CFLAGS+=-DSOFTFLOAT

MDSRCS+= machdep_ldisd.c
SYM_MAPS+= ${.CURDIR}/mips/Symbol.map
