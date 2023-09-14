EFR32XG13_HAL_PREFIX := $(MCU_PREFIX)hal/

ifeq ($(HAL_UART), yes)
SRCS += $(EFR32XG13_HAL_PREFIX)hfperclk.c    \
        $(EFR32XG13_HAL_PREFIX)usart.c
endif

ifeq ($(HAL_BUTTON), yes)
SRCS += $(EFR32XG13_HAL_PREFIX)button.c
endif

ifeq ($(HAL_LED), yes)
SRCS += $(EFR32XG13_HAL_PREFIX)led.c
endif

SRCS += $(EFR32XG13_HAL_PREFIX)ds.c    \
        $(EFR32XG13_HAL_PREFIX)hal.c
