include $(REP_DIR)/lib/mk/bionic_libm_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc
LIBS += bionic_libc_libc_so

CC_OPT = $(libm_common_cflags)
SRC_C = $(libm_common_src_files)
SRC_C += $(LOCAL_PATH)/i387/fenv.c $(libm_ld_src_files)
#LOCAL_SYSTEM_SHARED_LIBRARIES := libc

#SHARED_LIB =yes
