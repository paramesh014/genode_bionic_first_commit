include $(REP_DIR)/lib/mk/bionic_libc_src.inc
include $(REP_DIR)/lib/mk/bionic_libc_includes.inc

INC_DIR += $(LOCAL_PATH)/dns/include

DNS_SRC_FILES := \
    upstream-netbsd/lib/libc/isc/ev_streams.c \
    upstream-netbsd/lib/libc/isc/ev_timers.c \

DNS_SRC_FILES += \
	ns_name.c  ns_netint.c  ns_parse.c  ns_print.c  ns_samedomain.c  ns_ttl.c \
	

DNS_SRC_FILES += \
	base64.c  getaddrinfo.c  getnameinfo.c  getservbyname.c  getservbyport.c  getservent.c  nsdispatch.c \

DNS_SRC_FILES += \
	__dn_comp.c  res_cache.c    res_comp.c  res_debug.c  res_init.c   __res_send.c  res_state.c \
	herror.c    __res_close.c  res_data.c res_mkquery.c  res_query.c    res_send.c \
	
DNS_SRC_FILES += \
	gethnamaddr.c

CC_OPT = \
     $(libc_common_cflags) \
    -DANDROID_CHANGES \
    -DINET6 \
    -fvisibility=hidden \
    -Wno-unused-parameter \
    -I$(LOCAL_PATH)/dns/include \
    -I$(LOCAL_PATH)/private \
    -I$(LOCAL_PATH)/upstream-netbsd/lib/libc/include \
    -I$(LOCAL_PATH)/upstream-netbsd/android/include \
    -include netbsd-compat.h \
    -fPIC

CC_C_OPT := $(libc_common_conlyflags)
CC_CXX_OPT := $(libc_common_cppflags)

SRC_C += $(DNS_SRC_FILES)


vpath %.c $(LOCAL_PATH)/dns/nameser/
vpath %.c $(LOCAL_PATH)/dns/net/
vpath %.c $(LOCAL_PATH)/dns/resolv/
vpath %.c $(LOCAL_PATH)/dns/
vpath %.c $(LOCAL_PATH)/


#LOCAL_MODULE := libc_dns
#LOCAL_CLANG := $(use_clang)

#SHARED_LIB = yes

