/*
 * Non-physical true random number generator based on timing jitter.
 *
 * Copyright Stephan Mueller <smueller@chronox.de>, 2013
 *
 * OpenSSL Engine support
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

#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/rand.h>

#ifdef OPENSSL_FIPS
#include <openssl/fips.h>
#include <openssl/fips_rand.h>
#endif

#include "jitterentropy.h"
#include "jitterentropy-openssl-common.h"

static const RAND_METHOD *DRNG_meth = NULL;

void jent_seed(const void *buf, int num)
{
	DRNG_meth->seed(buf, num);	
}

void jent_add(const void *buf, int num, double add)
{
	if(DRNG_meth->add)
		DRNG_meth->add(buf, num, add);
}

int
jent_release(struct jent_drng *drng)
{
	if(drng->entropy_collector)
	{
		CRYPTO_w_lock(CRYPTO_LOCK_RAND);
		jent_entropy_collector_free(drng->entropy_collector);
		CRYPTO_w_unlock(CRYPTO_LOCK_RAND);
		drng = NULL;
	}
	return 1;
}

int
jent_rand_status(struct jent_drng *drng)
{
	if(drng->entropy_collector)
		return 1;
	return 0;
}

/* seed or reseed the DRNG -- no locks necessary */
int
jent_update(struct jent_drng *drng, int type)
{
	char seed[100];
	size_t seedsize = 0;
	int rc = 0;

	if(DRNG_RESET == type)
		seedsize = 48; /* hardcoded into RAND_poll */
	else if(DRNG_RESEED == type)
		seedsize = 16; /* there is no API to get that value but we know
				  that the ANSI X9.31 V value is 16 bytes */
	if(sizeof(seed) < seedsize || 0 > seedsize)
		return -1;
	memset(&seed, 0, sizeof(seed));
	CRYPTO_w_lock(CRYPTO_LOCK_RAND);
	rc = jent_read_entropy(drng->entropy_collector, seed, seedsize);
	CRYPTO_w_unlock(CRYPTO_LOCK_RAND);
	if(0 > rc)
		return rc;

	if(DRNG_RESET == type && DRNG_meth->cleanup)
		DRNG_meth->cleanup();
	jent_seed(seed, seedsize);
	return 0;
}

/*
 * Check for reseed requirement
 *
 * return: the function returns the maximum bytes the caller is allowed to
 *	   pull from the RNG before another call to this function
 *	   is to be made
 */
size_t
jent_reseed_rekey(struct jent_drng *drng, const size_t lim_reseed,
		  const size_t lim_rekey, size_t bytes)
{
        size_t max_bytes = 0;

        if(drng->norekey_data >= lim_rekey)
        {
		if(jent_update(drng, DRNG_RESET))
			return 0;
		drng->norekey_data = 0;
		drng->noreseed_data = 0;
	}
	/* if we did a full reset, a reseed is not needed */
	else if(drng->noreseed_data >= lim_reseed)
	{
		if(jent_update(drng, DRNG_RESEED))
			return 0;
		drng->noreseed_data = 0;
	}

        /* calculate the bytes to be obtained in the next round of the RNG
	 * to not overstep the limit boundaries */
        max_bytes = min((lim_reseed - drng->noreseed_data),
			(lim_rekey - drng->norekey_data));
        /* of course, the maximum bytes cannot be higher than the bytes
	 *          * requested */
        max_bytes = min(max_bytes, bytes);

        return max_bytes;
}

int
jent_rand_bytes(unsigned char *data, int num, struct jent_drng *drng,
	        const size_t lim_reseed, const size_t lim_rekey)
{
	size_t pull = 0;
	unsigned char *p = data;
	if(!drng->entropy_collector)
		return 0;
	if(0 >= num)
		return 0;

	while(0 < num)
	{
		pull = jent_reseed_rekey(drng, lim_reseed, lim_rekey, num);
		DRNG_meth->bytes(p, pull);
		num -= pull;
		p += pull;
		drng->noreseed_data += pull;
		drng->norekey_data += pull;
	}
	return 1;
}

int
jent_init(struct jent_drng *drng)
{
#ifdef OPENSSL_FIPS
	DRNG_meth = FIPS_mode() ? FIPS_rand_method() : RAND_SSLeay();
	if (FIPS_mode() && DRNG_meth != FIPS_rand_check())
	{
		RANDerr(RAND_F_RAND_GET_RAND_METHOD,RAND_R_NON_FIPS_METHOD);
		return 0;
	}
#else
	DRNG_meth = RAND_SSLeay();
#endif

	if (!DRNG_meth || !DRNG_meth->bytes || !DRNG_meth->seed)
		return 0;

	if(!drng->entropy_collector)
	{
		CRYPTO_w_lock(CRYPTO_LOCK_RAND);
		drng->entropy_collector = jent_entropy_collector_alloc(1, 0);
		CRYPTO_w_unlock(CRYPTO_LOCK_RAND);
		if(!drng->entropy_collector)
			return 0;
	}
	jent_update(drng, DRNG_RESET);
	drng->noreseed_data = 0;
	drng->norekey_data = 0;
	return 1;
}

