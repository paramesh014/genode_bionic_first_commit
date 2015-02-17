BIONIC_PORT_DIR := $(call select_from_ports,bionic)
BIONIC_DIR      := $(BIONIC_PORT_DIR)/src/lib/bionic
LOCAL_PATH     := $(BIONIC_DIR)/libc

# x86_64 specific configs


libc_common_src_files_x86_64 := \
    bionic/memchr.c \
    bionic/memrchr.c \
    bionic/strnlen.c \
    upstream-freebsd/lib/libc/string/wcscat.c \
    upstream-freebsd/lib/libc/string/wcschr.c \
    upstream-freebsd/lib/libc/string/wcscmp.c \
    upstream-freebsd/lib/libc/string/wcscpy.c \
    upstream-freebsd/lib/libc/string/wcslen.c \
    upstream-freebsd/lib/libc/string/wcsrchr.c \
    upstream-freebsd/lib/libc/string/wmemcmp.c \
    upstream-freebsd/lib/libc/string/wmemmove.c \
    upstream-openbsd/lib/libc/string/strlcat.c \
    upstream-openbsd/lib/libc/string/strlcpy.c \
    


      

# Fortify implementations of libc functions.
libc_common_src_files_x86_64_CPP += \
    bionic/strchr.cpp \
    bionic/strrchr.cpp \
    bionic/__memcpy_chk.cpp \
    bionic/__memset_chk.cpp \
    bionic/__strcpy_chk.cpp \
    bionic/__strcat_chk.cpp \



##########################################
### CPU specific source files
libc_bionic_src_files_x86_64 := \
    arch-x86_64/bionic/__bionic_clone.S \
    arch-x86_64/bionic/_exit_with_stack_teardown.S \
    arch-x86_64/bionic/__get_sp.S \
    arch-x86_64/bionic/__rt_sigreturn.S \
    arch-x86_64/bionic/syscall.S \
    arch-x86_64/bionic/_setjmp.S \
    arch-x86_64/bionic/setjmp.S \
    arch-x86_64/bionic/sigsetjmp.S \
    
libc_bionic_src_files_x86_64_c :=  \
	    arch-x86_64/bionic/__set_tls.c \

libc_bionic_src_files_x86_64 += \
    arch-x86_64/string/sse2-memcpy-slm.S \
    arch-x86_64/string/sse2-memmove-slm.S \
    arch-x86_64/string/sse2-memset-slm.S \
    arch-x86_64/string/sse2-stpcpy-slm.S \
    arch-x86_64/string/sse2-stpncpy-slm.S \
    arch-x86_64/string/sse2-strcat-slm.S \
    arch-x86_64/string/sse2-strcpy-slm.S \
    arch-x86_64/string/sse2-strlen-slm.S \
    arch-x86_64/string/sse2-strncat-slm.S \
    arch-x86_64/string/sse2-strncpy-slm.S \
    arch-x86_64/string/sse4-memcmp-slm.S \
    arch-x86_64/string/ssse3-strcmp-slm.S \
    arch-x86_64/string/ssse3-strncmp-slm.S \
    
    
   
CC_OPT = -fPIC
vpath %.cpp $(LOCAL_PATH)/
vpath %.c $(LOCAL_PATH)/
vpath %.S $(LOCAL_PATH)/
