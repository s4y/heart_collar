DEPS := deps
GCC := deps/gcc-arm-none-eabi-10.3-2021.07
SDK := deps/nRF5_SDK_17.1.0_ddde560

include Makefile.nrf

OPENOCD := openocd -f interface/jlink.cfg -c "transport select swd" -f target/nrf52.cfg
GCC_BIN := $(GCC)/bin

TARGET := display
BOARD := BT832

DEBUG_HEX = build/Debug/$(TARGET).hex
RELEASE_HEX = build/Release/$(TARGET).hex

LOCAL_CC := $(CC)
CC := $(GCC_BIN)/arm-none-eabi-gcc
AR := $(GCC_BIN)/arm-none-eabi-ar
CFLAGS := \
	-Wall \
	-Werror \
	-Wno-expansion-to-defined \
	-DNRF52 \
	-DS140 \
	-DNRF52832_XXAA \
	-DBOARD_CUSTOM \
	-DSOFTDEVICE_PRESENT \
	-DNRF_SD_BLE_API_VERSION=7 \
	-DBOARD_$(BOARD) \
	-I. \
	-Ideps/nRF5_WS2812B/ws2812b_drive \
	$(addprefix -isystem,$(NRF_INC_FOLDERS)) \
	-L$(SDK)/modules/nrfx/mdk \

TARGET_CFLAGS := $(CFLAGS) \
	-mthumb \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \

DEBUG_CFLAGS := $(TARGET_CFLAGS) \
	--specs=rdimon.specs \
	-DDEBUG \
	-g \
	-lc \
	-lrdimon \

RELEASE_CFLAGS := $(TARGET_CFLAGS) \
	--specs=nosys.specs \
	-Os \

SUPPORT_HEADER_FILES := $(wildcard *.h) $(wildcard display/*.h) $(wildcard deps/nRF5_WS2812B/ws2812b_drive/*.h)
SUPPORT_SOURCE_FILES := $(wildcard display/*.c) $(wildcard deps/nRF5_WS2812B/ws2812b_drive/*.c) $(wildcard deps/nRF5_WS2812B/i2s_ws2812b_drive/*.c)

NRF_OBJ_FILES := $(NRF_SRC_FILES)
NRF_OBJ_FILES := $(patsubst %.c,%.o,$(NRF_OBJ_FILES))
NRF_OBJ_FILES := $(patsubst %.S,%.o,$(NRF_OBJ_FILES))

NRF_DEBUG_OBJ_FILES := $(NRF_DEBUG_SRC_FILES)
NRF_DEBUG_OBJ_FILES := $(patsubst %.c,%.o,$(NRF_DEBUG_OBJ_FILES))
NRF_DEBUG_OBJ_FILES := $(patsubst %.S,%.o,$(NRF_DEBUG_OBJ_FILES))
NRF_DEBUG_OBJ_FILES := $(NRF_OBJ_FILES) $(NRF_DEBUG_OBJ_FILES)

default: $(RELEASE_HEX)

ifndef VERBOSE
.SILENT:
endif

__PHONY:

$(GCC):
	mkdir -p $(DEPS)
	cd $(DEPS) && curl -LOJ https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.07/gcc-arm-none-eabi-10.3-2021.07-mac-10.14.6.tar.bz2
	cd $(DEPS) && tar -xjf gcc-arm-none-eabi-10.3-2021.07-mac-10.14.6.tar.bz2

$(SDK):
	mkdir -p $(DEPS)
	cd $(DEPS) && curl -LOJ https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/nRF5_SDK_17.1.0_ddde560.zip
	cd $(DEPS) && unzip nRF5_SDK_17.1.0_ddde560.zip

$(DEPS)/stamp: $(GCC) $(SDK)
	touch $(DEPS)/stamp

nuke: __PHONY
	$(OPENOCD) -c 'init;reset halt;nrf5 mass_erase;exit'

halt: __PHONY
	$(OPENOCD) -c 'init;reset halt;exit'

reset: __PHONY
	$(OPENOCD) -c 'init;reset;exit'

openocd: __PHONY
	$(OPENOCD) -c 'init' $(ARGS)

run-attached: $(DEBUG_HEX) __PHONY
	$(OPENOCD) -c 'init;reset halt;program $< verify;arm semihosting enable;reset;sleep 500;rtt setup 0x20000000 0xffff "SEGGER RTT";rtt start;rtt server start 9090 0'

upload-softdevice: __PHONY
	$(OPENOCD) -c 'init;reset halt;program $(SDK)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex reset verify exit'

upload: $(RELEASE_HEX) __PHONY
	$(OPENOCD) -c 'init;reset halt;program $< verify exit'

run: $(RELEASE_HEX) __PHONY
	$(OPENOCD) -c 'init;reset halt;program $< verify reset exit'

debugserver: $(DEBUG_HEX) __PHONY
	$(OPENOCD) -c 'init;reset halt;program $< verify;arm semihosting enable'

debugclient: build/Debug/$(TARGET).out __PHONY
	$(GCC_BIN)/arm-none-eabi-gdb $< -ex 'target extended-remote 127.0.0.1:3333'

debug: $(DEBUG_HEX) __PHONY
	$(GCC_BIN)/arm-none-eabi-gdb "$<" -ex 'target remote | $(OPENOCD) -c "gdb_port pipe; init; arm semihosting enable; reset halt"'

%.hex: %.out
	$(GCC_BIN)/arm-none-eabi-objcopy -O ihex "$<" "$@"

define builddir
.PRECIOUS: $(1)/$$(SDK)/%.o

$(1)/$$(SDK)/%.o: $$(SDK)/%.c sdk_config.h Makefile Makefile.nrf
	@mkdir -p $$(dir $$@)
	$$(CC) -c -o "$$@" $(2) $$(TARGET_CFLAGS) "$$<"

$(1)/$$(SDK)/%.o: $$(SDK)/%.S sdk_config.h Makefile Makefile.nrf
	@mkdir -p $$(dir $$@)
	$$(CC) -c -o "$$@" $(2) $$(TARGET_CFLAGS) "$$<"

$(1)/%.out: %.ld %.c $$(SUPPORT_HEADER_FILES) $$(SUPPORT_SOURCE_FILES) $$(addprefix $(1)/,$(3))
	@mkdir -p $$(dir $$@)
	$$(CC) -o "$$@" $(2) -Wpedantic -L$(1) -Wl,-T,$$(filter-out %.h,$$^)
endef

$(eval $(call builddir,build/Debug,$(DEBUG_CFLAGS),$(NRF_DEBUG_OBJ_FILES)))
$(eval $(call builddir,build/Release,$(RELEASE_CFLAGS),$(NRF_OBJ_FILES)))

%/.ycm_debug_flags: __PHONY
	@echo \
		--sysroot=$(GCC)/arm-none-eabi \
		--target=armv7m-none-eabi \
		$(DEBUG_CFLAGS) \
		-x c
