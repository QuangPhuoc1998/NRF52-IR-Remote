#ifndef _MAIN_H_
#define _MAIN_H_

#include "Op_HeaderDefine.h"
#include "Op_HeaderFlag.h"
#include "Op_HeaderVariable.h"
#include "Op_HeaderFunction.h"

#include "LongUnion.h"
#include "sdk_config.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_cus.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "ble_lbs.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "app_scheduler.h"
#include "lib_ble.h"
#include "bsp.h"
#include "nrf_drv_gpiote.h"

#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"
#include "nrfx_saadc.h"

#include "nrf_delay.h"
#include "nrf_fstorage.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"

#define ERASE_SIZE	(4096) // byte

#define START_ADDRESS_FSTORAGE	(0x2b000)
#define END_ADDRESS_FSTORAGE	(0x2f000)
#define FSTORAGE_SIZE			(END_ADDRESS_FSTORAGE - START_ADDRESS_FSTORAGE)

#define SIZE_OF_IR_DATA_TYPE           (sizeof(IRData_t))
#define SIZE_OF_IR_DATA_VAR            (sizeof(t_IRDataCommom))
#define SIZE_OF_IR_TRIG_TYPE           (sizeof(IrDataTrigger_t))
#define SIZE_OF_IR_TRIG_VAR            (sizeof(g_atIRDataTrigger))


#define IR_DATA_START_ADDRESS       START_ADDRESS_FSTORAGE
#define IR_TRIG_START_ADDRESS       (IR_DATA_START_ADDRESS + ERASE_SIZE)

#define CONVERT_IR_DATA_TO_ADDRESS(Var)      (IR_DATA_START_ADDRESS+(SIZE_OF_IR_DATA_TYPE * Var))
#define CONVERT_IR_TRIG_TO_ADDRESS(Var)      (IR_TRIG_START_ADDRESS+(SIZE_OF_IR_TRIG_TYPE * Var))

#endif // _MAIN_H_
