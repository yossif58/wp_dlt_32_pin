include makefile_common.mk

.DEFAULT_GOAL := all

ifeq ($(BOOTLOADER_VERSION), legacy)
BOOTLOADER_SUFFIX := .hex
else
BOOTLOADER_SUFFIX := .a
endif

# Get the right bootloader binary
# Handle the unprotected flag to determine the binary to pick
ifeq ($(unprotected), yes)
# unprotected flag is set, pick the unprotected binary
BOOTLOADER_SRC := $(IMAGE_PATH)bootloader_unprotected$(BOOTLOADER_SUFFIX)
ifeq (,$(wildcard $(BOOTLOADER_SRC)))
$(error You don't have the unprotected binary to build unprotected image. \
Please contact your sales representative.)
else
$(warning *********************************************************************)
$(warning *********************************************************************)
$(warning The generated image is UNPROTECTED and must not be used in the field )
$(warning *********************************************************************)
$(warning *********************************************************************)
endif
else
BOOTLOADER_SRC := $(IMAGE_PATH)bootloader$(BOOTLOADER_SUFFIX)
endif #ifeq ($(unprotected), yes)

# Bootloader build variables
BL_BUILDPREFIX := $(BUILDPREFIX_BOOTLOADER)

# Linker for the bootloader
LDSCRIPT := mcu/$(MCU)/linker/gcc_bl_$(MCU)$(MCU_SUB).ld

BOOTLOADER_ELF := $(BL_BUILDPREFIX)bootloader.elf

# Include bootloader makefile
-include bootloader/makefile

OBJS_ = $(SRCS:.c=.o)
OBJS = $(addprefix $(BL_BUILDPREFIX), $(OBJS_))

# Files to be cleaned
CLEAN := $(OBJS) $(BOOTLOADER_ELF) $(BOOTLOADER_HEX)

$(BL_BUILDPREFIX)%.o : %.c
ifneq ($(BOOTLOADER_VERSION), legacy)
	$(MKDIR) $(@D)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@
endif
	@	# File not needed if legacy

$(BOOTLOADER_ELF): $(OBJS)
ifneq ($(BOOTLOADER_VERSION), legacy)
	$(MKDIR) $(@D)
	@	# Linking with the provided lib $(BOOTLOADER_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(BOOTLOADER_SRC)\
	      -Wl,-Map=$(BL_BUILDPREFIX)bootloader.map \
	      -Wl,-T,$(LDSCRIPT) $(LIBS)
endif
	@	# File not needed if legacy


$(BOOTLOADER_SRC_HEX):
	@test -e $@ || { echo No bootloader file found. Put the image \
                          under $(IMAGE_PATH); \
                                exit 1; }

$(BOOTLOADER_HEX): $(BOOTLOADER_SRC) $(BOOTLOADER_ELF)
	$(MKDIR) $(@D)
ifeq ($(BOOTLOADER_VERSION), legacy)
	@	# Just copy the chosen source file to destination
	cp $< $@
else
	$(OBJCOPY) $(BOOTLOADER_ELF) -O ihex $@
endif

.PHONY: all
all: $(BOOTLOADER_HEX)

clean:
	$(RM) -rf $(CLEAN)
