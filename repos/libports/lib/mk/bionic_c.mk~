include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

CC_OPT = -fPIC -nostdinc



#LIBS += bionic_stack_protector
#LIBS += bionic_libc_tzcode
#LIBS += bionic_libc_dns
#LIBS += bionic_libc_freebsd 
#LIBS += bionic_libc_netbsd
#LIBS += bionic_libc_openbsd
#LIBS += bionic_libc_gdtoa
#LIBS += bionic_libc_bionic
#LIBS += bionic_libc_syscalls
LIBS += bionic_libc-aeabi

#LIBS += bionic_libc_nomalloc
LIBS += bionic_libc_malloc
#LIBS += bionic_libc_libc
LIBS += bionic_libc_libc_so
#LIBS += bionic_libc_malloc_debug_leak
LIBS += bionic_libc_malloc_debug_qemu
LIBS += bionic_libdl
LIBS += binoic_libc_common




#LIBS += bionic_libc_libc_so
#LIBS += bionic_libc
# 
#

#
#
LIBS += bionic_libstdc++_a
LIBS += bionic_libstdc++_so


#LIBS += bionic_libc_x86
#LIBS += bionic_libm_a
LIBS += bionic_libm_so
 #LIBS += bionic_libc_openbsd
#
#
#
#LIBS += bionic_benchmarks

SRC_C += paramesh_added_files/unwind.c \

SRC_CC = \
    bionic/debug_mapinfo.cpp \
    bionic/debug_stacktrace.cpp \
    bionic/libc_init_dynamic.cpp \
    bionic/NetdClient.cpp \
    bionic/malloc_debug_common.cpp \
    
    
vpath %.cpp $(LOCAL_PATH)
vpath %.c $(BIONIC_DIR)/
SHARED_LIB = yes
