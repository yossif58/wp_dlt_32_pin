METER_PREFIX := $(SRCS_PATH)meter/
# Define meter version here
CFLAGS += -DMETER_VERSION=1
CFLAGS += -D_SILICON_LABS_32B_SERIES_0
INCLUDES += -I$(METER_PREFIX)
SRCS +=   $(METER_PREFIX)utils.c     \
          $(METER_PREFIX)DLTProtocol.c              \
          $(METER_PREFIX)uartdlt.c              \
          $(METER_PREFIX)persistent.c              \
          $(METER_PREFIX)MeterProtocol.c      \
          $(METER_PREFIX)MeterPush.c      \
          $(METER_PREFIX)DCUPush.c         
#Y.F. 20/04/20 REMOVED           $(METER_PREFIX)em_i2c.c              \
#Y.F. 20/04/20 REMOVED           $(METER_PREFIX)eeprom.c              \
 