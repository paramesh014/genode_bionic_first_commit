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

#include "jitterentropy.h"

static struct rand_data *jent_raw;
static const char *jent_raw_id = "jitterentropy-raw";
static const char *jent_raw_name = "CPU Jitter random number generator (raw entropy)";

static int
jent_raw_init(ENGINE *e)
{
	if(!jent_raw)
	{
		CRYPTO_w_lock(CRYPTO_LOCK_RAND);
		jent_raw = jent_entropy_collector_alloc(1, 0);
		CRYPTO_w_unlock(CRYPTO_LOCK_RAND);
		if(!jent_raw)
			return 0;
	}
	return 1;
}

static int
jent_raw_free(ENGINE *e)
{
	if(jent_raw)
	{
		CRYPTO_w_lock(CRYPTO_LOCK_RAND);
		jent_entropy_collector_free(jent_raw);
		CRYPTO_w_unlock(CRYPTO_LOCK_RAND);
		jent_raw = NULL;
	}
	return 1;
}

static int
jent_raw_rand_status(void)
{
	if(jent_raw)
		return 1;
	return 0;
}

static int
jent_raw_rand_bytes(unsigned char *data, int num)
{
	int rc;
	if(!jent_raw)
		return 0;
	if(0 >= num)
		return 0;
	CRYPTO_w_lock(CRYPTO_LOCK_RAND);
	rc = jent_read_entropy(jent_raw, (char *)data, num);
	CRYPTO_w_unlock(CRYPTO_LOCK_RAND);
	if(0 > rc)
		return 0;
	return 1;
}

static RAND_METHOD jent_raw_rand =
{
	NULL,			/* seed */
	jent_raw_rand_bytes,	/* bytes */
	NULL,			/* cleanup */
	NULL,			/* add */
	jent_raw_rand_bytes,	/* pseudorand */
	jent_raw_rand_status,	/* rand status */
};

static int
jent_bind_helper(ENGINE *e)
{
	if(jent_entropy_init())
		return 0;
	if (!ENGINE_set_id(e, jent_raw_id) ||
	    !ENGINE_set_name(e, jent_raw_name) ||
	    !ENGINE_set_init_function(e, jent_raw_init) ||
	    !ENGINE_set_RAND(e, &jent_raw_rand) ||
	    !ENGINE_set_finish_function(e, &jent_raw_free)
	    )
	{
		return 0;
	}
	return 1;
	
}

static ENGINE *
ENGINE_jent_raw(void)
{
	ENGINE *eng = ENGINE_new();

	if (!eng)
		return NULL;
	if(!jent_bind_helper(eng))
	{
		ENGINE_free(eng);
		return NULL;
	}
	return eng;
}

void ENGINE_load_jent_raw(void)
{
	ENGINE *toadd = ENGINE_jent_raw();
	if(!toadd) return;
	ENGINE_add(toadd);
	ENGINE_free(toadd);
	ERR_clear_error();
}

#ifdef ENGINE_DYNAMIC_SUPPORT
static int
jent_bind_fn(ENGINE *e, const char *id)
{
	if (id && (strcmp(id, jent_raw_id) != 0))
		return 0;

        if (!jent_bind_helper(e))
		return 0;

        return 1;
}

IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(jent_bind_fn)
#endif /* DYNAMIC_ENGINE */
	
