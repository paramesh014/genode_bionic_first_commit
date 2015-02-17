include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc
include $(REP_DIR)/lib/mk/bionic_libc_syscalls_x86_64.inc

LIBS += bionic_libc_x86

SRC_C += $(libc_bionic_src_files)
SRC_CC += $(libc_bionic_cpp_src_files)

CC_OPT := $(libc_common_cflags) \
    -Wframe-larger-than=2048 \
    -fPIC 

CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)


#SHARED_LIB = yes



