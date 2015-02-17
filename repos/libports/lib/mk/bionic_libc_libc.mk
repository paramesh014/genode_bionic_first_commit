include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

LIBS += binoic_libc_common

SRC_CC := \
    $(libc_arch_static_src_files) \
    $(libc_static_common_src_files) \
    bionic/libc_init_static.cpp \
    bionic/malloc_debug_common.cpp \
    # have pointer to android/external/jemalloc/jemalloc.h

CC_OPT := $(libc_common_cflags) \
    -DLIBC_STATIC \
    -fPIC

CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)
LOCAL_C_INCLUDES := $(libc_common_c_includes)

#LOCAL_MODULE := libc
#LOCAL_CLANG := $(use_clang)
#LOCAL_ADDITIONAL_DEPENDENCIES := $(libc_common_additional_dependencies)
#LOCAL_WHOLE_STATIC_LIBRARIES := libc_common

$(eval $(call patch-up-arch-specific-flags,LOCAL_CFLAGS,libc_common_cflags))
$(eval $(call patch-up-arch-specific-flags,LOCAL_SRC_FILES,libc_arch_static_src_files))

vpath %.cpp $(LOCAL_PATH)/
