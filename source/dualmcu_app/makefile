# You can add more sources here if needed
include $(SRCS_PATH)waps/makefile
include $(SRCS_PATH)drivers/makefile

# Generic util functions are needed by WAPS
include $(UTIL_PATH)makefile

# HAL API is needed by WAPS for UART
HAL_UART=yes
include $(HAL_API_PATH)makefile


INCLUDES += -I$(SRCS_PATH)

# Define default baudrate and hardware flow control settings
#uart_br ?= 125000
# y.f. add parity none for dualmcu 
uart_br ?= 115200
uart_fc ?= false
uart_dualmcu_pariry ?= true
CFLAGS += -DUART_BAUDRATE=$(uart_br)
CFLAGS += -DUART_FLOWCONTROL=$(uart_fc)
CFLAGS += -DDUALMCU=$(uart_dualmcu_pariry)
# Define default network settings
default_network_address ?= 0xA1B2C3
default_network_channel ?= 2
# Define default network settings
CFLAGS += -DNETWORK_ADDRESS=$(default_network_address)
CFLAGS += -DNETWORK_CHANNEL=$(default_network_channel)

