# Compile Noise Source as Linux kernel module
# Remove the compiler flags if the make file is integrated with the kernel
# build system
#ccflags-y += -DCONFIG_CRYPTO_CPU_JITTERENTROPY_DBG -DCRYPTO_CPU_JITTERENTROPY_KCAPI

# This enables the readout of the collection loop counter
ccflags-y += -DCONFIG_CRYPTO_CPU_JITTERENTROPY_DBG -DCONFIG_CRYPTO_CPU_JITTERENTROPY_KCAPI -DCONFIG_CRYPTO_CPU_JITTERENTROPY_STAT

obj-m += jitterentropy-rng.o
#ifeq ($(CONFIG_CRYPTO_CPU_JITTERENTROPY),y)
CFLAGS_jitterentropy-base.o = -O0
jitterentropy-rng-y := jitterentropy-base.o 
#endif
#ifeq ($(CONFIG_CRYPTO_CPU_JITTERENTROPY_DRNG),y)
jitterentropy-rng-y += jitterentropy-drng.o
#endif
#ifeq ($(CONFIG_CRYPTO_CPU_JITTERENTROPY_DBG),y)
jitterentropy-rng-y += jitterentropy-dbg.o
#endif
#ifeq ($(CONFIG_CRYPTO_CPU_JITTERENTROPY_KCAPI),y)
jitterentropy-rng-y += jitterentropy-kcapi.o
#endif
#ifeq ($(CONFIG_CRYPTO_CPU_JITTERENTROPY_STAT),y)
jitterentropy-rng-y += jitterentropy-stat.o
#endif


all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/sm/hacking/sources/jitterentropy/jitterentropy/Linux-kernel modules
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/sm/hacking/sources/jitterentropy/jitterentropy/Linux-kernel clean
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean