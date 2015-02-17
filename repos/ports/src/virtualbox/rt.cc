/*
 * \brief  VirtualBox runtime (RT)
 * \author Norman Feske
 * \date   2013-08-20
 */

/*
 * Copyright (C) 2013 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

/* Genode includes */
#include <base/printf.h>
#include <base/env.h>

/* VirtualBox includes */
#include <iprt/initterm.h>
#include <iprt/mem.h>
#include <iprt/err.h>
#include <iprt/semaphore.h>
#include <iprt/time.h>
#include <internal/iprt.h>


/*
 * Called by the recompiler to allocate executable RAM
 */

void *RTMemExecAllocTag(size_t cb, const char *pszTag) RT_NO_THROW
{
	PDBG("size=0x%zx, tag=%s", cb, pszTag);

	/*
	 * XXX error handling is missing
	 */

	using namespace Genode;
	Ram_dataspace_capability ds = env()->ram_session()->alloc(cb);

	size_t        const whole_size = 0;
	Genode::off_t const offset     = 0;
	bool          const executable = true;
	bool          const any_addr   = false;

	void *local_addr =
		env()->rm_session()->attach(ds, whole_size, offset,
		                            any_addr, (void *)0, executable);

	return local_addr;
}


void *RTMemPageAllocZTag(size_t cb, const char *pszTag) RT_NO_THROW
{
	/*
	 * The RAM dataspace freshly allocated by 'RTMemExecAllocTag' is zeroed
	 * already.
	 */
	return RTMemExecAllocTag(cb, pszTag);
}



void *RTMemPageAllocTag(size_t cb, const char *pszTag) RT_NO_THROW
{
	return RTMemPageAllocZTag(cb, pszTag);
}


#include <iprt/buildconfig.h>

uint32_t RTBldCfgVersionMajor(void) { return VBOX_VERSION_MAJOR; }
uint32_t RTBldCfgVersionMinor(void) { return VBOX_VERSION_MINOR; }
uint32_t RTBldCfgVersionBuild(void) { return VBOX_VERSION_BUILD; }
uint32_t RTBldCfgRevision(void)     { return ~0; }


extern "C" DECLHIDDEN(int) rtProcInitExePath(char *pszPath, size_t cchPath)
{
	Genode::strncpy(pszPath, "/virtualbox", cchPath);
	return 0;
}
