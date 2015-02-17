include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

LIBS += bionic_libdl
LIBS += bionic_libc_libc_so

CC_OPT := \
    $(libc_common_cflags) \
    -DMALLOC_QEMU_INSTRUMENT \
    -fPIC

CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)


SRC_CC := \
    bionic/libc_logging.cpp \
    bionic/malloc_debug_qemu.cpp \

SHARED_LIB =yes

vpath %.cpp $(LOCAL_PATH)/
#LOCAL_CLANG := $(use_clang)

# Don't install on release build
#LOCAL_MODULE_TAGS := eng debug

