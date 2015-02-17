include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

#LIBS += bionic_libc_x86 
#LIBS += bionic_libc_freebsd



LOCAL_CFLAGS := \
    -Wno-sign-compare \
    -Wno-uninitialized \
    -Wno-unused-parameter \
    -I$(LOCAL_PATH)/private \
    -I$(LOCAL_PATH)/upstream-openbsd/android/include \
    -I$(LOCAL_PATH)/upstream-openbsd/lib/libc/include \
    -I$(LOCAL_PATH)/upstream-openbsd/lib/libc/gdtoa/ \
    -include openbsd-compat.h \
    -fPIC \
    $(libc_common_cflags) \

LOCAL_CONLYFLAGS := $(libc_common_conlyflags)
LOCAL_CPPFLAGS := $(libc_common_cppflags)
    
CC_OPT = $(LOCAL_CFLAGS)
CC_C_OPT = $(LOCAL_CONLYFLAGS)
CC_CXX_OPT = $(LOCAL_CPPFLAGS)


SRC_C = $(libc_upstream_openbsd_src_files)

#SHARED_LIB = yes

