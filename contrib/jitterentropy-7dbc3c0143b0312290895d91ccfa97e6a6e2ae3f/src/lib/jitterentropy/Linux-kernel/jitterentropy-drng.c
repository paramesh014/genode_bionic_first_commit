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
 * Design
 * ======
 *
 * See documentation in doc/ folder.
 *
 * Interface
 * =========
 *
 * See documentation in doc/ folder.
 *
 * TODO: reseed and rekey are not (yet) implemented in the kernel crypto API.
 *	 Therefore, this code always performs a DRNG reset when one of the
 *	 limits are reached.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fips.h>

#include <linux/crypto.h>
#include <crypto/rng.h>
#include <crypto/hash.h>

#include "jitterentropy.h"
#include "jitterentropy-drng.h"
#include "jitterentropy-dbg.h"
#include "jitterentropy-kcapi.h"

/*************************************************************************
 * DRNG instance seeded, reseeded and re-keyed by the jitter entropy
 *
 * This DRNG is intended to provide good random numbers out of a
 * DRNG implemented in the kernel crypto API
 *************************************************************************/

/* clean up what jent_drng_init_drng allocated */
void jent_drng_cleanup_drng(struct jent_drng *drng)
{
	spin_lock_bh(&drng->drng_lock);
	if (NULL != drng->drng)
		crypto_free_rng(drng->drng);
	drng->drng = NULL;
	if (NULL != drng->entropy_collector)
		jent_entropy_collector_free(drng->entropy_collector);
	drng->entropy_collector = NULL;
	spin_unlock_bh(&drng->drng_lock);
}

void jent_drng_cleanup_raw(struct jent_raw *raw)
{
	spin_lock_bh(&raw->raw_lock);
	if (NULL != raw->entropy_collector)
		jent_entropy_collector_free(raw->entropy_collector);
	raw->entropy_collector = NULL;
	spin_unlock_bh(&raw->raw_lock);
}

/* seed or reseed the DRNG -- any locks necessary must be held by the caller */
#define DRNG_RESET 1
#define DRNG_RESEED 2
static int jent_drng_update(struct jent_drng *drng, int type)
{
	/* since we are in atomic context with a lock taken, we
	 * cannot kzalloc the memory */
	char seed[100];
	size_t seedsize = 0;
	int ret = 0;

	/*XXX remove the following once the ANSI X9.31 DRNG implementation
	 * is updated */
	type = DRNG_RESET;

	if (DRNG_RESET == type)
		seedsize = crypto_rng_seedsize(drng->drng);
	else if (DRNG_RESEED == type)
		seedsize = 16; /* there is no API to get that value but we know
				  that the ANSI X9.31 V value is 16 bytes */

	if (sizeof(seed) < seedsize || 0 > seedsize) {
		printk(DRIVER_NAME": seedsize %zu reported by RNG is unexpected!\n", seedsize);
		ret = -EFAULT;
		goto out;
	}

	memset(&seed, 0, sizeof(seed));
	if (0 > jent_read_entropy(drng->entropy_collector, seed, seedsize)) {
		printk(DRIVER_NAME": Unable to obtain %zu bytes of entropy\n", seedsize);
		ret = -EAGAIN;
		goto out;
	}

	/* enable code only when kernel crypto API is extended with a
	 * reseed interface -- in this case, remove the code marked
	 * above!
	 */
#if 0
	if (DRNG_RESEED == type)
		ret = crypto_rng_reseed(drng->drng, seed, seedsize);
	else
#endif
		ret = crypto_rng_reset(drng->drng, seed, seedsize);
	if (ret)
		printk(DRIVER_NAME": Failed to update rng\n");

out:
	return ret;
}

/* initialization of DRNG and reset of the DRNG:
 * 1. allocation of the DRNG uses as frontend
 * 2. allocation of entropy collector for seeding
 */
int jent_drng_init_drng(struct jent_drng *drng)
{
	int ret = 0;
	char name[17];
	struct crypto_rng *rng;

	/* init DRNG */

	/* if there are more DRNGs in the kernel in the future,
	 * this can be made selectable via a module parameter.
	 */
	memset(&name, 0, sizeof(name));
	if (fips_enabled)
		sprintf(name, "fips(ansi_cprng)");
	else
		sprintf(name, "ansi_cprng");
	rng = crypto_alloc_rng(name, 0, 0);
	if (IS_ERR(rng)) {
		printk(DRIVER_NAME": could not allocate RNG handle\n");
		return -ENOMEM;
	}

	/* init entropy collector */
	drng->entropy_collector = jent_entropy_collector_alloc(1, 0);
	if (!drng->entropy_collector) {
		crypto_free_rng(drng->drng);
		drng->drng = NULL;
		return -ENOMEM;
	}

	drng->drng = rng;
	drng->norekey_data = 0;
	drng->noreseed_data = 0;
	jent_drng_stat_read_init(drng);
	jent_drng_stat_reseed_init(drng);
	jent_drng_stat_rekey_init(drng);

	/* seed the new instance of the DRNG with the entropy collector */
	ret = jent_drng_update(drng, DRNG_RESET);
	if (ret)
		jent_drng_cleanup_drng(drng);

	/* initialize the spinlock last so that it can only be taken
	 * after the memory allocation is successfully completed */
	spin_lock_init(&drng->drng_lock);

	return ret;
}

int jent_drng_init_raw(struct jent_raw *raw, unsigned int flags)
{
	int ret = 0;

	raw->entropy_collector = jent_entropy_collector_alloc(1, flags);
	if (!raw->entropy_collector)
		ret = -ENOMEM;

	/* we do not need the reseed/rekey values, and therefore do not
	 * initialize them */
	jent_drng_stat_read_init(raw);

	/* initialize the spinlock last so that it can only be taken
	 * after the memory allocation is successfully completed */
	spin_lock_init(&raw->raw_lock);
	return ret;
}

/* call this function with the limits and bytes set to 0 to force
 * reseed of the seed and key
 *
 * return: the function returns the maximum bytes the caller is allowed to
 * 	   pull from the RNG before another call to this function
 * 	   is to be made
 */
size_t jent_drng_reseed_rekey(struct jent_drng *drng, const size_t lim_reseed,
			      const size_t lim_rekey, size_t bytes)
{
	size_t max_bytes = 0;

	if (drng->norekey_data >= lim_rekey) {
		if (jent_drng_update(drng, DRNG_RESET))
			return 0;
		jent_drng_stat_rekey_add(drng, 1);
		drng->norekey_data = 0;
		drng->noreseed_data = 0;
	}
	/* if we did a full reset, a reseed is not needed */
	else if (drng->noreseed_data >= lim_reseed) {
		if (jent_drng_update(drng, DRNG_RESEED))
			return 0;
		jent_drng_stat_reseed_add(drng, 1);
		drng->noreseed_data = 0;
	}

	/* calculate the bytes to be obtained in the next round of the RNG
	 * to not overstep the limit boundaries */
	max_bytes = min_t(size_t, (lim_rekey - drng->norekey_data),
		       		  (lim_reseed - drng->noreseed_data));
	/* of course, the maximum bytes cannot be higher than the bytes
	 * requested */
	max_bytes = min_t(size_t, max_bytes, bytes);

	return max_bytes;
}

ssize_t jent_drng_get_bytes(struct jent_drng *drng, const size_t lim_reseed,
			    const size_t lim_rekey, char *data, size_t len)
{
	int ret = 0;
	size_t pull = 0;
	char *p = data;
	size_t orig_len = len;

	while (0 < len) {
		spin_lock_bh(&drng->drng_lock);
		pull = jent_drng_reseed_rekey(drng, lim_reseed, lim_rekey, len);
		if (!pull) {
			spin_unlock_bh(&drng->drng_lock);
			return -EFAULT;
		}

		ret = crypto_rng_get_bytes(drng->drng, p, pull);
		if (0 >= ret) {
			spin_unlock_bh(&drng->drng_lock);
			printk(DRIVER_NAME": Unable to obtain %zu bytes from DRNG -- error code: %d\n", pull, ret);
			return ret;
		}
		drng->noreseed_data += ret;
		drng->norekey_data += ret;
		jent_drng_stat_read_add(drng, ret);
		spin_unlock_bh(&drng->drng_lock);

		len -= ret;
		p += ret;
	}

	return orig_len;
}

int jent_drng_get_bytes_raw(struct jent_raw *raw, char *data, size_t len)
{
	int ret = 0;

	spin_lock_bh(&raw->raw_lock);
	ret = jent_read_entropy(raw->entropy_collector, data, len);

	if (0 > ret) {
		printk(DRIVER_NAME": Unable to obtain %zu bytes of entropy\n", len);
		ret = -EAGAIN;
	}
	else
		jent_drng_stat_read_add(raw, ret);

	spin_unlock_bh(&raw->raw_lock);
	return ret;
}

/****************************************************************
 * initialization of kernel module
 ****************************************************************/

static int __init jent_drng_init(void)
{
	int ret = 0;
	int dbg = 0;
	int kcapi = 0;

	ret = jent_entropy_init();
	if (ret) {
		printk(DRIVER_NAME ": Initialization failed with host not compliant with requirements: %d\n", ret);
		return -EFAULT;
	}

#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG
	ret = jent_dbg_init();
	if (ret)
		return ret;
	dbg = 1;
#endif

#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_KCAPI
	ret = jent_kcapi_init();
	if (ret) {
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG
		/* uninitialize the DBG interface */
		jent_dbg_exit();
#endif /* CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG */
		return ret;
	}
	kcapi = 1;
#endif /* CONFIG_CRYPTO_CPU_JITTERENTROPY_KCAPI */

	printk(DRIVER_NAME ": Initialized and ready for operation with interfaces%s%s\n",
			dbg	? " -DebugFS-" : "",
			kcapi	? " -Kernel crypto API-" : ""
	      );

	return ret;
}

static void __exit jent_drng_exit(void)
{
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG
	jent_dbg_exit();
#endif
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_KCAPI
	jent_kcapi_exit();
#endif
	printk(DRIVER_NAME ": Deconfigured and removed\n");
}

module_init(jent_drng_init);
module_exit(jent_drng_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephan Mueller <smueller@chronox.de>");
MODULE_DESCRIPTION("Non-physical True Random Number Generator based on CPU Jitter");

