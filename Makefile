EMU_NATIVE_BIN=../../zeal8bit/Zeal-NativeEmulator/builddir/zeal.elf
EMU_NATIVE_ROM=../../zeal8bit/Zeal-NativeEmulator/roms/default.img

ifndef ZOS_PATH
    $(error "Failure: ZOS_PATH variable not found. It must point to Zeal 8-bit OS path.")
endif

SHELL ?= /bin/bash
SRC_DIR := src
BIN_DIR := bin
BUILD_DIR := build


SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
REL_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.rel,$(SRC_FILES))
IHX_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.ihx,$(SRC_FILES))
BIN_FILES := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(SRC_FILES))

ZOS_INCLUDE=$(ZOS_PATH)/kernel_headers/sdcc/include/
CRT_REL=$(ZOS_PATH)/kernel_headers/sdcc/bin/zos_crt0.rel

ZOS_CC ?= sdcc
ZOS_LD ?= sdldz80

CFLAGS = -mz80 -c --codeseg TEXT --nostdlib -I$(ZOS_INCLUDE) -Iinclude $(ZOS_CFLAGS)
# CFLAGS = -mz80 -c --codeseg TEXT -I$(ZOS_INCLUDE) $(ZOS_CFLAGS)
LDFLAGS = -n -mjwx -i -b _HEADER=0x4000 -k $(ZOS_PATH)/kernel_headers/sdcc/lib -l z80 $(ZOS_LDFLAGS)

OBJCOPY ?= $(shell which sdobjcopy objcopy gobjcopy | head -1)
BIN_HEX=$(patsubst %.bin,%.ihx,$(BIN))

.PHONY: all clean

all: $(BIN_FILES)
	echo "Complete"

$(BUILD_DIR)/%.rel: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(ZOS_CC) $(CFLAGS) -o $(BUILD_DIR)/$(dir $*) $<

$(BUILD_DIR)/%.ihx: $(BUILD_DIR)/%.rel
	$(ZOS_LD) $(LDFLAGS) $(BUILD_DIR)/$* $(CRT_REL) $<

$(BIN_DIR)/%: $(BUILD_DIR)/%.ihx | $(BIN_DIR)
	$(OBJCOPY) --input-target=ihex --output-target=binary $< $(BIN_DIR)/$*

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	echo "Cleaning..."
	rm -rf ./$(BIN_DIR) ./$(BUILD_DIR)

run:
	$(EMU_NATIVE_BIN) -H bin -r $(EMU_NATIVE_ROM) #-t tf.img -e eeprom.img

native: all run