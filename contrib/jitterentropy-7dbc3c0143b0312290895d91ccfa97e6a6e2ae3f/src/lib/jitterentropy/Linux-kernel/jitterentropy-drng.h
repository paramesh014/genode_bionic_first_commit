/*
 * Non-physical true random number generator based on timing jitter.
 *
 * Copyright Stephan Mueller <smueller@chronox.de>, 2013
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

#ifndef _JITTERENTROPY_DRNG_KERNEL_H
#define _JITTERENTROPY_DRNG_KERNEL_H

/* Statistical data from the DRNG */
struct jent_stat {
	size_t read_data;	/* total data read from DRNG */
	size_t num_reseed;	/* number of reseeds of DRNG since init */
	size_t num_rekey;	/* number of rekeys of DRNG since init */
};

#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG
#define jent_drng_stat_read_add(rng, val)   rng->jent_stat.read_data += val
#define jent_drng_stat_reseed_add(rng, val) rng->jent_stat.num_reseed += val
#define jent_drng_stat_rekey_add(rng, val)  rng->jent_stat.num_rekey += val
#define jent_drng_stat_read_init(rng)	    rng->jent_stat.read_data = 0
#define jent_drng_stat_reseed_init(rng)	    rng->jent_stat.num_reseed = 0
#define jent_drng_stat_rekey_init(rng)	    rng->jent_stat.num_rekey = 0
#else /* CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG */
#define jent_drng_stat_read_add(rng, val)
#define jent_drng_stat_reseed_add(rng, val)
#define jent_drng_stat_rekey_add(rng, val)
#define jent_drng_stat_read_init(rng)
#define jent_drng_stat_reseed_init(rng)
#define jent_drng_stat_rekey_init(rng)
#endif /* CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG */

/* Entropy collector with DRNG as whitening function frontend */
struct jent_drng
{
	spinlock_t drng_lock;
	struct crypto_rng *drng; /* RNG handle */
	struct rand_data *entropy_collector;	/* entropy collector */
	size_t noreseed_data;   /* extracted data since last reseed */
	size_t norekey_data;    /* extracted data since last re-key */
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG
	struct jent_stat jent_stat;
#endif
};

/* Raw entropy collector */
struct jent_raw
{
	spinlock_t raw_lock;
	struct rand_data *entropy_collector;	/* entropy collector */
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG
	struct jent_stat jent_stat;
#endif
};

/* --- BEGIN interfaces for the entropy collector connected to DRNG --- */
int jent_drng_init_drng(struct jent_drng *drng);
void jent_drng_cleanup_drng(struct jent_drng *drng);
ssize_t jent_drng_get_bytes(struct jent_drng *drng, size_t lim_reseed,
			    size_t lim_rekey, char *data, size_t len);
size_t jent_drng_reseed_rekey(struct jent_drng *drng, size_t lim_reseed,
			      size_t lim_rekey, size_t bytes);

/* Reseed and rekey limits for regular DRNG */
#define MAX_BYTES_RESEED 1<<10  /* max bytes before reseed */
#define MAX_BYTES_REKEY  1<<20  /* max bytes before seed key is updated */

/* Reseed and rekey limits for strong DRNG */
#define MAX_BYTES_STRONG_RESEED 16	/* max bytes before reseed - note,
					   this value is the size of the seed V
					   and not crypto_rng_seedsize() since
					   the function call returns the sum
					   of V, DT, K -- and with a reseed
					   we only update V */
#define MAX_BYTES_STRONG_REKEY  1<<10 /* max bytes before update of seed key */
/* --- END interfaces for the entropy collector connected DRNG --- */

/* --- BEGIN interfaces for the raw entropy collector output --- */
int jent_drng_init_raw(struct jent_raw *raw, unsigned int flags);
void jent_drng_cleanup_raw(struct jent_raw *raw);
int jent_drng_get_bytes_raw(struct jent_raw *raw, char *data, size_t len);
/* --- END interfaces for the raw entropy collector output --- */


#endif /* _JITTERENTROPY_DRNG_KERNEL_H */
