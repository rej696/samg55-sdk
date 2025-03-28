
PREFIX ?= arm-none-eabi
CC = $(PREFIX)-gcc
OBJCOPY = $(PREFIX)-objcopy
OBJDUMP = $(PREFIX)-objdump
GDB ?= $(PREFIX)-gdb
PYTHON ?= python
PROG ?= edbg

SYMBOL ?= main

CMSIS_INCLUDE_PATH ?= vendor/CMSIS/6.1.0/CMSIS/Core/Include
SAMG_DFP_PATH ?= vendor/SAMG_DFP/3.9.204/samg55
DFP_INCLUDE_PATH ?= $(SAMG_DFP_PATH)/include
DFP_SOURCE_PATH ?= $(SAMG_DFP_PATH)/gcc
# There is also an sram linker script
LINKER_SCRIPT ?= $(DFP_SOURCE_PATH)/gcc/samg55j19_flash.ld

CFLAGS  ?=  -W -Wall -Wextra -Werror -Wundef -Wshadow -Wdouble-promotion \
            -Wformat-truncation -fno-common -Wconversion \
            -g3 -Os -ffunction-sections -fdata-sections \
	    -I$(CMSIS_INCLUDE_PATH) -I$(DFP_INCLUDE_PATH) \
	    -D__SAMG55G19__ \
            -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 $(EXTRA_CFLAGS)
LDFLAGS ?= -T$(LINKER_SCRIPT) --specs=nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map

BUILD := build
SRC := src
INC := inc
TARGET := $(BUILD)/firmware

SOURCES := $(shell find $(SRC) -name '*.c')
OBJECTS := $(SOURCES:$(SRC)/%.c=$(BUILD)/%.o)
DFP_SOURCES := $(DFP_SOURCE_PATH)/system_samg55j19.c $(DFP_SOURCE_PATH)/gcc/startup_samg55j19.c
DFP_OBJECTS := $(DFP_SOURCES:$(DFP_SOURCE_PATH)/%.c=$(BUILD)/%.o)

.PHONY: build flash dump gdb disasm addr addresses clean
build: $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).elf: $(OBJECTS) $(DFP_OBJECTS)
	$(CC) $^ $(CFLAGS) -I$(INC) -o $@ $(LDFLAGS)

$(BUILD)/%.o : $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(BUILD)/%.o : $(DFP_SOURCE_PATH)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

flash: $(TARGET).bin
	sudo $(PROG) -b -t samg55 -pv -f $<

dump: $(TARGET).elf
	$(OBJDUMP) -dh $<

gdb: $(TARGET).elf
	# st-util &> /dev/null &
	$(GDB) $< -iex "target extended-remote localhost:4242" --tui

disasm: $(TARGET).elf
	$(GDB) -q $< -ex "set pagination off" -ex "disas /s $(SYMBOL)" -ex quit

addresses: $(TARGET).elf
	@$(OBJDUMP) -d $< | awk '/$$1 ~ /8.*:/ && $$3 !~ /word/ { print $1 }' | tr -d ':'

addr: $(TARGET).elf
	@readelf $<  -s | awk '/$(SYMBOL)/ { printf("%x ", strtonum("0x"$$2) - 1); printf("%x\n", (strtonum("0x"$$2) + $$3 - 1)) }'

clean:
	rm -rf build

