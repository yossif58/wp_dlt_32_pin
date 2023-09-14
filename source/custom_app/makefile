# You can add more sources here if needed
include $(SRCS_PATH)drivers/makefile
include $(SRCS_PATH)meter/makefile
SRCS += $(SRCS_PATH)drivers/$(MCU)/led.c
SRCS += $(SRCS_PATH)drivers/$(MCU)/io.c
# HAL API is needed by WAPS for UART
HAL_UART=yes
include $(HAL_API_PATH)makefile
INCLUDES +=
# Define default network settings
CFLAGS += -DNETWORK_ADDRESS=$(default_network_address)
CFLAGS += -DNETWORK_CHANNEL=$(default_network_channel)

# Generic util functions are needed (api.c)
include $(UTIL_PATH)makefile
