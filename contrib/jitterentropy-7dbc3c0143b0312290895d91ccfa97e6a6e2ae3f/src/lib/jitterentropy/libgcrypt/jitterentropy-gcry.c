/* jitterentropy-gcry-user.c  -  raw random number for OSes with high
 * resolution timer to employ the CPU Jitter random number generator
 *
 * Copyright (C) 2013 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include "types.h"
#include "g10lib.h"
#include "rand-internal.h"
#include "ath.h"

#include "jitterentropy.h"

/* For each of the RNG strength types, allocate one separate instance of the
 * CPU Jitter random number generator */
/* The memory should be allocated in libgcrypt's secmem which is automatically
 * wiped at termination time of the application. Nonetheless, we employ a
 * deallocation function to allow the user to disable the CPU Jitter entropy
 * collector if he desires. In this case clean out the memory. */
/* Another implementation would be that the entropy collector is allocated
 * on the fly in _gcry_jent_gather_random and immediately deallocated after
 * use. This way, no state would linger in memory and the entire code in this
 * file could shrink to a minimum. However, the processing overhead is larger
 * as every initialization of an entropy collector generates one random number
 * for initialization. */
struct gcry_jent_rng {
  ath_mutex_t rng_lock;
  struct rand_data *entropy_collector;
};

static struct gcry_jent_rng weak_entropy_collector;
static struct gcry_jent_rng strong_entropy_collector;
static struct gcry_jent_rng very_strong_entropy_collector;

/* Take the rng lock. */
static void
_gcry_jent_lock_rng(struct gcry_jent_rng *jent_rng)
{
  int err;

  err = ath_mutex_lock (&jent_rng->rng_lock);
  if (err)
    log_fatal ("failed to acquire the CPU Jitter RNG lock: %s\n", strerror (err));
}

/* Release the rng lock. */
static void
_gcry_jent_unlock_rng(struct gcry_jent_rng *jent_rng)
{
  int err;

  err = ath_mutex_unlock (&jent_rng->rng_lock);
  if (err)
    log_fatal ("failed to release the CPU Jitter RNG lock: %s\n", strerror (err));
}

static int
_gcry_jent_rng_alloc(struct gcry_jent_rng *jent_rng)
{
  int err = 0;
  if(!jent_rng->entropy_collector)
    {
      /* create the mutex first so that we can lock the entropy collector
       * creation, just in case */
      err = ath_mutex_init(&jent_rng->rng_lock);
      if(err)
        return err;
      _gcry_jent_lock_rng(jent_rng);
      jent_rng->entropy_collector = jent_entropy_collector_alloc(1, 0);
      if(!jent_rng->entropy_collector)
        err = -1;
      _gcry_jent_unlock_rng(jent_rng);
      if(err)
        ath_mutex_destroy(&jent_rng->rng_lock);
    }
  return err;
}

static void
_gcry_jent_rng_free(struct gcry_jent_rng *jent_rng)
{
  if(jent_rng->entropy_collector)
    {
      _gcry_jent_lock_rng(jent_rng);
      jent_entropy_collector_free(jent_rng->entropy_collector);
      _gcry_jent_unlock_rng(jent_rng);
      ath_mutex_destroy(&jent_rng->rng_lock);
    }
  /* ensure that the pointers are set to NULL which helps the validation
   * logic in _gcry_jent_gather_random to decide whether the CPU Jitter
   * entropy collection can commence */
  jent_rng->entropy_collector = NULL;
}

int
_gcry_jent_init(void)
{
  if(jent_entropy_init())
    return -1;
  /* ensure smooth operation even when an application tries to enable us
   * multiple times, we allocate only once */
  if(_gcry_jent_rng_alloc(&weak_entropy_collector))
    return -1;
  if(_gcry_jent_rng_alloc(&strong_entropy_collector))
    {
      _gcry_jent_rng_free(&weak_entropy_collector);
      return -1;
    }
  if(_gcry_jent_rng_alloc(&very_strong_entropy_collector))
    {
      _gcry_jent_rng_free(&strong_entropy_collector);
      _gcry_jent_rng_free(&weak_entropy_collector);
      return -1;
    }
  return 0;
}

void
_gcry_jent_free(void)
{
  _gcry_jent_rng_free(&very_strong_entropy_collector);
  _gcry_jent_rng_free(&strong_entropy_collector);
  _gcry_jent_rng_free(&weak_entropy_collector);
}

int
_gcry_jent_gather_random (void (*add)(const void*, size_t,
                                      enum random_origins),
                          enum random_origins origin,
                          size_t length, int level )
{
  char buffer[768];
  struct gcry_jent_rng *jent_rng;

  if (2 <= level)
    jent_rng = &very_strong_entropy_collector;
  else if (1 == level)
    jent_rng = &strong_entropy_collector;
  else
    jent_rng = &weak_entropy_collector;
  if(!jent_rng->entropy_collector)
    return 1; /* CPU Jitter entropy collection is not available as it was
		 not initialized with GCRYCTL_ENABLE_CPU_JITTER_ENTROPY or
		 it was disabled with GCRYCTL_DISABLE_CPU_JITTER_ENTROPY
	       */

  _gcry_jent_lock_rng(jent_rng);
  while (length)
    {
      int rc;
      size_t nbytes = length < sizeof(buffer)? length : sizeof(buffer);

      rc = jent_read_entropy(jent_rng->entropy_collector, buffer, nbytes);
      if(0 > rc)
        return -1;

      (*add)( buffer, rc, origin );
      length -= rc;
    }
  memset(buffer, 0, sizeof(buffer) );
  _gcry_jent_unlock_rng(jent_rng);

  return 0; /* success */
}

