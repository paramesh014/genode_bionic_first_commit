/*-
 * Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
 *    redistribution must be conditioned upon including a substantially
 *    similar Disclaimer requirement for further binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.
 *
 * $Id: compat.h 3051 2007-12-13 04:07:54Z proski $
 */
#ifndef _ATH_COMPAT_H_
#define _ATH_COMPAT_H_

/* Compatibility with older Linux kernels */
#ifdef __KERNEL__
#include <linux/types.h>
#endif
#if !defined(__KERNEL__) || !defined (__bitwise)
#define __le16 u_int16_t
#define __le32 u_int32_t
#define __le64 u_int64_t
#define __be16 u_int16_t
#define __be32 u_int32_t
#define __be64 u_int64_t
#define __force
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

/*
 * BSD/Linux compatibility shims.  These are used mainly to
 * minimize differences when importing necesary BSD code.
 */
#define	NBBY	8			/* number of bits/byte */

/* roundup() appears in Linux 2.6.18 */
#include <linux/kernel.h>
#ifndef roundup
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))  /* to any y */ 
#endif

#define	howmany(x, y)	(((x)+((y)-1))/(y))

/* Bit map related macros. */
#define	setbit(a,i)	((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a,i)	((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a,i)	((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a,i)	(((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

#ifndef __packed
#define	__packed	__attribute__((__packed__))
#endif

#define	__printflike(_a,_b) \
	__attribute__ ((__format__ (__printf__, _a, _b)))
#define	__offsetof(t,m)	offsetof(t,m)

#ifndef ALIGNED_POINTER
/*
 * ALIGNED_POINTER is a boolean macro that checks whether an address
 * is valid to fetch data elements of type t from on this architecture.
 * This does not reflect the optimal alignment, just the possibility
 * (within reasonable limits). 
 *
 */
#define ALIGNED_POINTER(p,t)	1
#endif

#ifdef __KERNEL__
#define	KASSERT(exp, msg) do {			\
	if (unlikely(!(exp))) {			\
		printk msg;			\
		BUG();				\
	}					\
} while (0)
#endif /* __KERNEL__ */

/*
 * NetBSD/FreeBSD defines for file version.
 */
#define	__FBSDID(_s)
#define	__KERNEL_RCSID(_n,_s)

/*
 * Fixes for Linux API changes
 */
#ifdef __KERNEL__

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21)
#define ATH_REGISTER_SYSCTL_TABLE(t) register_sysctl_table(t, 1)
#else
#define ATH_REGISTER_SYSCTL_TABLE(t) register_sysctl_table(t)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9)
#define __user
#define __kernel
#define __iomem
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#define IRQF_SHARED SA_SHIRQ
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
#include <linux/skbuff.h>
static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}

static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static inline void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb->nh.raw = skb->data + offset;
}

static inline void skb_reset_network_header(struct sk_buff *skb)
{
	skb->nh.raw = skb->data;
}

static inline unsigned char *skb_mac_header(const struct sk_buff *skb)
{
	return skb->mac.raw;
}

static inline void skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac.raw = skb->data;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#define CTL_AUTO -2
#define DEV_ATH 9
#else
#define CTL_AUTO CTL_UNNUMBERED
#define DEV_ATH CTL_UNNUMBERED
#endif

#endif /* __KERNEL__ */

#endif /* _ATH_COMPAT_H_ */
