include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

LIBS += binoic_libc_common
LIBS += bionic_libdl
#LIBS += bionic_libc_x86

CC_OPT := $(libc_common_cflags) \
	-fPIC \
	
CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)


SRC_CC += \
    $(libc_arch_dynamic_src_files) \
    $(libc_static_common_src_files) \
    bionic/libc_init_dynamic.cpp \
    bionic/NetdClient.cpp \
    bionic/debug_mapinfo.cpp \
    bionic/malloc_debug_common.cpp \
    bionic/debug_stacktrace.cpp \
 
LOCAL_LDFLAGS_x86_64 := -Wl,--exclude-libs,libgcc.a
LD_OPT += $(LOCAL_LDFLAGS_X86_64)    
SHARED_LIB =yes

vpath %.cpp $(LOCAL_PATH)/
# ========================================================
# libc.so
# ========================================================

#LOCAL_MODULE := libc
#LOCAL_CLANG := $(use_clang)
#LOCAL_ADDITIONAL_DEPENDENCIES := $(libc_common_additional_dependencies)


#LOCAL_SHARED_LIBRARIES := libdl
#LOCAL_WHOLE_STATIC_LIBRARIES := libc_common
#LOCAL_SYSTEM_SHARED_LIBRARIES :=

#LOCAL_LDFLAGS_arm64 := -Wl,--exclude-libs,libgcc.a
#LOCAL_LDFLAGS_mips64 := -Wl,--exclude-libs,libgcc.a
#LOCAL_LDFLAGS_x86_64 := -Wl,--exclude-libs,libgcc.a

#$(eval $(call patch-up-arch-specific-flags,LOCAL_CFLAGS,libc_common_cflags))
#$(eval $(call patch-up-arch-specific-flags,LOCAL_SRC_FILES,libc_arch_dynamic_src_files))
#$(eval $(call patch-up-arch-specific-flags,LOCAL_SRC_FILES,libc_static_common_src_files))
# special for arm
#LOCAL_NO_CRT_arm := true
#LOCAL_CFLAGS_arm += -DCRT_LEGACY_WORKAROUND
#LOCAL_SRC_FILES_arm += \
#    arch-common/bionic/crtbegin_so.c \
#    arch-arm/bionic/atexit_legacy.c \
#    arch-common/bionic/crtend_so.S

#include $(BUILD_SHARED_LIBRARY)

