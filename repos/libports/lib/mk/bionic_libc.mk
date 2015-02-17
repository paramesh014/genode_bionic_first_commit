include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

#LIBS += bionic_libc_x86
LIBS := bionic_libc_tzcode

CC_OPT := $(libc_common_cflags)
CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)


SRC_CC = \
    $(libc_arch_dynamic_src_files) \
    $(libc_static_common_src_files) \
    $(LOCAL_PATH)/bionic/malloc_debug_common.cpp \
    $(LOCAL_PATH)/bionic/debug_mapinfo.cpp \
    $(LOCAL_PATH)/bionic/debug_stacktrace.cpp \
    $(LOCAL_PATH)/bionic/libc_init_dynamic.cpp \
    $(LOCAL_PATH)/bionic/NetdClient.cpp \

# ========================================================
# libc.so
# ========================================================

LOCAL_MODULE := libc
LOCAL_CLANG := $(use_clang)
LOCAL_ADDITIONAL_DEPENDENCIES := $(libc_common_additional_dependencies)


# Leave the symbols in the shared library so that stack unwinders can produce
# meaningful name resolution.
LOCAL_STRIP_MODULE := keep_symbols

# WARNING: The only library libc.so should depend on is libdl.so!  If you add other libraries,
# make sure to add -Wl,--exclude-libs=libgcc.a to the LOCAL_LDFLAGS for those libraries.  This
# ensures that symbols that are pulled into those new libraries from libgcc.a are not declared
# external; if that were the case, then libc would not pull those symbols from libgcc.a as it
# should, instead relying on the external symbols from the dependent libraries.  That would
# create a "cloaked" dependency on libgcc.a in libc though the libraries, which is not what
# you wanted!

LOCAL_SHARED_LIBRARIES := libdl
LOCAL_WHOLE_STATIC_LIBRARIES := libc_common
LOCAL_SYSTEM_SHARED_LIBRARIES :=

# We'd really like to do this for all architectures, but since this wasn't done
# before, these symbols must continue to be exported on LP32 for binary
# compatibility.
LOCAL_LDFLAGS_arm64 := -Wl,--exclude-libs,libgcc.a
LOCAL_LDFLAGS_mips64 := -Wl,--exclude-libs,libgcc.a
LOCAL_LDFLAGS_x86_64 := -Wl,--exclude-libs,libgcc.a

$(eval $(call patch-up-arch-specific-flags,LOCAL_CFLAGS,libc_common_cflags))
$(eval $(call patch-up-arch-specific-flags,LOCAL_SRC_FILES,libc_arch_dynamic_src_files))
$(eval $(call patch-up-arch-specific-flags,LOCAL_SRC_FILES,libc_static_common_src_files))
# special for arm
LOCAL_NO_CRT_arm := true
LOCAL_CFLAGS_arm += -DCRT_LEGACY_WORKAROUND
LOCAL_SRC_FILES_arm += \
    arch-common/bionic/crtbegin_so.c \
    arch-arm/bionic/atexit_legacy.c \
    arch-common/bionic/crtend_so.S

include $(BUILD_SHARED_LIBRARY)

