# Global makefile to build images to be flashed
# or updated through OTAP

include makefile_common.mk

# WPC stack binary location
FIRMWARE_HEX := $(IMAGE_PATH)wpc_stack.hex
include $(IMAGE_PATH)version.mk
FIRMWARE_NAME := wpc_stack

#
# Targets
#

# Scratchpads for OTAP
FULL_SCRATCHPAD_NAME := $(APP_NAME)_$(FIRMWARE_NAME)
FULL_SCRATCHPAD_BIN := $(BUILDPREFIX_APP)$(FULL_SCRATCHPAD_NAME).otap
APP_SCRATCHPAD_NAME := $(APP_NAME)
APP_SCRATCHPAD_BIN := $(BUILDPREFIX_APP)$(APP_SCRATCHPAD_NAME).otap
STACK_SCRATCHPAD_NAME := $(FIRMWARE_NAME)
STACK_SCRATCHPAD_BIN := $(BUILDPREFIX_APP)$(STACK_SCRATCHPAD_NAME).otap

CLEAN += $(FULL_SCRATCHPAD_BIN) $(APP_SCRATCHPAD_BIN) $(STACK_SCRATCHPAD_BIN)

# Final image for programming
FINAL_IMAGE_NAME := final_image_$(APP_NAME)
FINAL_IMAGE_HEX := $(BUILDPREFIX_APP)$(FINAL_IMAGE_NAME).hex

CLEAN += $(FINAL_IMAGE_HEX)

# Intermediate image
BOOTLOADER_HEX_WITH_AREA_AND_KEY_HEX := $(BUILDPREFIX_APP)bootloader_key_area.hex


 TARGETS += $(FINAL_IMAGE_HEX) otap

#
# Manage area id and ini files
#
# Include app specific config needed to generate a scratchpad
-include $(APP_SRCS_PATH)config.mk

# Define the app_area as a combination of app_area and HW_MAGIC
ifeq ($(app_specific_area_id),)
$(error You must define a specific area id in your application config.mk file)
endif
app_area_id =$(app_specific_area_id)$(HW_MAGIC)

# Defines the default firmware_area_id
include $(IMAGE_PATH)bootloader.mk

# Set Default scratchpad ini file if not overriden by app makefile
INI_FILE ?= ./tools/scratchpad_$(MCU)$(MCU_SUB).ini

#
# Functions
#
define COMBINEHEX
	@echo "  Combining $(4) + $(3) + $(2) -> $1)"
	$(HEXTOOL) --output=hex:$(1) hex:$(4) hex:$(3) hex:$(2)
endef

define BUILD_FULL_SCRATCHPAD
	@echo "  Creating Full Scratchpad: $(3) + $(4) + $(5) -> $(1) + $(2)"
	$(SCRAT_GEN)    --bootloader=$(3) \
	                --genprog=$(1) \
	                --configfile=$(INI_FILE) \
	                --otapseq=$(otap_seq_number) \
	                --set=APP_AREA_ID=$(app_area_id) \
	                $(2) \
	                $(firmware_major).$(firmware_minor).$(firmware_maintenance).$(firmware_development):$(firmware_area_id):$(4) \
	                $(app_major).$(app_minor).$(app_maintenance).$(app_development):$(app_area_id):$(5)
endef

define BUILD_APP_SCRATCHPAD
	@echo "  Creating App Scratchpad: $(2) -> $(1)"
	$(SCRAT_GEN)    --configfile=$(INI_FILE) \
	                --otapseq=$(otap_seq_number) \
	                --set=APP_AREA_ID=$(app_area_id) \
	                $(1) \
	                $(app_major).$(app_minor).$(app_maintenance).$(app_development):$(app_area_id):$(2)
endef

define BUILD_STACK_SCRATCHPAD
	@echo "  Creating Stack Scratchpad: $(2) -> $(1)"
	$(SCRAT_GEN)    --configfile=$(INI_FILE) \
	                --otapseq=$(otap_seq_number) \
	                --set=APP_AREA_ID=$(app_area_id) \
	                $(1) \
	                $(firmware_major).$(firmware_minor).$(firmware_maintenance).$(firmware_development):$(firmware_area_id):$(2)
endef

define BUILD_BOOTLOADER_WITHOUT_SCRATCHPAD
	@echo "  Creating flashable Bootloader without Scratchpad: $(2) -> $(1)"
	$(SCRAT_GEN)    --bootloader=$(2) \
	                --genprog=$(1) \
	                --configfile=$(INI_FILE) \
	                --otapseq=$(otap_seq_number) \
	                --set=APP_AREA_ID=$(app_area_id) \
	                empty

endef

# Params: (1) Final image (2) bootloader (3) Ini file (4) Test app
define BUILD_BOOTLOADER_TEST_APP
	@echo "  Creating test application for bootloader: $(2) + $(3) + $(4) -> $(1)"
	$(eval output_file:=$(BUILDPREFIX_TEST_BOOTLOADER)temp_file.hex)
	$(SCRAT_GEN)    --bootloader=$(2) \
	                --genprog=$(output_file) \
	                --configfile=$(3) \
	                --otapseq=$(otap_seq_number) \
	                --set=APP_AREA_ID=$(app_area_id) \
	                empty

	$(HEXTOOL) --output=hex:$(1) hex:$(output_file) hex:$(4)
	$(RM) $(output_file)
endef


.PHONY: all flashable otap
all: $(TARGETS)

flashable: $(APP_HEX)

otap: $(FULL_SCRATCHPAD_BIN) $(APP_SCRATCHPAD_BIN) $(STACK_SCRATCHPAD_BIN)

bootloader: $(BOOTLOADER_HEX)

$(FIRMWARE_HEX):
	@   # File created by stack build
	@test -e $(FIRMWARE_HEX) || { echo No WPC stack hex file found. Put the image \
                                   in $(IMAGE_PATH); \
                              exit 1; }

# Add $(APP_HEX) to PHONY to always call app makefile
.PHONY: $(APP_HEX)
$(APP_HEX): $(BUILDPREFIX_APP)
	@	# Call app makefile to get the hex file of app
	+$(MAKE) -f makefile_app.mk


# Add $(BOOTLOADER_HEX) to PHONY to always call bootloader makefile
.PHONY: $(BOOTLOADER_HEX)
$(BOOTLOADER_HEX):
	@	# Call bootloader makefile to get the hex file of bootloader
	+$(MAKE) -f makefile_bootloader.mk

$(BOOTLOADER_HEX_WITH_AREA_AND_KEY_HEX): $(BOOTLOADER_HEX) $(BUILDPREFIX_APP)

.PHONY: $(BOOTLOADER_TEST_HEX)
$(BOOTLOADER_TEST_HEX):
	@	# Call bootloader test makefile to get the test application hex file
	+$(MAKE) -f makefile_bootloader_test.mk

$(BOOTLOADER_HEX_WITH_AREA_AND_KEY_HEX): $(BOOTLOADER_HEX)
	$(call BUILD_BOOTLOADER_WITHOUT_SCRATCHPAD,$@,$<)

$(STACK_SCRATCHPAD_BIN): $(FIMWARE_HEX) $(BOOTLOADER_HEX) $(BUILDPREFIX_APP)
	$(call BUILD_STACK_SCRATCHPAD,$(STACK_SCRATCHPAD_BIN),$(FIRMWARE_HEX))

$(APP_SCRATCHPAD_BIN): $(APP_HEX)
	$(call BUILD_APP_SCRATCHPAD,$(APP_SCRATCHPAD_BIN),$(APP_HEX))

$(FULL_SCRATCHPAD_BIN): $(BOOTLOADER_HEX) $(FIRMWARE_HEX) $(APP_HEX)
	$(call BUILD_FULL_SCRATCHPAD,$(FINAL_IMAGE_HEX),$(FULL_SCRATCHPAD_BIN),$(BOOTLOADER_HEX),$(FIRMWARE_HEX),$(APP_HEX))

$(FINAL_IMAGE_HEX): $(FIRMWARE_HEX) $(APP_HEX) $(BOOTLOADER_HEX_WITH_AREA_AND_KEY_HEX)
	$(call COMBINEHEX,$@,$<,$(word 2,$^),$(word 3,$^))

bootloader_test: $(BOOTLOADER_HEX) $(BOOTLOADER_TEST_HEX)
	$(call BUILD_BOOTLOADER_TEST_APP,$(BUILDPREFIX_TEST_BOOTLOADER)final_bootloader_test.hex,$<,$(INI_FILE),$(word 2,$^))


$(BUILDPREFIX_APP):
	$(MKDIR) $(@D)

# clean the specified app
.PHONY: clean
clean:
	+$(MAKE) -f makefile_app.mk clean
	+$(MAKE) -f makefile_bootloader.mk clean
	rm -rf $(BUILDPREFIX_APP)

# clean all the apps
.PHONY: clean_all
clean_all:
	rm -rf $(GLOBAL_BUILD)
