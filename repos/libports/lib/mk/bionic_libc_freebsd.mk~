include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

#LIB += bionic_libc_openbsd

FREEBSD_CFLAGS := \
    $(libc_common_cflags) \
    -Wno-sign-compare -Wno-uninitialized \
    -I$(LOCAL_PATH)/upstream-freebsd/android/include \
    -I$(LOCAL_PATH)/upstream-freebsd/lib/libc/include \
    -include freebsd-compat.h \
    -fPIC
    
FREEBSD_CONLYFLAGS := $(libc_common_conlyflags)
FREEBSD_CPPFLAGS := $(libc_common_cppflags)

CC_OPT = $(FREEBSD_CFLAGS)
CC_C_OPT = $(FREEBSD_CONLYFLAGS)
CC_CXX_OPT = $(FREEBSD_CPPFLAGS)
SRC_C = $(libc_upstream_freebsd_src_files)

#SHARED_LIB = yes

#LOCAL_CLANG := $(use_clang)

