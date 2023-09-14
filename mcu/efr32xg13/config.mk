# Mcu instruction set
ARCH=armv7e-m

# Hardware magic used for this architecture with 512kB Flash, 64kB RAM
HW_MAGIC=08
CFLAGS += -DEFR32FG13 -DEFR32FG13P233F512GM48
INCLUDES += -Imcu/$(MCU)/cmsis

# This mcu has a bootloader (enough memory)
HAS_BOOTLOADER=yes

# This mcu uses the version 3 of the bootloader (with external flash support)
BOOTLOADER_VERSION=v3

# Add custom flags
# Remove the -Wunused-parameter flag added by -Wextra as some cortex M4 header do not respect it
CFLAGS += -Wno-unused-parameter
