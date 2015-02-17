include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

LIBS += bionic_libdl
LIBS += bionic_libc_libc_so
#LIBS += binoic_libc_common

CC_OPT := \
    $(libc_common_cflags) \
    -DMALLOC_LEAK_CHECK \
    -fPIC

CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)


SRC_CC = \
    bionic/libc_logging.cpp \
    bionic/debug_mapinfo.cpp \
    bionic/malloc_debug_check.cpp \
    bionic/malloc_debug_leak.cpp \
    bionic/debug_stacktrace.cpp \


SHARED_LIB =yes

vpath %.cpp $(LOCAL_PATH)/
#LOCAL_MODULE := libc_malloc_debug_leak
#LOCAL_CLANG := $(use_clang)

#LOCAL_SHARED_LIBRARIES := libc libdl
#LOCAL_SYSTEM_SHARED_LIBRARIES :=
#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

# Don't install on release build
#LOCAL_MODULE_TAGS := eng debug



