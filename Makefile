PROJECT_NAME     := executable
TARGETS          := nrf52810_xxaa
OUTPUT_DIRECTORY := build
PROJ_DIR         := .
#TOOLCHAIN        := arm-gnu-toolchain-12.2.mpacbti-rel1-darwin-arm64-arm-none-eabi
TOOLCHAIN        := arm-gnu-toolchain-12.2.mpacbti-rel1-darwin-x86_64-arm-none-eabi

HEX_APP := $(OUTPUT_DIRECTORY)/$(TARGETS).hex
HEX_SD  := $(PROJ_DIR)/Source/nRF_SoftDevice/hex/s112_nrf52_7.2.0_softdevice.hex

.PHONY: help toolchain firmware

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo "  toolchain        - uncompress the gcc toolchain (run this first or other steps will fail)"
	@echo "  nrf52810_xxaa    - build the main firmware (make sure to run make toolchain before make nrf52810_xxaa)"
	@echo "  flash_softdevice - flash the softdevice (which is necessary for the firmware to run)"
	@echo "  flash            - flashing the app binary (make sure to run make nrf52810_xxaa before make flash)"

toolchain: $(OUTPUT_DIRECTORY)/$(TOOLCHAIN)

$(OUTPUT_DIRECTORY):
	mkdir -p $(OUTPUT_DIRECTORY)

$(OUTPUT_DIRECTORY)/$(TOOLCHAIN): $(TOOLCHAIN).tar.xz.parta $(TOOLCHAIN).tar.xz.partb $(OUTPUT_DIRECTORY)
	cat $(TOOLCHAIN).tar.xz.parta $(TOOLCHAIN).tar.xz.partb > $(TOOLCHAIN).tar.xz 
	tar -xf $(TOOLCHAIN).tar.xz -C $(OUTPUT_DIRECTORY)
	rm $(TOOLCHAIN).tar.xz
	touch $(OUTPUT_DIRECTORY)/$(TOOLCHAIN)

$(OUTPUT_DIRECTORY)/$(TARGETS).out: \
  LINKER_SCRIPT  := ble_app_blinky_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/Source/Application/main.c \
  $(PROJ_DIR)/Source/components/libraries/fstorage/nrf_fstorage.c \
  $(PROJ_DIR)/Source/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_ppi.c \
  $(PROJ_DIR)/Source/integration/nrfx/legacy/nrf_drv_ppi.c	\
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_timer.c \
  $(PROJ_DIR)/Source/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(PROJ_DIR)/Source/components/libraries/log/src/nrf_log_backend_serial.c \
  $(PROJ_DIR)/Source/components/libraries/log/src/nrf_log_backend_uart.c \
  $(PROJ_DIR)/Source/components/libraries/log/src/nrf_log_default_backends.c \
  $(PROJ_DIR)/Source/components/libraries/log/src/nrf_log_frontend.c \
  $(PROJ_DIR)/Source/components/libraries/log/src/nrf_log_str_formatter.c \
  $(PROJ_DIR)/Source/components/libraries/button/app_button.c \
  $(PROJ_DIR)/Source/components/libraries/util/app_error.c \
  $(PROJ_DIR)/Source/components/libraries/util/app_error_handler_gcc.c \
  $(PROJ_DIR)/Source/components/libraries/util/app_error_weak.c \
  $(PROJ_DIR)/Source/components/libraries/scheduler/app_scheduler.c \
  $(PROJ_DIR)/Source/components/libraries/timer/app_timer2.c \
  $(PROJ_DIR)/Source/components/libraries/util/app_util_platform.c \
  $(PROJ_DIR)/Source/components/libraries/timer/drv_rtc.c \
  $(PROJ_DIR)/Source/components/libraries/hardfault/hardfault_implementation.c \
  $(PROJ_DIR)/Source/components/libraries/util/nrf_assert.c \
  $(PROJ_DIR)/Source/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(PROJ_DIR)/Source/components/libraries/atomic_flags/nrf_atflags.c \
  $(PROJ_DIR)/Source/components/libraries/atomic/nrf_atomic.c \
  $(PROJ_DIR)/Source/components/libraries/balloc/nrf_balloc.c \
  $(PROJ_DIR)/Source/external/fprintf/nrf_fprintf.c \
  $(PROJ_DIR)/Source/external/fprintf/nrf_fprintf_format.c \
  $(PROJ_DIR)/Source/components/libraries/memobj/nrf_memobj.c \
  $(PROJ_DIR)/Source/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(PROJ_DIR)/Source/components/libraries/ringbuf/nrf_ringbuf.c \
  $(PROJ_DIR)/Source/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(PROJ_DIR)/Source/components/libraries/sortlist/nrf_sortlist.c \
  $(PROJ_DIR)/Source/components/libraries/strerror/nrf_strerror.c \
  $(PROJ_DIR)/Source/modules/nrfx/mdk/system_nrf52810.c \
  $(PROJ_DIR)/Source/components/boards/boards.c \
  $(PROJ_DIR)/Source/integration/nrfx/legacy/nrf_drv_clock.c \
  $(PROJ_DIR)/Source/integration/nrfx/legacy/nrf_drv_uart.c \
  $(PROJ_DIR)/Source/modules/nrfx/soc/nrfx_atomic.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_clock.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_uart.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(PROJ_DIR)/Source/external/segger_rtt/SEGGER_RTT.c \
  $(PROJ_DIR)/Source/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(PROJ_DIR)/Source/external/segger_rtt/SEGGER_RTT_printf.c \
  $(PROJ_DIR)/Source/components/ble/common/ble_advdata.c \
  $(PROJ_DIR)/Source/components/ble/common/ble_conn_params.c \
  $(PROJ_DIR)/Source/components/ble/common/ble_conn_state.c \
  $(PROJ_DIR)/Source/components/ble/common/ble_srv_common.c \
  $(PROJ_DIR)/Source/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(PROJ_DIR)/Source/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(PROJ_DIR)/Source/external/utf_converter/utf.c \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_lbs/ble_lbs.c \
  $(PROJ_DIR)/Source/components/softdevice/common/nrf_sdh.c \
  $(PROJ_DIR)/Source/components/softdevice/common/nrf_sdh_ble.c \
  $(PROJ_DIR)/Source/components/softdevice/common/nrf_sdh_soc.c \
  $(PROJ_DIR)/Source/Application/bsp/bsp.c \
  $(PROJ_DIR)/Source/Application/mid/lib_ble.c \
  $(PROJ_DIR)/Source/Application/mid/ble_bas.c \
  $(PROJ_DIR)/Source/Application/mid/ble_dis.c \
  $(PROJ_DIR)/Source/Application/mid/ble_cus.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_IrRemote.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_kaseikyo.c \
  $(PROJ_DIR)/Source/Application/bsp/hw_tests.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_MotionSensor.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_saadc.c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src/nrfx_ppi.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_DistanceWidthProtocol.c \
  $(PROJ_DIR)/Source/Application/Op/Op_SupportFunction.c \
  $(PROJ_DIR)/Source/Application/App/App_Control.c \
  $(PROJ_DIR)/Source/Application/Sys/SysTimer.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_RTC.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_Flash.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_Decode.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_KeyControl.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_IrSend.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_Nec.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_Denon.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_Sony.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_RC5_RC6.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_LG.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_JVC.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_Samsung.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_BangOlufsen.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_FAST.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_Whynter.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_Lego.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_BoseWave.c \
  $(PROJ_DIR)/Source/Application/mid/Ir_MagiQuest.c \
  $(PROJ_DIR)/Source/Application/mid/Mid_LedControl.c \
  $(PROJ_DIR)/Source/Application/App/App_AlarmControl.c \
  $(PROJ_DIR)/Source/modules/nrfx/mdk/gcc_startup_nrf52810.S \

# Include folders common to all targets
INC_FOLDERS += \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_ancs_c \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_ias_c \
  $(PROJ_DIR)/Source/components/libraries/pwm \
  $(PROJ_DIR)/Source/components/softdevice/s112/headers/nrf52 \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/cdc/acm \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/hid/generic \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/msc \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/hid \
  $(PROJ_DIR)/Source/modules/nrfx/hal \
  $(PROJ_DIR)/Source/components/libraries/log \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_gls \
  $(PROJ_DIR)/Source/components/libraries/fstorage \
  $(PROJ_DIR)/Source/components/libraries/mutex \
  $(PROJ_DIR)/Source/components/libraries/gpiote \
  $(PROJ_DIR)/Source/components/libraries/bootloader/ble_dfu \
  $(PROJ_DIR)/Source/components/boards \
  $(PROJ_DIR)/Source/components/ble/ble_advertising \
  $(PROJ_DIR)/Source/external/utf_converter \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_bas_c \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/include \
  $(PROJ_DIR)/Source/components/libraries/experimental_task_manager \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_hrs_c \
  $(PROJ_DIR)/Source/components/libraries/queue \
  $(PROJ_DIR)/Source/components/libraries/pwr_mgmt \
  $(PROJ_DIR)/Source/components/ble/ble_dtm \
  $(PROJ_DIR)/Source/components/toolchain/cmsis/include \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_rscs_c \
  $(PROJ_DIR)/Source/components/ble/common \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_lls \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_bas \
  $(PROJ_DIR)/Source/components/libraries/mpu \
  $(PROJ_DIR)/Source/components/libraries/experimental_section_vars \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_ans_c \
  $(PROJ_DIR)/Source/components/libraries/slip \
  $(PROJ_DIR)/Source/components/libraries/delay \
  $(PROJ_DIR)/Source/components/libraries/csense_drv \
  $(PROJ_DIR)/Source/components/libraries/memobj \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_nus_c \
  $(PROJ_DIR)/Source/components/softdevice/common \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_ias \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/hid/mouse \
  $(PROJ_DIR)/Source/components/libraries/low_power_pwm \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_dfu \
  $(PROJ_DIR)/Source/external/fprintf \
  $(PROJ_DIR)/Source/components/libraries/svc \
  $(PROJ_DIR)/Source/components/libraries/atomic \
  $(PROJ_DIR)/Source/components \
  $(PROJ_DIR)/Source/components/libraries/scheduler \
  $(PROJ_DIR)/Source/components/libraries/cli \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_lbs \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_hts \
  $(PROJ_DIR)/Source/components/libraries/crc16 \
  $(PROJ_DIR)/Source/components/libraries/util \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/cdc \
  $(PROJ_DIR)/Source/components/libraries/csense \
  $(PROJ_DIR)/Source/components/libraries/balloc \
  $(PROJ_DIR)/Source/components/libraries/ecc \
  $(PROJ_DIR)/Source/components/libraries/hardfault \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_cscs \
  $(PROJ_DIR)/Source/components/libraries/hci \
  $(PROJ_DIR)/Source/components/libraries/timer \
  $(PROJ_DIR)/Source/integration/nrfx \
  $(PROJ_DIR)/Source/components/libraries/sortlist \
  $(PROJ_DIR)/Source/components/libraries/spi_mngr \
  $(PROJ_DIR)/Source/components/softdevice/s112/headers \
  $(PROJ_DIR)/Source/components/libraries/led_softblink \
  $(PROJ_DIR)/Source/components/libraries/sdcard \
  $(PROJ_DIR)/Source/modules/nrfx/mdk \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_cts_c \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_nus \
  $(PROJ_DIR)/Source/components/libraries/twi_mngr \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_hids \
  $(PROJ_DIR)/Source/components/libraries/strerror \
  $(PROJ_DIR)/Source/components/libraries/crc32 \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/audio \
  $(PROJ_DIR)/Source/components/ble/peer_manager \
  $(PROJ_DIR)/Source/components/libraries/mem_manager \
  $(PROJ_DIR)/Source/components/libraries/ringbuf \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_tps \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_dis \
  $(PROJ_DIR)/Source/components/ble/nrf_ble_gatt \
  $(PROJ_DIR)/Source/components/ble/nrf_ble_qwr \
  $(PROJ_DIR)/Source/components/libraries/gfx \
  $(PROJ_DIR)/Source/components/libraries/button \
  $(PROJ_DIR)/Source/modules/nrfx \
  $(PROJ_DIR)/Source/components/libraries/twi_sensor \
  $(PROJ_DIR)/Source/integration/nrfx/legacy \
  $(PROJ_DIR)/Source/components/libraries/usbd/class/hid/kbd \
  $(PROJ_DIR)/Source/external/segger_rtt \
  $(PROJ_DIR)/Source/components/libraries/atomic_fifo \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_lbs_c \
  $(PROJ_DIR)/Source/components/libraries/crypto \
  $(PROJ_DIR)/Source/components/ble/ble_racp \
  $(PROJ_DIR)/Source/components/libraries/fds \
  $(PROJ_DIR)/Source/components/libraries/atomic_flags \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_hrs \
  $(PROJ_DIR)/Source/components/ble/ble_services/ble_rscs \
  $(PROJ_DIR)/Source/components/libraries/usbd \
  $(PROJ_DIR)/Source/components/libraries/stack_guard \
  $(PROJ_DIR)/Source/components/libraries/log/src \
  $(PROJ_DIR)/Source/Application \
  $(PROJ_DIR)/Source/Application/bsp		 \
  $(PROJ_DIR)/Source/Application/mid		 \
  $(PROJ_DIR)/Source/Application/Op		 \
  $(PROJ_DIR)/Source/modules/nrfx/drivers/src	\
  $(PROJ_DIR)/Source/Application/App/			\
  $(PROJ_DIR)/Source/Application/Sys/			\

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -Os -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
#CFLAGS += -DBOARD_PCA10040
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DDEVELOP_IN_NRF52832
CFLAGS += -DFLOAT_ABI_SOFT
CFLAGS += -DNRF52810_XXAA
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS112
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall
CFLAGS += -w
CFLAGS += -mfloat-abi=soft
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)
# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=soft
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
#ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DDEVELOP_IN_NRF52832
ASMFLAGS += -DFLOAT_ABI_SOFT
ASMFLAGS += -DNRF52810_XXAA
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS112
ASMFLAGS += -DSOFTDEVICE_PRESENT

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(PROJ_DIR)/toolchain_essentials/toolchain_and_libraries/makefile -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

$(TARGETS): CFLAGS += -D__HEAP_SIZE=2048
$(TARGETS): CFLAGS += -D__STACK_SIZE=2048
$(TARGETS): ASMFLAGS += -D__HEAP_SIZE=2048
$(TARGETS): ASMFLAGS += -D__STACK_SIZE=2048

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm

TEMPLATE_PATH := $(PROJ_DIR)/toolchain_essentials/toolchain_and_libraries/makefile

ifeq ($(MAKECMDGOALS),nrf52810_xxaa)
include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))
endif

.PHONY: flash flash_softdevice erase clean

# Flash the program
flash:
	@echo Flashing: $(HEX_APP)
#	nrfjprog -f nrf52 --program $(HEX_APP) --sectorerase
#	nrfjprog -f nrf52 --reset
#	openocd -c 'adapter driver jlink; transport select swd; source [find target/nrf52.cfg]; program $(HEX_APP) verify reset exit'
	openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg -c "reset_config none; program $(HEX_APP) verify reset exit"
#	openocd -f interface/stlink.cfg -f target/nrf52.cfg -c "reset_config none; program $(HEX_APP) verify reset exit"

# Flash softdevice
flash_softdevice:
	@echo Flashing: $(HEX_SD)
#	nrfjprog -f nrf52 --program $(HEX_SD) --sectorerase
#	nrfjprog -f nrf52 --reset
#	openocd -c 'adapter driver jlink; transport select swd; source [find target/nrf52.cfg]; program $(HEX_SD) verify reset exit'
	openocd -f interface/cmsis-dap.cfg -f target/nrf52.cfg -c "reset_config none; program $(HEX_SD) verify reset exit"
#	openocd -f interface/stlink.cfg -f target/nrf52.cfg -c "reset_config none; program $(HEX_SD) verify reset exit"

erase:
#	nrfjprog -f nrf52 --eraseall

clean:
	rm -rf $(OUTPUT_DIRECTORY)
