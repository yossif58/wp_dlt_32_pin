CFLAGS	+= -I$(HAL_API_PATH)

MCU_PREFIX := mcu/$(MCU)/

# HAL API needs generic util functions
include $(UTIL_PATH)makefile

# Then include drivers
include $(MCU_PREFIX)hal/makefile
