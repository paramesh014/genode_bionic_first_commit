BIONIC_PORT_DIR := $(call select_from_ports,bionic)
BIONIC_LIBC_DIR := $(BIONIC_PORT_DIR)/src/lib/bionic/libc


include $(REP_DIR)/lib/mk/bionic_libc_includes.inc
include $(REP_DIR)/lib/mk/bionic_libc_syscalls_x86_64.inc
#LIBS += bionic_libc_malloc_debug_leak
#LIBS += bionic_libc_libc_so


CC_OPT = -fPIC 

SRC_CC += $(libc_common_src_files_x86_64_CPP)  
SRC_C += $(libc_common_src_files_x86_64) 
SRC_C += $(libc_bionic_src_files_x86_64_c)
SRC_S += $(libc_bionic_src_files_x86_64)


#SHARED_LIB = yes 

