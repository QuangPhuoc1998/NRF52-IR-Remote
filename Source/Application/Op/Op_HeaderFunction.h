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
/*					   		HEADER FOR MID RTC							   */
/***************************************************************************/
void Mid_RTCInit(void);
void Mid_RTCHandle(void *p_event_data, uint16_t event_size);
void Mid_RTCSetUnit(uint64_t ullUnixValue);
void Mid_RTCConvertUnixToDate(uint64_t ulUnix, date_time_t * pDate);
uint64_t Mid_RTCConvertDateToUnixTime(const date_time_t *date);
uint8_t Mid_RTCComputeDayOfWeek(uint16_t ulYear, uint8_t ubMonth, uint8_t ubDay);
/***************************************************************************/
/*					   		HEADER FOR MID FLASH						   */
/***************************************************************************/
void Mid_FlashInit(void);
bool Mid_FlashWrite(uint32_t ulAddress, void const * p_src, uint32_t ulLen);
bool Mid_FlashRead(void * pData, uint32_t ulAddress, uint32_t ulLen);
bool Mid_FlashErase(uint32_t ulAddress, uint32_t ulLen);
/***************************************************************************/
/*					   		HEADER FOR SYSTIMER							   */
/***************************************************************************/
void SystimerInit(void);
uint32_t SysTimerGetUsValue(void);
uint32_t SysTimerGetMsValue(void);
/***************************************************************************/
/*					   		HEADER FOR SETTING IR						   */
/***************************************************************************/
void Mid_DecodeInit(void);
void Mid_DecodeCommand(uint8_t ubCommandIndex, uint8_t * pData, uint8_t len);
void Mid_DecodeTimeBasedScheduling(void *pData, uint16_t event_size);
bool Mid_DecodeTimeBasedSchedulingDecode(uint8_t * ubCommand, IrDataTrigger_t * tIRDataTrigger,  uint8_t * ubNumOfCompenent, uint8_t * pData, uint8_t len);
int16_t Mid_FindIRTrigger(uint16_t uwTrigID);
bool Mid_InsertNewIRTrigger(IrDataTrigger_t * tIRDataTrigger);
bool Mid_RemoveIRTrigger(uint16_t uwTrigID);

void Mid_DecodeTimeOut(void * pData, uint16_t len);
uint8_t Mid_DecodeTimeOutDecode(uint8_t * ubCommand, IrDataTimeout_t * tIRDataTimeout,  uint8_t * ubNumOfCompenent, uint8_t * pData, uint8_t len);
int16_t Mid_FindIRTimeout(uint16_t uwTrigID);
bool Mid_InsertNewIRTimeOut(IrDataTimeout_t * tIRDataTrigger);
bool Mid_RemoveIRTimeOut(uint16_t uwTrigID);
bool Mid_InsertNewIRCode(IRData * tIRData, uint16_t uwLearnID);
void Mid_DecodeRTCSet(void *p_event_data, uint16_t event_size);
/***************************************************************************/
/*					   		HEADER FOR KEY FUNCTION					   	   */
/***************************************************************************/
void Mid_KeyControlInit(void);
void Mid_KeyHandler(void *p_event_data, uint16_t event_size);
/***************************************************************************/
/*					   HEADER FOR SUPPORT FUNCTION					   	   */
/***************************************************************************/
uint32_t power(uint32_t ulXNumber, uint8_t ulYNumber);
uint16_t ConvertID(uint8_t * pData, uint8_t len);
void myMemCpy(void *dest, void *src, size_t n);
int16_t LinearSerachID(IRData_t * pData, uint8_t ubSize, uint16_t uwKey);
uint32_t StrToInt(uint8_t * str, uint8_t ubLen);
bool CheckStringIsNumber(uint8_t * pData, uint8_t len);
bool ConvertCharToHex(uint8_t ubData, uint8_t * ubResult);
bool ConvertStringToHex(uint8_t * pData, uint8_t len, uint8_t * ubResult);
bool ConvertStringToHex32Bit(uint8_t * pData, uint8_t len, uint32_t * ulResult);
uint64_t ConvertStringToTimeStamp(uint8_t * pData, uint8_t len);
uint8_t is_delim(char c, char *delim);
char * my_strtok(char *srcString, char *delim);

#endif // _OP_HEADER_FUNCTION_H_
