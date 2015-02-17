#	@(#)Makefile.inc	8.1 (Berkeley) 6/4/93
# $FreeBSD: release/8.2.0/lib/libc/i386/string/Makefile.inc 214086 2010-10-19 22:11:50Z jilles $

MDSRCS+=bcmp.S bcopy.S bzero.S ffs.S index.S memchr.S memcmp.S memcpy.S \
	memmove.S memset.S rindex.S strcat.S strchr.S strcmp.S strcpy.S \
	strncmp.S strrchr.S swab.S wcschr.S wcscmp.S wcslen.S \
	wmemchr.S
