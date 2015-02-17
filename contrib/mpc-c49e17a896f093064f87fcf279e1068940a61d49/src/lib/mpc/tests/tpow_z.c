/* test file for mpc_pow_z.

Copyright (C) INRIA, 2009

This file is part of the MPC Library.

The MPC Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPC Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPC Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "mpc-tests.h"

int
main (void)
{
  mpc_t z;
  mpz_t t;

  test_start ();

  mpc_init2 (z, 5);
  mpz_init_set_ui (t, 1);
  mpc_set_ui_ui (z, 17, 42, MPC_RNDNN);
  mpc_pow_z (z, z, t, MPC_RNDNN);
  if (mpc_cmp_si_si (z, 17, 42) != 0)
    {
      printf ("Error for mpc_pow_z (1)\n");
      exit (1);
    }
  mpc_clear (z);
  mpz_clear (t);

  test_end ();

  return 0;
}
