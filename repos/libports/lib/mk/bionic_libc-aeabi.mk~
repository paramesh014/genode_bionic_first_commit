include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

#LIBS += bionic_libc_x86

CC_OPT := $(libc_common_cflags) -fno-builtin -fPIC

SRC_C = arch-arm/bionic/__aeabi.c

#SHARED_LIB = yes
vpath %.c $(LOCAL_PATH)
