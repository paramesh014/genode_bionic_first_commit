include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

CC_OPT := \
      -Wno-sign-compare -Wno-uninitialized \
    -fvisibility=hidden \
    -I$(LOCAL_PATH)/private \
    -I$(LOCAL_PATH)/upstream-openbsd/android/include \
    -I$(LOCAL_PATH)/upstream-openbsd/lib/libc/include \
    -include openbsd-compat.h \
    -fPIC \
     $(libc_common_cflags) \

CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)

SRC_C = $(libc_upstream_openbsd_gdtoa_src_files_64) 
SRC_C += $(libc_upstream_openbsd_gdtoa_src_files_32)
SRC_CC = $(libc_upstream_openbsd_gdtoa_cpp_src_files)

#SHARED_LIB = yes
