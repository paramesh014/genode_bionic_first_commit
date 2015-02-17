/*
 * Non-physical true random number generator based on timing jitter -- DRNG part
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
 *
 */

#include <linux/module.h>
#include <crypto/internal/rng.h>

#include "jitterentropy.h"
#include "jitterentropy-drng.h"
#include "jitterentropy-kcapi.h"

/*************************************************************************
 * Export DRNG instances to kernel crypto API
 *************************************************************************/

static int jent_kcapi_init_drng(struct crypto_tfm *tfm)
{
	return jent_drng_init_drng(crypto_tfm_ctx(tfm));
}

static void jent_kcapi_cleanup_drng(struct crypto_tfm *tfm)
{
	jent_drng_cleanup_drng(crypto_tfm_ctx(tfm));
}

static int jent_kcapi_reg_random(struct crypto_rng *tfm, u8 *rdata,
			       unsigned int dlen)
{
	return jent_drng_get_bytes(crypto_rng_ctx(tfm),
				   MAX_BYTES_RESEED, MAX_BYTES_REKEY,
				   rdata, dlen);
}

static int jent_kcapi_strong_random(struct crypto_rng *tfm, u8 *rdata,
				  unsigned int dlen)
{
	return jent_drng_get_bytes(crypto_rng_ctx(tfm),
				   MAX_BYTES_STRONG_RESEED,
				   MAX_BYTES_STRONG_REKEY,
				   rdata, dlen);
}

static int jent_kcapi_reset_drng(struct crypto_rng *tfm, u8 *seed, unsigned int slen)
{
	return jent_drng_reseed_rekey(crypto_rng_ctx(tfm), 0, 0, 0);
}

/*************************************************************************
 * Export raw entropy instance to kernel crypto API
 *************************************************************************/
static int jent_kcapi_init_raw(struct crypto_tfm *tfm)
{
	return jent_drng_init_raw(crypto_tfm_ctx(tfm), 0);
}

static void jent_kcapi_cleanup_raw(struct crypto_tfm *tfm)
{
	jent_drng_cleanup_raw(crypto_tfm_ctx(tfm));
}

static int jent_kcapi_raw_random(struct crypto_rng *tfm, u8 *rdata,
			       unsigned int dlen)
{
	return jent_drng_get_bytes_raw(crypto_rng_ctx(tfm), rdata, dlen);
}

static int jent_kcapi_reset_raw(struct crypto_rng *tfm, u8 *seed, unsigned int slen)
{
	return 0;
}

/*************************************************************************
 * Definitions of random number generators for kernel crypto API
 *************************************************************************/
static struct crypto_alg jent_drng_algs[] = { {
	.cra_name               = "reg(jent_drng)",
	.cra_driver_name        = "reg_jent_drng",
	.cra_priority           = 100,
	.cra_flags              = CRYPTO_ALG_TYPE_RNG,
	.cra_ctxsize            = sizeof(struct jent_drng),
	.cra_type               = &crypto_rng_type,
	.cra_module             = THIS_MODULE,
	.cra_init               = jent_kcapi_init_drng,
	.cra_exit               = jent_kcapi_cleanup_drng,
	.cra_u                  = {
		.rng =	{
			.rng_make_random	= jent_kcapi_reg_random,
			.rng_reset		= jent_kcapi_reset_drng,
			.seedsize		= 0,
			}
	}
}, {
	.cra_name               = "strong(jent_drng)",
	.cra_driver_name        = "strong_jent_drng",
	.cra_priority           = 300,
	.cra_flags              = CRYPTO_ALG_TYPE_RNG,
	.cra_ctxsize            = sizeof(struct jent_drng),
	.cra_type               = &crypto_rng_type,
	.cra_module             = THIS_MODULE,
	.cra_init               = jent_kcapi_init_drng,
	.cra_exit               = jent_kcapi_cleanup_drng,
	.cra_u                  = {
		.rng =	{
			.rng_make_random	= jent_kcapi_strong_random,
			.rng_reset		= jent_kcapi_reset_drng,
			.seedsize		= 0,
			}
	}
}, {
	.cra_name               = "raw(jent_drng)",
	.cra_driver_name        = "raw_jent_drng",
	.cra_priority           = 300,
	.cra_flags              = CRYPTO_ALG_TYPE_RNG,
	.cra_ctxsize            = sizeof(struct jent_raw),
	.cra_type               = &crypto_rng_type,
	.cra_module             = THIS_MODULE,
	.cra_init               = jent_kcapi_init_raw,
	.cra_exit               = jent_kcapi_cleanup_raw,
	.cra_u                  = {
		.rng =	{
			.rng_make_random	= jent_kcapi_raw_random,
			.rng_reset		= jent_kcapi_reset_raw,
			.seedsize		= 0,
			}
	}
} };

int __init jent_kcapi_init(void)
{
	return crypto_register_algs(jent_drng_algs, ARRAY_SIZE(jent_drng_algs));
}

void __exit jent_kcapi_exit(void)
{
	crypto_unregister_algs(jent_drng_algs, ARRAY_SIZE(jent_drng_algs));
}
