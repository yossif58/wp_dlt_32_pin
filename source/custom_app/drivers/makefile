
DRIVERS_PREFIX := $(SRCS_PATH)drivers/

INCLUDES += -I$(DRIVERS_PREFIX)

ifeq ($(MCU),nrf52)
    include $(DRIVERS_PREFIX)nrf52/makefile
else ifeq ($(MCU),nrf52840)
    include $(DRIVERS_PREFIX)nrf52840/makefile
else ifeq ($(MCU),cc2650)
    include $(DRIVERS_PREFIX)cc2650/makefile
else ifeq ($(MCU),efm32lg)
    include $(DRIVERS_PREFIX)efm32lg/makefile
else ifeq ($(MCU),efr32xg12)
    include $(DRIVERS_PREFIX)efr32xg12/makefile
else ifeq ($(MCU),efr32xg13)
    include $(DRIVERS_PREFIX)efr32xg13/makefile
else
    $(error Cannot determine MCU for drivers)
endif
