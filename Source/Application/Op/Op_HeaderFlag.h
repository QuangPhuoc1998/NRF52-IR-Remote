#ifndef _OP_HEADER_FLAG_H_
#define _OP_HEADER_FLAG_H_

#ifdef GLOBAL_DEFINE
#define HEADER_FLAG
#else
#define HEADER_FLAG extern
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
  uint8_t b0 : 1;   /// 0 bit memoery define
  uint8_t b1 : 1;   /// 1 bit memoery define
  uint8_t b2 : 1;   /// 2 bit memoery define
  uint8_t b3 : 1;   /// 3 bit memoery define
  uint8_t b4 : 1;   /// 4 bit memoery define
  uint8_t b5 : 1;   /// 5 bit memoery define
  uint8_t b6 : 1;   /// 6 bit memoery define
  uint8_t b7 : 1;   /// 7 bit memoery define
} USER_BYTE_FIELD;

typedef union
{
  uint8_t   byte;
  USER_BYTE_FIELD   byte_bit;
} USER_TBYTE;
/***************************************************************************/
/*					   			FLAG FOR BLE							   */
/***************************************************************************/
HEADER_FLAG volatile USER_TBYTE	g_FubBLE1;
#define BLE_CONNECTED_FLAG						g_FubBLE1.byte_bit.b0
/***************************************************************************/
/*					   			FLAG FOR IR							  	   */
/***************************************************************************/
HEADER_FLAG volatile USER_TBYTE	g_FubOperation1;
#define APP_START_LEARN_IR_FLAG					g_FubOperation1.byte_bit.b0
#define APP_LEARN_IR_COUNT_START_FLAG			g_FubOperation1.byte_bit.b1
#define APP_LEARN_IR_COUNT_TIMEOUT_FLAG			g_FubOperation1.byte_bit.b2
#define IR_SCAN_DELAY_FLAG						g_FubOperation1.byte_bit.b3
#define DETECT_IR_SIGNAL_DONE_FLAG				g_FubOperation1.byte_bit.b4
#define APP_START_EMIT_IR_FLAG					g_FubOperation1.byte_bit.b5
#define APP_START_ERASE_IR_FLAG					g_FubOperation1.byte_bit.b6

HEADER_FLAG USER_TBYTE	g_FubTimerCount1;
/***************************************************************************/
/*					   			FLAG FOR KEY							   */
/***************************************************************************/
HEADER_FLAG USER_TBYTE KeyFlag1;
#define KEY_1CLICK_FLAG            	KeyFlag1.byte_bit.b0
#define KEY_0_5SEC_CLICK_FLAG     	KeyFlag1.byte_bit.b1
#define KEY_1SEC_CLICK_FLAG     	KeyFlag1.byte_bit.b2
#define KEY_1_5SEC_CLICK_FLAG     	KeyFlag1.byte_bit.b3
#define KEY_2SEC_CLICK_FLAG     	KeyFlag1.byte_bit.b4
#define KEY_3SEC_CLICK_FLAG     	KeyFlag1.byte_bit.b5
#define KEY_5SEC_CLICK_FLAG     	KeyFlag1.byte_bit.b6
#define KEY_7SEC_CLICK_FLAG     	KeyFlag1.byte_bit.b7

HEADER_FLAG USER_TBYTE KeyFlag2;
#define KEY_RELEASE_CLICK_FLAG            KeyFlag2.byte_bit.b0
#define ERROR_KEYSHORT_FLAG               KeyFlag2.byte_bit.b1
#define KEY_PRESS_HOLD_FLAG               KeyFlag2.byte_bit.b2
/***************************************************************************/
/*					   	FLAG FOR MOTION SENSOR							   */
/***************************************************************************/
HEADER_FLAG USER_TBYTE MotionSensorFlag1;
#define MOTION_SENSOR_START_SENSING            	MotionSensorFlag1.byte_bit.b0
#define MOTION_SENSOR_SENSING_DONE				MotionSensorFlag1.byte_bit.b1
#define MOTION_SENSOR_DISABLE					MotionSensorFlag1.byte_bit.b2
#define MOTION_SENSOR_TIMEOUT_COUNT				MotionSensorFlag1.byte_bit.b3
#define MOTION_SENSOR_PRE_TIMEOUT_COUNT			MotionSensorFlag1.byte_bit.b4
/***************************************************************************/
/*					   	FLAG FOR CONTROL LED							   */
/***************************************************************************/
HEADER_FLAG USER_TBYTE LedFlag1;
#define LED_START_CONTROL_FLAG            LedFlag1.byte_bit.b0
#define LED_START_ON_FLAG                 LedFlag1.byte_bit.b1
#define LED_START_OFF_FLAG                LedFlag1.byte_bit.b2
#endif // _OP_HEADER_FLAG_H_
