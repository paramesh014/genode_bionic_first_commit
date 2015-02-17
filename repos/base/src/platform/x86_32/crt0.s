/**
 * \brief   Startup code for Genode applications
 * \author  Christian Helmuth
 * \author  Martin Stein
 * \date    2009-08-12
 */

/*
 * Copyright (C) 2009-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */


/**************************
 ** .text (program code) **
 **************************/

.text

	/* program entry-point */
	.global _start
	_start:

	/* initialize GOT pointer in EBX */
	3:
	movl $., %ebx
	addl $_GLOBAL_OFFSET_TABLE_ + (. - 3b) , %ebx
	movl %esp, __initial_sp@GOTOFF(%ebx)

	/* make initial value of some registers available to higher-level code */
	mov %esp, __initial_sp
	mov %eax, __initial_ax
	mov %edi, __initial_di

	/*
	 * Install initial temporary environment that is replaced later by the
	 * environment that init_main_thread creates.
	 */
	leal _stack_high@GOTOFF(%ebx), %esp

	/* if this is the dynamic linker, init_rtld relocates the linker */
	call init_rtld

	/* create proper environment for the main thread */
	call init_main_thread

	/* apply environment that was created by init_main_thread */
	movl init_main_thread_result, %esp

	/* clear the base pointer in order that stack backtraces will work */
	xor %ebp, %ebp

	/* jump into init C code instead of calling it as it should never return */
	jmp _main


/*********************************
 ** .bss (non-initialized data) **
 *********************************/

.bss

	/* stack of the temporary initial environment */
	.p2align 4
	.space 32 * 1024
	_stack_high:

	/* initial value of the ESP, EAX and EDI register */
	.global __initial_sp
	__initial_sp:
	.space 4
	.global __initial_ax
	__initial_ax:
	.space 4
	.global __initial_di
	__initial_di:
	.space 4
