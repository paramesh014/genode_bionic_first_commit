include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc
LOCAL_PATH_DL = $(BIONIC_DIR)/libdl
#
# libdl
#

LOCAL_LDFLAGS := -Wl,--exclude-libs=libgcc.a

# for x86, exclude libgcc_eh.a for the same reasons as above
LOCAL_LDFLAGS_x86 := -Wl,--exclude-libs=libgcc_eh.a
LOCAL_LDFLAGS_x86_64 := $(LOCAL_LDFLAGS_x86)

#LOCAL_SRC_FILES:= libdl.c
CC_OPT := -Wall -Wextra -Wunused -Werror


# NOTE: libdl needs __aeabi_unwind_cpp_pr0 from libgcc.a but libgcc.a needs a
# few symbols from libc. Using --no-undefined here results in having to link
# against libc creating a circular dependency which is removed and we end up
# with missing symbols. Since this library is just a bunch of stubs, we set
# LOCAL_ALLOW_UNDEFINED_SYMBOLS to remove --no-undefined from the linker flags.
LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_SYSTEM_SHARED_LIBRARIES :=
CC_C_OPT = $(LOCAL_CFLAGS)
SRC_C = $(LOCAL_PATH_DL)/libdl.c

SHARED_LIB  = yes
