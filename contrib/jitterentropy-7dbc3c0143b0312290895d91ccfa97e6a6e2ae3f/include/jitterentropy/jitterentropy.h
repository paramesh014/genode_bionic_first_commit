﻿/*
 * Non-physical true random number generator based on timing jitter.
 *
 * Copyright Stephan Mueller <smueller@chronox.de>, 2014
 *
 * License
 * =======
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU General Public License, in which case the provisions of the GPL are
 * required INSTEAD OF the above restrictions.  (This clause is
 * necessary due to a potential bad interaction between the GPL and
 * the restrictions contained in a BSD-style copyright.)
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ALL OF
 * WHICH ARE HEREBY DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF NOT ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _JITTERENTROPY_H
#define _JITTERENTROPY_H

#ifdef JITTERENTROPY_GENODE
#include <jitterentropy-base-genode.h>
#else
#ifdef __KERNEL__
#include "jitterentropy-base-kernel.h"
#else
#include "jitterentropy-base-user.h"
#endif /* __KERNEL__ */
#endif

/* Statistical data from the entropy source */
struct entropy_stat {
	unsigned int bitslot[64];	/* Counter for the bits set per bit
					   position in ->data */
	unsigned int bitvar[64];	/* Counter for the number of bit
					   variations per bit position in
					   ->data */
	unsigned int enable_bit_test;	/* enable bit test
       					   this flag is vital for the accuracy
					   of the statistic tests: when we
					   do the time measurements, we want
					   the observed entropy collection
					   loop executed as fast as the
					   unmeasured loop, i.e. without
					   the bit statistic logic; on the
					   other hand, the bit statistics
					   test is not interested in exact
					   timing */
	__u64 collection_begin;		/* timer for beginning of one
					   entropy collection round */
	__u64 collection_end;		/* timer for end of one round */
	__u64 old_delta;		/* Time delta of previous round to
					   calculate delta of deltas */
	unsigned int setbits;		/* see _jent_calc_statistic */
	unsigned int varbits;		/* see _jent_calc_statistic */
	unsigned int obsbits;		/* see _jent_calc_statistic */
	unsigned int collection_loop_cnt;	/* Collection loop counter */
};

/* The entropy pool */
struct rand_data
{
	/* all data values that are vital to maintain the security
	 * of the RNG are marked as SENSITIVE. A user must not
	 * access that information while the RNG executes its loops to
	 * calculate the next random value. */
	__u64 data;		/* SENSITIVE Actual random number */
	__u64 prev_time;	/* SENSITIVE Previous time stamp */
#define DATA_SIZE_BITS ((sizeof(__u64)) * 8)
	__u64 old_data;		/* SENSITIVE FIPS continuous test */
	unsigned int osr;	/* Oversample rate */
	unsigned int fips_fail:1;	/* FIPS status */
	unsigned int stir:1;		/* Post-processing stirring */
	unsigned int disable_unbias:1;	/* Deactivate Von-Neuman unbias */
#define JENT_MEMORY_BLOCKS 64
#define JENT_MEMORY_BLOCKSIZE 32
#define JENT_MEMORY_ACCESSLOOPS 128
#define JENT_MEMORY_SIZE (JENT_MEMORY_BLOCKS*JENT_MEMORY_BLOCKSIZE)
	unsigned char *mem;	/* Memory access location with size of
				 * memblocks * memblocksize */
	unsigned int memlocation; /* Pointer to byte in *mem */
	unsigned int memblocks;	/* Number of memory blocks in *mem */
	unsigned int memblocksize; /* Size of one memory block in bytes */
	unsigned int memaccessloops; /* Number of memory accesses per random
				      * bit generation */
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT
	struct entropy_stat entropy_stat;
#endif
};

/* Flags that can be used to initialize the RNG */
#define JENT_DISABLE_STIR (1<<0) /* Disable stirring the entropy pool */
#define JENT_DISABLE_UNBIAS (1<<1) /* Disable Von Neuman unbias */
#define JENT_DISABLE_MEMORY_ACCESS (1<<2) /* Disable memory access for more
					     entropy, saves MEMORY_SIZE RAM for
					     entropy collector */

/* Number of low bits of the time value that we want to consider */
#define TIME_ENTROPY_BITS 1

#define DRIVER_NAME     "jitterentropy"

/* -- BEGIN Main interface functions -- */

/* Number of low bits of the time value that we want to consider */
/* get raw entropy */
int jent_read_entropy(struct rand_data *entropy_collector,
		      char *data, size_t len);
/* initialize an instance of the entropy collector */
struct rand_data *jent_entropy_collector_alloc(unsigned int osr,
	       				       unsigned int flags);
/* clearing of entropy collector */
void jent_entropy_collector_free(struct rand_data *entropy_collector);

/* initialization of entropy collector */
int jent_entropy_init(void);

/* -- END of Main interface functions -- */

/* -- BEGIN error codes for init function -- */
#define ENOTIME  	1 /* Timer service not available */
#define ECOARSETIME	2 /* Timer too coarse for RNG */
#define ENOMONOTONIC	3 /* Timer is not monotonic increasing */
#define EMINVARIATION	4 /* Timer variations too small for RNG */
#define EVARVAR		5 /* Timer does not produce variations of variations
			     (2nd derivation of time is zero) */
#define EMINVARVAR	6 /* Timer variations of variations is too small */
#define EPROGERR	7 /* Programming error */

/* -- BEGIN statistical test functions only complied with CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT -- */

void _jent_init_statistic(struct rand_data *entropy_collector);
void _jent_calc_statistic(struct rand_data *entropy_collector,
			  struct entropy_stat *stat, unsigned int loop_cnt);
void _jent_bit_count(struct rand_data *entropy_collector, __u64 prev_data);

#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT
#define jent_init_statistic(x)    do { _jent_init_statistic(x); }    while (0)
#define jent_calc_statistic(x, y, z) do { _jent_calc_statistic(x, y, z); } while (0)
#define jent_bit_count(x,y)       do { _jent_bit_count(x, y); }      while (0)
void jent_gen_entropy_stat(struct rand_data *entropy_collector,
			   struct entropy_stat *stat);
void jent_fold_time_stat(struct rand_data *ec, __u64 *fold, __u64 *loop_cnt);
__u64 jent_fold_var_stat(struct rand_data *ec, unsigned int min);
__u64 jent_oscillation_var_stat(void);
#else /* CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT */
#define jent_init_statistic(x)    do { if(0) _jent_init_statistic(x); }    while (0)
#define jent_calc_statistic(x, y, z) do { if(0) _jent_calc_statistic(x, y, z); } while (0)
#define jent_bit_count(x,y)       do { if(0) _jent_bit_count(x,y); }       while (0)
#endif /* CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT */

/* -- END of statistical test function -- */

#endif /* _JITTERENTROPY_H */
