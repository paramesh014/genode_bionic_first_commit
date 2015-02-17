TARGET   = cli_monitor
SRC_CC   = main.cc
LIBS     = base cli_monitor config
INC_DIR += $(PRG_DIR)

ifeq ($(findstring arm, $(SPECS)), arm)
INC_DIR += $(PRG_DIR)/arm
else
ifeq ($(findstring x86, $(SPECS)), x86)
INC_DIR += $(PRG_DIR)/x86
endif
endif
