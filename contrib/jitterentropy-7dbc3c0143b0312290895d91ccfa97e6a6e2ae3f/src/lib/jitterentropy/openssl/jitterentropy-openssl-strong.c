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

/* Reseed and rekey limits for strong DRNG */
#define MAX_BYTES_STRONG_RESEED 16    /* max bytes before reseed */
#define MAX_BYTES_STRONG_REKEY  1<<10 /* max bytes before update of seed key */

/* Strong DRNG */
static struct jent_drng jent_strong;
static const char *jent_strong_id = "jitterentropy-strong";
static const char *jent_strong_name = "CPU Jitter random number generator (Strong DRNG interface)";

static int
jent_strong_init(ENGINE *e)
{
	return jent_init(&jent_strong);
}

static int
jent_strong_free(ENGINE *e)
{
	return jent_release(&jent_strong);
}

static int
jent_strong_rand_status(void)
{
	return jent_rand_status(&jent_strong);
}

static int
jent_strong_rand_bytes(unsigned char *data, int num)
{
	return jent_rand_bytes(data, num, &jent_strong,
			       MAX_BYTES_STRONG_RESEED, MAX_BYTES_STRONG_REKEY);
}

static RAND_METHOD jent_strong_rand =
{
	jent_seed,		/* seed */
	jent_strong_rand_bytes,	/* bytes */
	NULL,			/* cleanup */
	jent_add,		/* add */
	jent_strong_rand_bytes,	/* pseudorand */
	jent_strong_rand_status,/* rand status */
};

static int
jent_strong_bind_helper(ENGINE *e)
{
	if(jent_entropy_init())
		return 0;
	if (!ENGINE_set_id(e, jent_strong_id) ||
	    !ENGINE_set_name(e, jent_strong_name) ||
	    !ENGINE_set_init_function(e, jent_strong_init) ||
	    !ENGINE_set_RAND(e, &jent_strong_rand) ||
	    !ENGINE_set_finish_function(e, &jent_strong_free)
	    )
	{
		return 0;
	}
	return 1;
}

static ENGINE *
ENGINE_jent_strong(void)
{
	ENGINE *eng = ENGINE_new();

	if (!eng)
		return NULL;
	if(!jent_strong_bind_helper(eng))
	{
		ENGINE_free(eng);
		return NULL;
	}
	return eng;
}

void ENGINE_load_jent_strong(void)
{
	ENGINE *toadd = ENGINE_jent_strong();
	if(!toadd) return;
	ENGINE_add(toadd);
	ENGINE_free(toadd);
	ERR_clear_error();
}

#ifdef ENGINE_DYNAMIC_SUPPORT
static int
jent_strong_bind_fn(ENGINE *e, const char *id)
{
	if (id && (strcmp(id, jent_strong_id) != 0))
		return 0;

        if (!jent_strong_bind_helper(e))
		return 0;

        return 1;
}

IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(jent_strong_bind_fn)
#endif /* DYNAMIC_ENGINE */

