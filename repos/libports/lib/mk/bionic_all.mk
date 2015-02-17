include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

CC_OPT := $(libc_common_cflags) \
            -fno-builtin\
            -Wframe-larger-than=2048 \
            
CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)


SRC_CC = \
	$(libc_arch_dynamic_src_files) \
    $(libc_static_common_src_files) \
    $(LOCAL_PATH)/bionic/malloc_debug_common.cpp \
    $(LOCAL_PATH)/bionic/debug_mapinfo.cpp \
    $(LOCAL_PATH)/bionic/debug_stacktrace.cpp \
    $(LOCAL_PATH)/bionic/libc_init_dynamic.cpp \
    $(LOCAL_PATH)/bionic/NetdClient.cpp \
    $(libc_bionic_cpp_src_files) \

SRC_C = \
    $(BIONIC_DIR)/libc/arch-arm/bionic/__aeabi.c \
    $(libc_bionic_src_files) \


DNS_SRC_FILES := \
    $(LOCAL_PATH)/upstream-netbsd/lib/libc/isc/ev_streams.c \
    $(LOCAL_PATH)/upstream-netbsd/lib/libc/isc/ev_timers.c \

DNS_SRC_FILES += \
	ns_name.c  ns_netint.c  ns_parse.c  ns_print.c  ns_samedomain.c  ns_ttl.c \
	

DNS_SRC_FILES += \
	base64.c  getaddrinfo.c  getnameinfo.c  getservbyname.c  getservbyport.c  getservent.c  nsdispatch.c \

DNS_SRC_FILES += \
	__dn_comp.c  res_cache.c    res_comp.c  res_debug.c  res_init.c   __res_send.c  res_state.c \
	herror.c    __res_close.c  res_data.c res_mkquery.c  res_query.c    res_send.c 

DNS_SRC_FILES_CPP += $(LOCAL_PATH)/bionic/NetdClientDispatch.cpp \
	$(LOCAL_PATH)/bionic/libc_logging.cpp

SRC_C += $(DNS_SRC_FILES) \
	$(libc_upstream_freebsd_src_files)
	
SRC_CC += $(DNS_SRC_FILES_CPP)

vpath %.c $(LOCAL_PATH)/dns/nameser/
vpath %.c $(LOCAL_PATH)/dns/net/
vpath %.c $(LOCAL_PATH)/dns/resolv/





SHARED_LIB = yes
