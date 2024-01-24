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
void App_ControlStartEmitIR(void *pData, uint16_t len);
void App_ControlStartEmitIRWithID(void *pData, uint16_t len);
void App_ControlEraseIR(uint16_t uwIrID);
/***************************************************************************/
/*					   	HEADER FOR APP ALARM							   */
/***************************************************************************/
void App_AlarmInit(void);
void App_AlarmOnTimeHandler(void);
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
void sendPulseDistanceWidthFromArrayV1(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
		uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros,
		IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, bool aMSBFirst, bool aSendStopBit,
		uint16_t aRepeatPeriodMillis, uint8_t aNumberOfRepeats);
void sendPulseDistanceWidthFromArrayV2(uint8_t aFrequencyKHz, struct DistanceWidthTimingInfoStruct *aDistanceWidthTimingInfo,
		IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, uint8_t aFlags, uint16_t aRepeatPeriodMillis,
		uint8_t aNumberOfRepeats);
void sendPulseDistanceWidthFromArrayV3(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
		uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros,
		IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, uint8_t aFlags, uint16_t aRepeatPeriodMillis,
		uint8_t aNumberOfRepeats);
void sendPulseDistanceWidthV1(PulseDistanceWidthProtocolConstants *aProtocolConstants, IRRawDataType aData,
		uint8_t aNumberOfBits, uint8_t aNumberOfRepeats);
void sendPulseDistanceWidthV2(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
		uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros, IRRawDataType aData,
		uint8_t aNumberOfBits, bool aMSBFirst, bool aSendStopBit, uint16_t aRepeatPeriodMillis, int_fast8_t aNumberOfRepeats,
		void (*aSpecialSendRepeatFunction)());
void sendPulseDistanceWidthV3(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
		uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros, IRRawDataType aData,
		uint8_t aNumberOfBits, uint8_t aFlags, uint16_t aRepeatPeriodMillis, uint8_t aNumberOfRepeats,
        void (*aSpecialSendRepeatFunction)());
void enableIROut(uint_fast8_t aFrequencyKHz);
void mark(uint16_t aMarkMicros);
void space(uint16_t aSpaceMicros);
void customDelayMicroseconds(unsigned long aMicroseconds);
bool checkHeader(PulseDistanceWidthProtocolConstants *aProtocolConstants);

void sendPulseDistanceWidthDataV3(PulseDistanceWidthProtocolConstants *aProtocolConstants, IRRawDataType aData,
        uint8_t aNumberOfBits);
void sendPulseDistanceWidthDataV2(uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros,
        uint16_t aZeroSpaceMicros, IRRawDataType aData, uint8_t aNumberOfBits, bool aMSBFirst, bool aSendStopBit);
void sendPulseDistanceWidthDataV1(uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros,
        uint16_t aZeroSpaceMicros, IRRawDataType aData, uint8_t aNumberOfBits, uint8_t aFlags);

void sendBiphaseData(uint16_t aBiphaseTimeUnit, uint32_t aData, uint8_t aNumberOfBits);
bool matchTicks(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool MATCH(uint16_t measured_ticks, uint16_t desired_us);
bool matchMark(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool MATCH_MARK(uint16_t measured_ticks, uint16_t desired_us);
bool matchSpace(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool MATCH_SPACE(uint16_t measured_ticks, uint16_t desired_us);
int getMarkExcessMicros();

void checkForRepeatSpaceTicksAndSetFlag(uint16_t aMaximumRepeatSpaceTicks) ;
bool decodePulseDistanceWidthData(uint8_t aNumberOfBits, uint8_t aStartOffset, uint16_t aOneMarkMicros,
        uint16_t aZeroMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroSpaceMicros, bool aMSBfirst) ;
bool decodePulseDistanceWidthDatav2(PulseDistanceWidthProtocolConstants *aProtocolConstants, uint8_t aNumberOfBits, uint8_t aStartOffset);
bool decodePulseDistanceWidthDatav3(PulseDistanceWidthProtocolConstants *aProtocolConstants, uint8_t aNumberOfBits);

void initBiphaselevel(uint8_t aRCDecodeRawbuffOffset, uint16_t aBiphaseTimeUnit);
uint8_t getBiphaselevel(void);

uint8_t compare(uint16_t oldval, uint16_t newval);
bool decodeHash(void);
/***************************************************************************/
/*					   		HEADER FOR DENON							   */
/***************************************************************************/
void sendSharp(uint8_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void sendDenon(uint8_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, bool aSendSharp);
bool decodeSharp(void);
bool decodeDenon(void);
/***************************************************************************/
/*					   		HEADER FOR KASEIKYO							   */
/***************************************************************************/
bool decodeKaseikyo(void);
void sendKaseikyo(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, uint16_t aVendorCode);
/***************************************************************************/
/*					   			HEADER FOR SONY					           */
/***************************************************************************/
bool decodeSony(void);
void sendSony(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, uint8_t numberOfBits);
/***************************************************************************/
/*					   		HEADER FOR RC5 C6					           */
/***************************************************************************/
bool decodeRC5(void);
void sendRC5(uint8_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, bool aEnableAutomaticToggle);
bool decodeRC6(void);
void sendRC6With32Bit(uint32_t aRawData, uint8_t aNumberOfBitsToSend);
void sendRC6RawWith32Bit(uint32_t aRawData, uint8_t aNumberOfBitsToSend);
void sendRC6With64Bit(uint64_t aRawData, uint8_t aNumberOfBitsToSend);
void sendRC6RawWith64Bit(uint64_t aRawData, uint8_t aNumberOfBitsToSend);
void sendRC6(uint8_t aAddress, uint8_t aCommand, int_fast8_t aNumberOfRepeats, bool aEnableAutomaticToggle);
/***************************************************************************/
/*					   			HEADER FOR LG					           */
/***************************************************************************/
void sendLG2Repeat(void);
void sendLG2SpecialRepeat(void);
uint32_t computeLGRawDataAndChecksum(uint8_t aAddress, uint16_t aCommand);
void sendLG(uint8_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats);
void sendLG2(uint8_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats);
bool decodeLG(void);
/***************************************************************************/
/*					   			HEADER FOR JVC					           */
/***************************************************************************/
void sendJVC(uint8_t aAddress, uint8_t aCommand, int_fast8_t aNumberOfRepeats);
bool decodeJVC(void);
/***************************************************************************/
/*					   		HEADER FOR SAMSUNG					           */
/***************************************************************************/
void sendSamsungLGRepeat();
void sendSamsungLGSpecialRepeat();
void sendSamsungLG(uint16_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats);
void sendSamsung(uint16_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats);
void sendSamsung48(uint16_t aAddress, uint32_t aCommand, uint8_t aNumberOfRepeats);
bool decodeSamsung(void);
/***************************************************************************/
/*					   		HEADER FOR BANGOLUFSEN					       */
/***************************************************************************/
void sendBangOlufsen(uint16_t aHeader, uint8_t aData, uint8_t aNumberOfRepeats, int8_t aNumberOfHeaderBits);
void sendBangOlufsenDataLink(uint32_t aHeader, uint8_t aData, uint8_t aNumberOfRepeats, int8_t aNumberOfHeaderBits);
/***************************************************************************/
/*					   		HEADER FOR FAST							       */
/***************************************************************************/
void sendFAST(uint8_t aCommand, uint8_t aNumberOfRepeats);
bool decodeFAST(void);
/***************************************************************************/
/*					   	HEADER FOR WHYNTER							       */
/***************************************************************************/
void sendWhynter(uint32_t aData, uint8_t aNumberOfBitsToSend);
bool decodeWhynter(void);
/***************************************************************************/
/*					   		HEADER FOR LEGO							       */
/***************************************************************************/
void sendLegoPowerFunctions(uint8_t aChannel, uint8_t aCommand, uint8_t aMode, bool aDoSend5Times);
void sendLegoPowerFunctionsV2(uint16_t aRawData, uint8_t aChannel, bool aDoSend5Times);
bool decodeLegoPowerFunctions(void);
/***************************************************************************/
/*					   		HEADER FOR BOSEWAVE							   */
/***************************************************************************/
void sendBoseWave(uint8_t aCommand, uint8_t aNumberOfRepeats);
bool decodeBoseWave(void);
/***************************************************************************/
/*					   		HEADER FOR MAGIQUEST						   */
/***************************************************************************/
void sendMagiQuest(uint32_t aWandId, uint16_t aMagnitude);
bool decodeMagiQuest(void);
/***************************************************************************/
/*					   		HEADER FOR PULSE DISTANCE					   */
/***************************************************************************/
bool aggregateArrayCounts(uint8_t aArray[], uint8_t aMaxIndex, uint8_t *aShortIndex, uint8_t *aLongIndex);
bool decodeDistanceWidth();
/***************************************************************************/
/*					   			HEADER FOR NEC					           */
/***************************************************************************/
void sendNECRepeat(void);
void sendNECSpecialRepeat(void);
bool decodeNec(void);
uint32_t computeNECRawDataAndChecksum(uint16_t aAddress, uint16_t aCommand);
void sendNEC(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void sendNEC2(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void sendOnkyo(uint16_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats);
void sendApple(uint8_t aDeviceId, uint8_t aCommand, uint8_t aNumberOfRepeats);
void sendNECRaw(uint32_t aRawData, uint8_t aNumberOfRepeats);
/***************************************************************************/
/*					   		HEADER FOR IR SEND							   */
/***************************************************************************/
void Mid_IrSendCommon(IRData * tIrDataToSend, uint8_t aNumberOfRepeats);

void Mid_IrSendPanasonic(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void Mid_IrSendKaseikyo_Denon(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void Mid_IrSendKaseikyo_Mitsubishi(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void Mid_IrSendKaseikyo_Sharp(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void sendKaseikyo_Sharp(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
void Mid_IrSendKaseikyo_JVC(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);

void Mid_IrSendPulseDistance(IRData * tIRDataToSend);
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
bool Mid_FlashWriteValue(uint32_t ulAddress, uint8_t ubValue, uint32_t ulLen);
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
void Mid_EraseAllITrigger(void);

bool Mid_InsertNewIRCode(IRData * tIRData, uint16_t uwLearnID);
bool Mid_RemoveIRCode(uint16_t ulEraseID);
void Mid_EraseAllIRCode(void);
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
void myMemSet(void *dest, uint8_t ubData, size_t n);
int16_t LinearSerachID(IRData_t * pData, uint8_t ubSize, uint16_t uwKey);
uint32_t StrToInt(uint8_t * str, uint8_t ubLen);
bool CheckStringIsNumber(uint8_t * pData, uint8_t len);
bool ConvertCharToHex(uint8_t ubData, uint8_t * ubResult);
bool ConvertStringToHex(uint8_t * pData, uint8_t len, uint8_t * ubResult);
bool ConvertStringToHex32Bit(uint8_t * pData, uint8_t len, uint32_t * ulResult);
uint64_t ConvertStringToTimeStamp(uint8_t * pData, uint8_t len);
uint8_t is_delim(char c, char *delim);
char * my_strtok(char *srcString, char *delim);
uint16_t ConvertArrayToNumber(uint8_t * pData, uint8_t ubLen);

#endif // _OP_HEADER_FUNCTION_H_
