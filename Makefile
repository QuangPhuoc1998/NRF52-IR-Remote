PROJECT_NAME     := ble_app_blinky_pca10040e_s112
TARGETS          := nrf52810_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := .
PROJ_DIR := .

$(OUTPUT_DIRECTORY)/nrf52810_xxaa.out: \
  LINKER_SCRIPT  := ble_app_blinky_gcc_nrf52.ld

HEX_APP := $(OUTPUT_DIRECTORY)/$(TARGETS).hex
HEX_SD  := $(PROJ_DIR)/Source/nRF_SoftDevice/hex/s112_nrf52_7.2.0_softdevice.hex

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)\Source\components\libraries\fstorage\nrf_fstorage.c \
  $(SDK_ROOT)\Source\components\libraries\fstorage\nrf_fstorage_sd.c \
  $(SDK_ROOT)\Source\modules\nrfx\drivers\src\nrfx_ppi.c \
  $(SDK_ROOT)/Source\integration\nrfx\legacy\nrf_drv_ppi.c	\
  $(SDK_ROOT)\Source\modules\nrfx\drivers\src\nrfx_timer.c \
  $(SDK_ROOT)/Source\Application/mid/ir_decoder.c \
  $(SDK_ROOT)/Source\Application/mid/ir_transmitter.c \
  $(SDK_ROOT)/Source\Application\main.c \
  $(SDK_ROOT)/Source\modules/nrfx/mdk/gcc_startup_nrf52810.S \
  $(SDK_ROOT)/Source\components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/Source\components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/Source\components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/Source\components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/Source\components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/Source\components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/Source\components/libraries/button/app_button.c \
  $(SDK_ROOT)/Source\components/libraries/util/app_error.c \
  $(SDK_ROOT)/Source\components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/Source\components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/Source\components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/Source\components/libraries/timer/app_timer2.c \
  $(SDK_ROOT)/Source\components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/Source\components/libraries/timer/drv_rtc.c \
  $(SDK_ROOT)/Source\components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/Source\components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/Source\components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/Source\components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/Source\components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/Source\components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/Source\external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/Source\external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/Source\components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/Source\components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/Source\components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/Source\components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/Source\components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/Source\components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/Source\modules/nrfx/mdk/system_nrf52810.c \
  $(SDK_ROOT)/Source\components/boards/boards.c \
  $(SDK_ROOT)/Source\integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/Source\integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/Source\modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/Source\external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/Source\external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/Source\external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/Source\components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/Source\components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/Source\components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/Source\components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/Source\components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/Source\components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/Source\external/utf_converter/utf.c \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_lbs/ble_lbs.c \
  $(SDK_ROOT)/Source\components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/Source\components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/Source\components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/Source\Application/bsp/bsp.c \
  $(SDK_ROOT)/Source\Application/mid/lib_ble.c \
  $(SDK_ROOT)/Source\Application/mid/ble_bas.c \
  $(SDK_ROOT)/Source\Application/mid/ble_dis.c \
  $(SDK_ROOT)/Source\Application/mid/ble_cus.c \
  $(SDK_ROOT)/Source\Application/mid/Mid_IrRemote.c \
  $(SDK_ROOT)/Source\Application/mid/Mid_IrNec.c \
  $(SDK_ROOT)/Source\Application/mid/Ir_kaseikyo.c \
  $(SDK_ROOT)/Source\Application/bsp/hw_tests.c \
  $(SDK_ROOT)/Source\Application/mid/Mid_MotionSensor.c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src/nrfx_saadc.c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src/nrfx_ppi.c \
  $(SDK_ROOT)/Source\Application/mid/Ir_DistanceWidthProtocol.c \
  $(SDK_ROOT)/Source\Application/Op/Op_SupportFunction.c \
  $(SDK_ROOT)/Source\Application/App/App_Control.c \
  $(SDK_ROOT)/Source\Application/Sys/SysTimer.c \
  $(SDK_ROOT)/Source\Application/mid/Mid_RTC.c \
  $(SDK_ROOT)/Source\Application/mid/Mid_Flash.c \
  $(SDK_ROOT)/Source\Application/mid/Mid_Decode.c \
  $(SDK_ROOT)/Source\Application/mid/Mid_KeyControl.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/Source\components/libraries/pwm \
  $(SDK_ROOT)/Source\components/softdevice/s112/headers/nrf52 \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/msc \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/hid \
  $(SDK_ROOT)/Source\modules/nrfx/hal \
  $(SDK_ROOT)/Source\components/libraries/log \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/Source\components/libraries/fstorage \
  $(SDK_ROOT)/Source\components/libraries/mutex \
  $(SDK_ROOT)/Source\components/libraries/gpiote \
  $(SDK_ROOT)/Source\components/libraries/bootloader/ble_dfu \
  $(SDK_ROOT)/Source\components/boards \
  $(SDK_ROOT)/Source\components/ble/ble_advertising \
  $(SDK_ROOT)/Source\external/utf_converter \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/include \
  $(SDK_ROOT)/Source\components/libraries/experimental_task_manager \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/Source\components/libraries/queue \
  $(SDK_ROOT)/Source\components/libraries/pwr_mgmt \
  $(SDK_ROOT)/Source\components/ble/ble_dtm \
  $(SDK_ROOT)/Source\components/toolchain/cmsis/include \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/Source\components/ble/common \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/Source\components/libraries/mpu \
  $(SDK_ROOT)/Source\components/libraries/experimental_section_vars \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/Source\components/libraries/slip \
  $(SDK_ROOT)/Source\components/libraries/delay \
  $(SDK_ROOT)/Source\components/libraries/csense_drv \
  $(SDK_ROOT)/Source\components/libraries/memobj \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_nus_c \
  $(SDK_ROOT)/Source\components/softdevice/common \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/Source\components/libraries/low_power_pwm \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/Source\external/fprintf \
  $(SDK_ROOT)/Source\components/libraries/svc \
  $(SDK_ROOT)/Source\components/libraries/atomic \
  $(SDK_ROOT)/Source\components \
  $(SDK_ROOT)/Source\components/libraries/scheduler \
  $(SDK_ROOT)/Source\components/libraries/cli \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/Source\components/libraries/crc16 \
  $(SDK_ROOT)/Source\components/libraries/util \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/Source\components/libraries/csense \
  $(SDK_ROOT)/Source\components/libraries/balloc \
  $(SDK_ROOT)/Source\components/libraries/ecc \
  $(SDK_ROOT)/Source\components/libraries/hardfault \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/Source\components/libraries/hci \
  $(SDK_ROOT)/Source\components/libraries/timer \
  $(SDK_ROOT)/Source\integration/nrfx \
  $(SDK_ROOT)/Source\components/libraries/sortlist \
  $(SDK_ROOT)/Source\components/libraries/spi_mngr \
  $(SDK_ROOT)/Source\components/softdevice/s112/headers \
  $(SDK_ROOT)/Source\components/libraries/led_softblink \
  $(SDK_ROOT)/Source\components/libraries/sdcard \
  $(SDK_ROOT)/Source\modules/nrfx/mdk \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/Source\components/libraries/twi_mngr \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/Source\components/libraries/strerror \
  $(SDK_ROOT)/Source\components/libraries/crc32 \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/audio \
  $(SDK_ROOT)/Source\components/ble/peer_manager \
  $(SDK_ROOT)/Source\components/libraries/mem_manager \
  $(SDK_ROOT)/Source\components/libraries/ringbuf \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/Source\components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/Source\components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/Source\components/libraries/gfx \
  $(SDK_ROOT)/Source\components/libraries/button \
  $(SDK_ROOT)/Source\modules/nrfx \
  $(SDK_ROOT)/Source\components/libraries/twi_sensor \
  $(SDK_ROOT)/Source\integration/nrfx/legacy \
  $(SDK_ROOT)/Source\components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/Source\external/segger_rtt \
  $(SDK_ROOT)/Source\components/libraries/atomic_fifo \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/Source\components/libraries/crypto \
  $(SDK_ROOT)/Source\components/ble/ble_racp \
  $(SDK_ROOT)/Source\components/libraries/fds \
  $(SDK_ROOT)/Source\components/libraries/atomic_flags \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/Source\components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/Source\components/libraries/usbd \
  $(SDK_ROOT)/Source\components/libraries/stack_guard \
  $(SDK_ROOT)/Source\components/libraries/log/src \
  $(SDK_ROOT)/Source\Application \
  $(SDK_ROOT)/Source\Application\bsp		 \
  $(SDK_ROOT)/Source\Application\mid		 \
  $(SDK_ROOT)/Source\Application\Op		 \
  $(SDK_ROOT)/Source\modules/nrfx/drivers/src	\
  $(SDK_ROOT)/Source\Application/App/			\
  $(SDK_ROOT)/Source\Application/Sys/			\

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
CFLAGS += -DBOARD_PCA10040
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
CFLAGS += -Wall #-Werror
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
ASMFLAGS += -DBOARD_PCA10040
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
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52810_xxaa: CFLAGS += -D__HEAP_SIZE=2048
nrf52810_xxaa: CFLAGS += -D__STACK_SIZE=2048
nrf52810_xxaa: ASMFLAGS += -D__HEAP_SIZE=2048
nrf52810_xxaa: ASMFLAGS += -D__STACK_SIZE=2048

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52810_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52810_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/Source/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

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
