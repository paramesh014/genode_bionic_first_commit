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

#ifndef _JITTERENTROPY_BASE_KERNEL_H
#define _JITTERENTROPY_BASE_KERNEL_H

#include <linux/slab.h> /* needed for kzalloc */
#include <linux/module.h> /* needed for random_get_entropy */
#include <linux/fips.h> /* needed for fips_enabled */
#include <linux/time.h> /* needed for __getnstimeofday */

static inline void jent_get_nstime(__u64 *out)
{
	struct timespec ts;
	__u64 tmp = 0;

	tmp = random_get_entropy();

	/* If random_get_entropy does not return a value (which is possible on,
	 * for example, MIPS), invoke __getnstimeofday
	 * hoping that there are timers we can work with.
	 *
	 * The list of available timers can be obtained from
	 * /sys/devices/system/clocksource/clocksource0/available_clocksource
	 * and are registered with clocksource_register()
	 */
	if((0 == tmp) &&
#ifndef MODULE
	   (0 == timekeeping_valid_for_hres()) && 
#endif
	   (0 == __getnstimeofday(&ts)))
	{
		tmp = ts.tv_sec;
		tmp = tmp << 32;
		tmp = tmp | ts.tv_nsec;
	}

	*out = tmp;
}

static inline void *jent_zalloc(size_t len)
{
	/* We consider kernel memory as secure -- if somebody breaks it,
	 * the user has much more pressing problems than the state of our
	 * RNG */
#define CONFIG_CRYPTO_CPU_JITTERENTROPY_SECURE_MEMORY
	return kzalloc(len, GFP_KERNEL);
}
static inline void jent_zfree(void *ptr, unsigned int len)
{
	kzfree(ptr);
}

static inline int jent_fips_enabled(void)
{
	return fips_enabled;
}

#endif /* _JITTERENTROPY_BASE_KERNEL_H */

