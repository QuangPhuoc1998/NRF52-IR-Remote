#ifndef _OP_HEADER_FUNCTION_H_
#define _OP_HEADER_FUNCTION_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "Op_HeaderDefine.h"

/***************************************************************************/
/*							HEADER FOR APP CONTROLER					   */
/***************************************************************************/
void App_ControlHandle(void);
void App_ControlStartLearnIR(uint16_t uwIrID);
void App_ControlStartEmitIR(uint16_t uwIrID);
/***************************************************************************/
/*					   	HEADER FOR MID IR REMOTE						   */
/***************************************************************************/
void Mid_IrRemoteInit(void);
void Mid_IrRemoteStartScan(void);
void Mid_IrRemoteStopScan(void);
bool Mid_IrRemoteDecode(void);
void Ir_RemoteInitDecodedIRData(void);
void Ir_RemoteResume(void);
void sendPulseDistanceWidthFromArray(PulseDistanceWidthProtocolConstants *aProtocolConstants,
        IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, uint8_t aNumberOfRepeats);\

void sendPanasonic(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
/***************************************************************************/
/*					   		HEADER FOR SYSTIMER							   */
/***************************************************************************/
void SystimerInit(void);
uint32_t SysTimerGetUsValue(void);
uint32_t SysTimerGetMsValue(void);
/***************************************************************************/
/*					   HEADER FOR SUPPORT FUNCTION					   	   */
/***************************************************************************/
uint32_t power(uint32_t ulXNumber, uint8_t ulYNumber);
uint16_t ConvertID(uint8_t * pData, uint8_t len);
void myMemCpy(void *dest, void *src, size_t n);
int16_t LinearSerachID(IRData_t * pData, uint8_t ubSize, uint16_t uwKey);

#endif // _OP_HEADER_FUNCTION_H_
