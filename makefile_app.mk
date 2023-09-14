include makefile_common.mk

.DEFAULT_GOAL := all

APP_BUILDPREFIX := $(BUILDPREFIX)$(APP_NAME)/
# Linker script
LDSCRIPT = mcu/$(MCU)/linker/gcc_app_$(MCU)$(MCU_SUB).ld

LIBS :=

# Include app specific config
-include $(APP_SRCS_PATH)config.mk

ifeq ($(filter $(TARGET_BOARDS),$(target_board)),)
 $(error Board $(target_board) is not supported board list: ($(TARGET_BOARDS)))
else
 $(info Building app for $(target_board))
endif

# App different formats
APP_ELF := $(APP_BUILDPREFIX)$(APP_NAME).elf


# For backward compatibility as app makefile except SRCS_PATH variable
SRCS_PATH := $(APP_SRCS_PATH)

# Include app specific makefile
-include $(APP_SRCS_PATH)makefile


# Include libraries code
-include $(WP_LIB_PATH)makefile
INCLUDES += -I$(WP_LIB_PATH)

#
# Sources & includes paths
#
#SRCS += $(APP_SRCS_PATH)app.c $(MCU_COMMON_SRCS_PATH)start.c $(MCU_EFR_HAL_SRCS_PATH)led.c
SRCS += $(APP_SRCS_PATH)app.c $(MCU_COMMON_SRCS_PATH)start.c 
ASM_SRCS += $(MCU_COMMON_SRCS_PATH)entrypoint.s
INCLUDES += -I$(API_PATH) -I$(APP_SRCS_PATH)include -I$(UTIL_PATH) -I$(MCU_HAL_API_PATH)
# Objects list
OBJS_ = $(SRCS:.c=.o) $(ASM_SRCS:.s=.o)
OBJS  = $(addprefix $(APP_BUILDPREFIX), $(OBJS_))
# Files to be cleaned
CLEAN := $(OBJS) $(APP_ELF) $(APP_HEX)


$(APP_BUILDPREFIX)%.o : %.c
	$(MKDIR) $(@D)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(APP_BUILDPREFIX)%.o : %.s
	$(MKDIR) $(@D)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@


$(APP_ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ \
	      -Wl,-Map=$(APP_BUILDPREFIX)$(APP_NAME).map \
	      -Wl,-T,$(LDSCRIPT) $(LIBS)

$(APP_HEX): $(APP_ELF)
	@echo "Generating $(APP_HEX)"
	$(OBJCOPY) $< -O ihex $@

.PHONY: all
all: $(APP_HEX)

clean:
	$(RM) -rf $(CLEAN)

