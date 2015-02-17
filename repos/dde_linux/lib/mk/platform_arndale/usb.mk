SRC_C   += usbnet.c asix_devices.c asix_common.c ax88172a.c ax88179_178a.c ehci-exynos.c

include $(REP_DIR)/lib/mk/xhci.inc
include $(REP_DIR)/lib/mk/usb.inc
include $(REP_DIR)/lib/mk/armv7/usb.inc

CC_OPT  += -DCONFIG_USB_EHCI_TT_NEWSCHED -DCONFIG_USB_DWC3_HOST=1 \
           -DCONFIG_USB_DWC3_GADGET=0 -DCONFIG_USB_OTG_UTILS -DCONFIG_USB_XHCI_PLATFORM -DDWC3_QUIRK
INC_DIR += $(LX_CONTRIB_DIR)/arch/arm/plat-samsung/include
SRC_CC  += platform.cc

#DWC3
SRC_C   += host.c core.c

#XHCI
SRC_C   += xhci-plat.c

vpath platform.cc $(LIB_DIR)/arm/platform_arndale
vpath %.c         $(LX_CONTRIB_DIR)/drivers/usb/dwc3
vpath %.c         $(LX_CONTRIB_DIR)/drivers/net/usb
