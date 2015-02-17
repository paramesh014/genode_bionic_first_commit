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

static int enabled = 0;

int
_gcry_jent_init(void)
{
  if(jent_entropy_init())
    return -1;
  enabled = 1;
  return 0;
}

void
_gcry_jent_free(void)
{
  enabled = 0;
}

int
_gcry_jent_gather_random (void (*add)(const void*, size_t,
                                      enum random_origins),
                          enum random_origins origin,
                          size_t length, int level )
{
  char buffer[768];
  struct rand_data *ec;

  /* CPU Jitter entropy collection is not available as it was
     not initialized with GCRYCTL_ENABLE_CPU_JITTER_ENTROPY or
     it was disabled with GCRYCTL_DISABLE_CPU_JITTER_ENTROPY */
  if(!enabled)
    return 1;

  ec = jent_entropy_collector_alloc(1, 0);
  if(!ec)
    return -1;

  while (length)
    {
      int rc;
      size_t nbytes = length < sizeof(buffer)? length : sizeof(buffer);

      rc = jent_read_entropy(ec, buffer, nbytes);
      if(0 > rc)
        return -1;

      (*add)(buffer, rc, origin);
      length -= rc;
    }
  memset(buffer, 0, sizeof(buffer));
  jent_entropy_collector_free(ec);
  ec = NULL;

  return 0; /* success */
}

