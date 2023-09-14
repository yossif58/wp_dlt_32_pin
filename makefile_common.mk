# Version of GCC used for Wirepas testing
GCC_TESTED_VERSION := 4.8.4

# General SDK folder structure
MCU_COMMON_SRCS_PATH := mcu/common/
MCU_EFR_HAL_SRCS_PATH := mcu/efr32xg13/hal/
MCU_HAL_API_PATH := mcu/hal_api

API_PATH := api/
UTIL_PATH := util/
HAL_API_PATH := mcu/hal_api/
WP_LIB_PATH := libraries/
GLOBAL_BUILD := build/

# General compiler flags (Define it before specific makefile in order to allow app to overwrite it)
CFLAGS  := -Wall -Werror -Wextra
CFLAGS  += -std=gnu99 -mthumb -nostartfiles -lgcc -lnosys -ggdb --specs=nano.specs
CFLAGS  += -Os -ffunction-sections -fdata-sections

# Flags for linker
LDFLAGS := -Wl,--gc-sections

# include global config file
-include config.mk

#
# Tools
#
# Prefix for Arm tools
PREFIX := $(arm_toolchain)arm-none-eabi-

# Toolchain programs
CC        := $(PREFIX)gcc
AR        := $(PREFIX)ar
OBJCOPY   := $(PREFIX)objcopy
RM        := rm
CP        := cp
MKDIR     := mkdir -p
SCRAT_GEN := python tools/genscratchpad.py
HEXTOOL   := python tools/hextool.py
MAKE      := make

# Check the toolchain version with GCC
GCC_VERSION := $(shell $(CC) -dumpversion)
ifneq ($(GCC_VERSION),$(GCC_TESTED_VERSION))
$(warning ***********************************************************************)
$(warning "GCC version used is not the version recommended and tested by Wirepas )
$(warning "Recommended version is  $(GCC_TESTED_VERSION))
$(warning ***********************************************************************)
endif

# Name of app
APP_NAME := $(app_name)
# If target_board not specified from command line,
# first supported board is used
target_board ?= $(firstword brd4158a)
$(info Target board is $(target_board))

ifeq ("$(wildcard board/$(target_board)/config.mk)","")
$(error Specified target board $(target_board) doesn't exist)
endif

# Include board specific config
-include board/$(target_board)/config.mk

# Include mcu specific config
-include mcu/$(MCU)/config.mk
# Folder for Wirepas stack binary image
IMAGE_PATH := image/$(MCU)$(MCU_SUB)/
# Add new flags as board and mcu are known
CFLAGS += -DTARGET_BOARD=$(target_board)
CFLAGS += -DMCU=$(MCU)

MCU_UPPER=$(shell echo $(MCU) | tr a-z A-Z)
CFLAGS += -D$(MCU_UPPER)

CFLAGS += -march=$(ARCH)

INCLUDES += -Imcu/$(MCU) -Imcu/$(MCU)/hal -Iboard/$(target_board)

# Folder where the application sources are located (and config file)
APP_SRCS_PATH := source/$(app_name)/
# Build prefixes
BUILDPREFIX := $(GLOBAL_BUILD)$(target_board)/
BUILDPREFIX_APP := $(BUILDPREFIX)$(APP_NAME)/
BUILDPREFIX_BOOTLOADER := $(BUILDPREFIX)bootloader/
BUILDPREFIX_TEST_BOOTLOADER := $(BUILDPREFIX)bootloader_test/

BOOTLOADER_HEX := $(BUILDPREFIX_BOOTLOADER)bootloader.hex
BOOTLOADER_TEST_HEX := $(BUILDPREFIX_TEST_BOOTLOADER)bootloader_test.hex
APP_HEX := $(BUILDPREFIX_APP)$(APP_NAME).hex
