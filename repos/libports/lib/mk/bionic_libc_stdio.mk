include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc
LIBC_STDIO_DIR = $(BIONIC_DIR)/libc/stdio

SRC_C = \
	upstream-freebsd/lib/libc/stdio/sprintf.c \
	upstream-freebsd/lib/libc/stdio/snprintf.c \
	upstream-freebsd/lib/libc/stdio/fclose.c \
	upstream-freebsd/lib/libc/stdio/fopen.c \
	upstream-freebsd/lib/libc/stdio/flags.c \

#SRC_C += 


vpath %.c $(LIBC_STDIO_DIR)
vpath %.c $(LOCAL_PATH)/
