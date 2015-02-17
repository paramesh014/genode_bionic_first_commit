include $(REP_DIR)/lib/mk/bionic_libm_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

LIBS += bionic_libc_libc_so
LIBS += bionic_libm_a
SHARED_LIB =yes


#LOCAL_MODULE:= libm
#LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
#LOCAL_SYSTEM_SHARED_LIBRARIES := libc
#LOCAL_WHOLE_STATIC_LIBRARIES := libm

# We'd really like to do this for all architectures, but since this wasn't done
# before, these symbols must continue to be exported on LP32 for binary
# compatibility.
LOCAL_LDFLAGS_arm64 := -Wl,--exclude-libs,libgcc.a
LOCAL_LDFLAGS_mips64 := -Wl,--exclude-libs,libgcc.a
LOCAL_LDFLAGS_x86_64 := -Wl,--exclude-libs,libgcc.a

