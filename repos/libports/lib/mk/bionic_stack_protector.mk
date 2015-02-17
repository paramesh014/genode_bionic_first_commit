include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

#LIBS += bionic_libc_malloc_debug_qemu
#LIBS += bionic_libc_bionic

SRC_CC = $(LOCAL_PATH)/bionic/__stack_chk_fail.cpp
CC_OPT = $(libc_common_cflags) -fno-stack-protector -fPIC
CC_C_OPT = $(libc_common_conlyflags)
CC_CXX_OPT = $(libc_common_cppflags)

#SHARED_LIB =yes
