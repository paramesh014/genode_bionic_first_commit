#include <sys/cdefs.h>
__FBSDID("$FreeBSD: release/8.2.0/lib/msun/src/s_lrintf.c 140088 2005-01-11 23:12:55Z das $");

#define type		float
#define	roundit		rintf
#define dtype		long
#define	fn		lrintf

#include "s_lrint.c"
