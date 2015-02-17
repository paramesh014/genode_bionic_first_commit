/*
 * Non-physical true random number generator based on timing jitter - DebugFS
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#include <linux/debugfs.h>

/* connection to entropy collector */
#include "jitterentropy.h"
/* connection to DRNG manager */
#include "jitterentropy-drng.h"
#include "jitterentropy-dbg.h"

/************************************************
 * Definition of the DRNG
 ************************************************/

/* Raw entropy interface */
static struct jent_raw raw_entropy;

static inline int jent_dbg_raw_bytes(char *data, size_t len)
{
        return jent_drng_get_bytes_raw(&raw_entropy, data, len);
}

/* Handle for the regular DRNG which is reseeded once in a while */
static struct jent_drng jent_reg_drng;
/* Handle for strong DRNG which is constantly reseeded */
static struct jent_drng jent_strong_drng;

static inline void jent_dbg_get_stat(struct jent_stat *src,
				     struct jent_stat *stat)
{
	stat->read_data = src->read_data;
	stat->num_reseed = src->num_reseed;
	stat->num_rekey = src->num_rekey;
}

static inline void jent_dbg_raw_get_stat(struct jent_stat *stat)
{
	jent_dbg_get_stat(&raw_entropy.jent_stat, stat);
}

static inline void jent_dbg_reg_get_stat(struct jent_stat *stat)
{
	jent_dbg_get_stat(&jent_reg_drng.jent_stat, stat);
}

static inline void jent_dbg_strong_get_stat(struct jent_stat *stat)
{
	jent_dbg_get_stat(&jent_strong_drng.jent_stat, stat);
}

/*
 * Entry function: Obtain random numbers out of DRNG.
 *
 * This function invokes the DRNG as often to generate as many bytes as
 * requested by the caller. This function also ensures that the DRNG is always
 * properly seeded.
 *
 * This function truncates the output to the exact size specified by the caller.
 *
 * @data: pointer to buffer for storing random data -- buffer must already
 *        exist
 * @len: size of the buffer, specifying also the requested number of random
 *       in bytes
 *
 * return: 0 when request is fulfilled or an error
 *
 * The following error codes can occur:
 *      EAGAIN  Initialization, seeding, reseeding of DRNG failed
 *      EFAULT  Generation of random numbers out of DRNG failed
 */
static inline int jent_dbg_random_bytes(char *data, size_t len)
{
        return jent_drng_get_bytes(&jent_reg_drng,
				   MAX_BYTES_RESEED, MAX_BYTES_REKEY,
				   data, len);
}

/*
 * Entry function: Obtain random numbers out of constantly seeded DRNG.
 *                 It follows the same logic as jent_dbg_random_bytes
 *                 with the exception that the DRNG is constantly seeded.
 */
static inline int jent_dbg_strong_random_bytes(char *data, size_t len)
{
        return jent_drng_get_bytes(&jent_strong_drng,
				   MAX_BYTES_STRONG_RESEED,
				   MAX_BYTES_STRONG_REKEY,
				   data, len);
}

/***********************************************
 * The interface
 ***********************************************/

/* debugfs interface */
struct jent_debugfs {
	struct dentry *jent_debugfs_root; /* /sys/kernel/debug/jent */
	struct dentry *jent_debugfs_stat; /* /sys/kernel/debug/jent/stat */
	struct dentry *jent_debugfs_stattimer; /* /sys/kernel/debug/jent/stat-timer */
	struct dentry *jent_debugfs_statfold; /* /sys/kernel/debug/jent/stat-fold */
	struct dentry *jent_debugfs_statbits; /* /sys/kernel/debug/jent/stat-bits */
	struct dentry *jent_debugfs_seed; /* /sys/kernel/debug/jent/seed */
	struct dentry *jent_debugfs_drng; /* /sys/kernel/debug/jent/drng */
	struct dentry *jent_debugfs_strong; /* /sys/kernel/debug/jent/strong */
	struct dentry *jent_debugfs_timer; /* /sys/kernel/debug/jent/timer */
	struct dentry *jent_debugfs_clc; /* /sys/kernel/debug/jent/collection_loop_count */
};
static struct jent_debugfs jent_debugfs;

static ssize_t jent_debugfs_stat_read(struct file *file, char __user *buf,
			      size_t nbytes, loff_t *ppos)
{
	char *out;
	struct jent_stat raw_stat;
	struct jent_stat reg_stat;
	struct jent_stat strong_stat;
	int ret = 0;

	jent_dbg_raw_get_stat(&raw_stat);
	jent_dbg_reg_get_stat(&reg_stat);
	jent_dbg_strong_get_stat(&strong_stat);

	out = kasprintf(GFP_KERNEL,
		 "Raw:     read %zu\n"
		 "Regular: read %zu - reseed %zu - rekey: %zu\n"
		 "Strong:  read %zu - reseed %zu - rekey: %zu\n",
		 raw_stat.read_data,
		 reg_stat.read_data, reg_stat.num_reseed, reg_stat.num_rekey,
		 strong_stat.read_data, strong_stat.num_reseed, strong_stat.num_rekey
		 );
	if (!out)
		return -ENOMEM;
	ret = simple_read_from_buffer(buf, nbytes, ppos, out, strlen(out));
	kfree(out);
	return ret;
}

static struct file_operations jent_stat_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_stat_read,
};

static ssize_t jent_debugfs_stattimer_read(struct file *file, char __user *buf,
					   size_t nbytes, loff_t *ppos)
{
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT
	char *out;
	ssize_t total = 0;
	loff_t pos = *ppos;
	struct entropy_stat stat;
	ssize_t len = 0;
	struct rand_data *ec = jent_entropy_collector_alloc(1, 0);
	if (!ec)
		return -ENOMEM;
	ec->entropy_stat.enable_bit_test = 0;

	while (0 < nbytes) {
		__u64 delta = -1;
		__u64 deltadelta = -1;

		/* get data */
		jent_gen_entropy_stat(ec, &stat);
		delta = stat.collection_end - stat.collection_begin;
		if (stat.old_delta > delta)
			deltadelta = stat.old_delta - delta;
		else
			deltadelta = delta - stat.old_delta;

		/* feed it to user space */
		out = kasprintf(GFP_KERNEL, "%llu %llu %u\n", delta, deltadelta,
				stat.collection_loop_cnt);
		if (!out) {
			len = -ENOMEM;
			break;
		}
		len = strlen(out);
		/* here we potentially discard the information from one
		 * generated round -- dd will cry, but we do not care */
		if (nbytes < len)
			break;
		if (copy_to_user(buf+pos+total, out, len)) {
			len = -EFAULT;
			break;
		}
		nbytes -= len;
		total += len;
		kfree(out);
	}
	jent_entropy_collector_free(ec);

	return ((0 > len) ? len : total);
#else
	return -ENXIO;
#endif
}

static struct file_operations jent_stattimer_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_stattimer_read,
};

static ssize_t jent_debugfs_statfold_read(struct file *file, char __user *buf,
					  size_t nbytes, loff_t *ppos)
{
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT
	char *out;
	ssize_t total = 0;
	loff_t pos = *ppos;
	ssize_t len = 0;
	struct rand_data *ec = NULL;

	ec = jent_entropy_collector_alloc(1, 0);
	if (NULL == ec)
		return -ENOMEM;

	while (0 < nbytes) {
		__u64 duration = 0;
		__u64 duration_min = 0;

		/* get data */
		duration = jent_fold_var_stat(ec, 0);
		duration_min = jent_fold_var_stat(ec, 1);

		/* feed it to user space */
		out = kasprintf(GFP_KERNEL, "%llu %llu\n", duration, duration_min);
		if (!out) {
			len = -ENOMEM;
			break;
		}
		len = strlen(out);
		/* here we potentially discard the information from one
		 * generated round -- dd will cry, but we do not care */
		if (nbytes < len)
			break;
		if (copy_to_user(buf+pos+total, out, len)) {
			len = -EFAULT;
			break;
		}
		nbytes -= len;
		total += len;
		kfree(out);
	}
	jent_entropy_collector_free(ec);
	return ((0 > len) ? len : total);
#else
	return -ENXIO;
#endif
}

static struct file_operations jent_statfold_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_statfold_read,
};

static ssize_t jent_debugfs_statbits_read(struct file *file, char __user *buf,
					  size_t nbytes, loff_t *ppos)
{
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT
	char *out;
	ssize_t total = 0;
	loff_t pos = *ppos;
	struct entropy_stat stat;
	ssize_t len = 0;
	struct rand_data *ec = jent_entropy_collector_alloc(1, 0);
	if (!ec)
		return -ENOMEM;
	ec->entropy_stat.enable_bit_test = 1;

	while (0 < nbytes) {
		/* get data */
		jent_gen_entropy_stat(ec, &stat);

		/* feed it to user space */
		out = kasprintf(GFP_KERNEL, "%u %u %u\n", stat.obsbits,
				stat.setbits, stat.varbits);
		if (!out) {
			len = -ENOMEM;
			break;
		}
		len = strlen(out);
		/* here we potentially discard the information from one
		 * generated round -- dd will cry, but we do not care */
		if (nbytes < len)
			break;
		if (copy_to_user(buf+pos+total, out, len)) {
			len = -EFAULT;
			break;
		}
		nbytes -= len;
		total += len;
		kfree(out);
	}
	jent_entropy_collector_free(ec);

	return ((0 > len) ? len : total);
#else
	return -ENXIO;
#endif
}

static struct file_operations jent_statbits_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_statbits_read,
};

static ssize_t jent_debugfs_read_func(struct file *file,
				      char __user *buf, size_t nbytes,
				      loff_t *ppos, size_t chunk,
				      int (*source)(char *out, size_t len))
{
	ssize_t total = 0;
	int ret = 0;
	loff_t pos = *ppos;
	char *out;

	if (!nbytes)
		return -EINVAL;

	/* chunk size shall be the block size in bytes of the RNG called with
	 * *source
	 * jent_read_entropy: 8
	 * jent_dbg_random_bytes: 16
	 * jent_dbg_strong_random_bytes: 16
	 */
	out = kzalloc(chunk, GFP_KERNEL);
	if (!out)
		return -ENOMEM;

	while (nbytes > 0) {
		int copy = min_t(int, chunk, nbytes);
		ret = source(out, copy);
		if (0 > ret) {
			printk(DRIVER_NAME": could not obtain random data: %d\n", ret);
			ret = -EAGAIN;
			break;
		}
		if (copy_to_user(buf+pos+total, out, ret)) {
			ret = -EFAULT;
			break;
		}
		nbytes -= ret;
		total += ret;
	}

	kzfree(out);

	return ((0 > ret) ? ret : total);
}

static ssize_t jent_debugfs_seed_read(struct file *file, char __user *buf,
			      size_t nbytes, loff_t *ppos)
{
	return jent_debugfs_read_func(file, buf, nbytes, ppos, 8,
				      jent_dbg_raw_bytes);
}

static struct file_operations jent_seed_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_seed_read,
};

static ssize_t jent_debugfs_drng_read(struct file *file,
				      char __user *buf, size_t nbytes,
				      loff_t *ppos)
{
	return jent_debugfs_read_func(file, buf, nbytes, ppos, 16,
				      jent_dbg_random_bytes);
}

static struct file_operations jent_drng_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_drng_read,
};

static ssize_t jent_debugfs_strong_read(struct file *file,
					char __user *buf,
					size_t nbytes, loff_t *ppos)
{
	return jent_debugfs_read_func(file, buf, nbytes, ppos, 16,
				      jent_dbg_strong_random_bytes);
}

static struct file_operations jent_strong_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_strong_read,
};

static ssize_t jent_debugfs_timer_read(struct file *file,
				       char __user *buf,
				       size_t nbytes, loff_t *ppos)
{
	char *out;
	ssize_t total = 0;
	loff_t pos = *ppos;
	size_t len = 0;

	while (0 < nbytes) {
		__u64 time, time2;
		/* get data */
		time = time2 = 0;
		time = random_get_entropy();
		time2 = random_get_entropy();

		/* feed it to user space */
		out = kasprintf(GFP_KERNEL, "%lld\n", (time2 - time));
		if (!out) {
			len = -ENOMEM;
			break;
		}
		len = strlen(out);
		/* here we potentially discard the information from one
		 * generated round -- dd will cry, but we do not care */
		if (nbytes < len)
			break;
		if (copy_to_user(buf+pos+total, out, len)) {
			len = -EFAULT;
			break;
		}
		nbytes -= len;
		total += len;
		kfree(out);
	}

	return ((0 > len) ? len : total);
}

static struct file_operations jent_timer_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_timer_read,
};

static ssize_t jent_debugfs_clc_read(struct file *file, char __user *buf,
				     size_t nbytes, loff_t *ppos)
{
	char out[12];
#ifdef CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT
	/* no lock needed -- statistic analyses should be single threaded;
	 * if not, it is the caller's fault if the value is not correct :-) */
	unsigned int collection_loop_cnt =
	       raw_entropy.entropy_collector->entropy_stat.collection_loop_cnt;
#else
	unsigned int collection_loop_cnt = -1;
#endif /* CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT */

	memset(out, 0, sizeof(out));
	snprintf(out, sizeof(out), "%u\n", collection_loop_cnt);
	return simple_read_from_buffer(buf, nbytes, ppos, out, sizeof(out));
}

#if 0
static ssize_t jent_debugfs_clc_write(struct file *file, const char __user *buf,
				      size_t nbytes, loff_t *ppos)
{
	unsigned long collection_loop_cnt = 0;
	char in[11];
	int ret = 0;

	ret = simple_write_to_buffer(in, nbytes, ppos, buf, sizeof(in));
	if (0 >= ret)
		return ret;
	collection_loop_cnt = simple_strtoul(in, NULL, 10);
	if (0 >= collection_loop_cnt || 1<<20 < collection_loop_cnt)
		return -EINVAL;
	else
		jent_set_loop_cnt((unsigned int)collection_loop_cnt);
	return ret;
}
#endif

static struct file_operations jent_clc_fops = {
	.owner = THIS_MODULE,
	.read = jent_debugfs_clc_read,
/*	.write = jent_debugfs_clc_write, */
};

/****************************************************************
 * initialization of debugfs interfaces
 ****************************************************************/

int __init jent_dbg_init(void)
{
	int ret = -EINVAL;
	jent_debugfs.jent_debugfs_stat = NULL;
	jent_debugfs.jent_debugfs_stattimer = NULL;
	jent_debugfs.jent_debugfs_statfold = NULL;
	jent_debugfs.jent_debugfs_statbits = NULL;
	jent_debugfs.jent_debugfs_seed = NULL;
	jent_debugfs.jent_debugfs_drng = NULL;
	jent_debugfs.jent_debugfs_strong = NULL;
	jent_debugfs.jent_debugfs_timer = NULL;
	jent_debugfs.jent_debugfs_clc = NULL;
	jent_debugfs.jent_debugfs_root = NULL;

	/* initialize the raw entropy **************/
	ret = jent_drng_init_raw(&raw_entropy, JENT_DISABLE_STIR);
	if (ret) {
		printk(DRIVER_NAME": Raw entropy collector instantiation failed\n");
		return ret;
	}
	printk(DRIVER_NAME": Raw entropy collector instantiated\n");

	/* initialize the DRNGs **************/
	ret = jent_drng_init_drng(&jent_reg_drng);
	if (ret) {
		printk(DRIVER_NAME": Regular DRNG instantiation failed\n");
		goto cleanraw;
	}
	printk(DRIVER_NAME": Regular DRNG instantiated\n");
	ret = jent_drng_init_drng(&jent_strong_drng);
	if (ret) {
		printk(DRIVER_NAME": Strong DRNG instantiation failed\n");
		goto cleandrng;
	}
	printk(DRIVER_NAME": Strong DRNG instantiated\n");

	/* instantiate the debugfs interfaces */
	jent_debugfs.jent_debugfs_root =
		debugfs_create_dir(DRIVER_NAME, NULL);
	if (IS_ERR(jent_debugfs.jent_debugfs_root)) {
		printk(DRIVER_NAME": initialization of debugfs directory failed\n");
		goto cleanstrongdrng;
	}
	jent_debugfs.jent_debugfs_stat =
		debugfs_create_file("stat", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_stat_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_stat)) {
		printk(DRIVER_NAME": initialization of stat file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_stattimer =
		debugfs_create_file("stat-timer", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_stattimer_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_stattimer)) {
		printk(DRIVER_NAME": initialization of stat-timer file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_statfold =
		debugfs_create_file("stat-fold", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_statfold_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_statfold)) {
		printk(DRIVER_NAME": initialization of stat-fold file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_statbits =
		debugfs_create_file("stat-bits", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_statbits_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_statbits)) {
		printk(DRIVER_NAME": initialization of stat-bits file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_seed =
		debugfs_create_file("seed", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_seed_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_seed)) {
		printk(DRIVER_NAME": initialization of seed file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_drng =
		debugfs_create_file("drng", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_drng_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_drng)) {
		printk(DRIVER_NAME": initialization of drng file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_strong =
		debugfs_create_file("strong-drng", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_strong_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_strong)) {
		printk(DRIVER_NAME": initialization of strong-drng file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_timer =
		debugfs_create_file("timer", S_IRUGO,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_timer_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_timer)) {
		printk(DRIVER_NAME": initialization of timer file failed\n");
		goto cleandir;
	}
	jent_debugfs.jent_debugfs_clc =
		debugfs_create_file("collection_loop_count", S_IRUGO|S_IWUSR,
		jent_debugfs.jent_debugfs_root,
		NULL, &jent_clc_fops);
	if (IS_ERR(jent_debugfs.jent_debugfs_clc)) {
		printk(DRIVER_NAME": initialization of collection_loop_count file failed\n");
		goto cleandir;
	}
	return 0;

cleandir:
	debugfs_remove_recursive(jent_debugfs.jent_debugfs_root);
cleanstrongdrng:
	jent_drng_cleanup_drng(&jent_strong_drng);
cleandrng:
	jent_drng_cleanup_drng(&jent_reg_drng);
cleanraw:
	jent_drng_cleanup_raw(&raw_entropy);

	return ret;
}

/* we potentially call the exit function within an __init function */
void /*__exit*/ jent_dbg_exit(void)
{
	debugfs_remove_recursive(jent_debugfs.jent_debugfs_root);
	jent_drng_cleanup_drng(&jent_reg_drng);
	jent_drng_cleanup_drng(&jent_strong_drng);
	jent_drng_cleanup_raw(&raw_entropy);
}
