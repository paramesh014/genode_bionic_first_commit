include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

LIBS := \
    bionic_libc_bionic \
     bionic_libc_dns \
     bionic_libc_freebsd \
     bionic_libc_gdtoa \
     bionic_libc_netbsd \
     bionic_libc_openbsd \
     bionic_stack_protector \
     bionic_libc_syscalls \
     bionic_libc_tzcode \
     bionic_libc_malloc \

CC_OPT = $(libc_common_cflags) -fPIC
#CC_OPT = -fPIC
CC_C_OPT = $(libc_common_conlyflags)
CC_CXX_OPT = $(libc_common_cppflags)

SRC_C += $(libc_common_src_files)
SRC_CC +=$(libc_common_cpp_src_files)

#LOCAL_CLANG := $(use_clang)

LOCAL_WHOLE_STATIC_LIBRARIES_arm := libc_aeabi

ifneq ($(MALLOC_IMPL),dlmalloc)
LOCAL_WHOLE_STATIC_LIBRARIES += libjemalloc
endif

LOCAL_SYSTEM_SHARED_LIBRARIES :=

# TODO: split out the asflags.
LOCAL_ASFLAGS := $(LOCAL_CFLAGS)

$(eval $(call patch-up-arch-specific-flags,LOCAL_CFLAGS,libc_common_cflags))
$(eval $(call patch-up-arch-specific-flags,LOCAL_SRC_FILES,libc_common_src_files))
$(eval $(call patch-up-arch-specific-flags,LOCAL_ASFLAGS,LOCAL_CFLAGS))
include $(BUILD_STATIC_LIBRARY)

#SHARED_LIB=yes
