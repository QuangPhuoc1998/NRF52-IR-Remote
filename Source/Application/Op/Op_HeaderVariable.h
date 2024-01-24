#ifndef _OP_HEADER_VARIABLE_H_
#define _OP_HEADER_VARIABLE_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef GLOBAL_DEFINE
#define HEADER_VARIABLE
#else
#define HEADER_VARIABLE extern
#endif

/***************************************************************************/
/*					   		VARIABLE FOR TIME   						   */
/***************************************************************************/
HEADER_VARIABLE uint8_t g_ubScanIRCount;
HEADER_VARIABLE volatile uint8_t g_ubDelayCount;
/***************************************************************************/
/*					   		VARIABLE FOR IR MODULE						   */
/***************************************************************************/
HEADER_VARIABLE struct irparams_struct irparams;
HEADER_VARIABLE IRData decodedIRData;
HEADER_VARIABLE decode_type_t lastDecodedProtocol;
HEADER_VARIABLE uint32_t lastDecodedCommand;
HEADER_VARIABLE uint32_t lastDecodedAddress;
/***************************************************************************/
/*					   		VARIABLE FOR IR CODE						   */
/***************************************************************************/
HEADER_VARIABLE IRData_t t_IRDataCommom[MAX_IR_CODE];
HEADER_VARIABLE uint8_t g_ubIRCount;
/***************************************************************************/
/*					   		VARIABLE FOR SYSTIMER						   */
/***************************************************************************/
HEADER_VARIABLE date_time_t g_tCommonTime;
HEADER_VARIABLE	uint64_t g_ullUnixTimeStamp;
/***************************************************************************/
/*					   		VARIABLE FOR SETTING IR						   */
/***************************************************************************/
HEADER_VARIABLE IrDataTrigger_t g_atIRDataTrigger[MAX_IR_CODE];
HEADER_VARIABLE uint8_t g_ubIRTriggerCount;
HEADER_VARIABLE uint8_t g_ubTriggerType;

/***************************************************************************/
/*					   		VARIABLE FOR KEY							   */
/***************************************************************************/
HEADER_VARIABLE uint8_t g_ubKeyOffCount;
HEADER_VARIABLE volatile uint32_t g_ulKeyInValue;
HEADER_VARIABLE uint8_t g_ubKeyPositionValue;
HEADER_VARIABLE uint16_t g_uwKeyContinousCount;
HEADER_VARIABLE uint8_t g_ubUpDownMoveValue;
/***************************************************************************/
/*					     VARIABLE FOR MOTION SENSOR						   */
/***************************************************************************/
HEADER_VARIABLE uint8_t g_ubMotionSensorGrade;
HEADER_VARIABLE uint8_t	g_ubMotionSensorCountTime;
HEADER_VARIABLE uint8_t	g_ubMotionSensorDisableTime;
HEADER_VARIABLE uint8_t g_ubMotionCount;
#endif
