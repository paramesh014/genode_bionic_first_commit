#
# \brief  Build configurations for 'base-hw' on Freescale i.MX53
# \author Stefan Kalkowski
# \date   2012-10-24
#

# denote wich specs are also fullfilled by this spec
SPECS += hw platform_imx53 epit

# configure multiprocessor mode
PROCESSORS = 1

# set address where to link the text segment at
LD_TEXT_ADDR ?= 0x70010000

# include implied specs
include $(call select_from_repositories,mk/spec-hw.mk)
include $(call select_from_repositories,mk/spec-platform_imx53.mk)
