ifeq ($(APP_SCHEDULER), yes)
SRCS += $(WP_LIB_PATH)app_scheduler.c
# Generic util functions are needed by app scheduler
include $(UTIL_PATH)makefile
endif

ifeq ($(TLV), yes)
SRCS += $(WP_LIB_PATH)tlv.c
endif
