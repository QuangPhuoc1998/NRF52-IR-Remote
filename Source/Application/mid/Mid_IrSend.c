#include "main.h"

#define DEBUG_IR_SEND

void Mid_IrSendCommon(IRData * tIrDataToSend)
{
	switch (tIrDataToSend->protocol)
	{
		case PULSE_DISTANCE:
		{
			Mid_IrSendPulseDistance(tIrDataToSend);
			break;
		}
		case PANASONIC:
		{
			Mid_IrSendPanasonic(tIrDataToSend->address, tIrDataToSend->command, 1);
			break;
		}
		default:
			break;
	}
}

/***************************************************************************/
/*					   	SEND IR FUNCTION FOR KASEIKYO					   */
/***************************************************************************/
void Mid_IrSendPanasonic(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    sendKaseikyo(aAddress, aCommand, aNumberOfRepeats, PANASONIC_VENDOR_ID_CODE);
}

void Mid_IrSendKaseikyo_Denon(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    sendKaseikyo(aAddress, aCommand, aNumberOfRepeats, DENON_VENDOR_ID_CODE);
}

void Mid_IrSendKaseikyo_Mitsubishi(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    sendKaseikyo(aAddress, aCommand, aNumberOfRepeats, MITSUBISHI_VENDOR_ID_CODE);
}

void sendKaseikyo_Sharp(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    sendKaseikyo(aAddress, aCommand, aNumberOfRepeats, SHARP_VENDOR_ID_CODE);
}

void Mid_IrSendKaseikyo_JVC(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    sendKaseikyo(aAddress, aCommand, aNumberOfRepeats, JVC_VENDOR_ID_CODE);
}

/***************************************************************************/
/*					  SEND IR FUNCTION FOR PULSE DISTANCE				   */
/***************************************************************************/
void Mid_IrSendPulseDistance(IRData * tIRDataToSend)
{
	IRRawDataType sDecodedRawDataArray[RAW_DATA_ARRAY_SIZE];
	struct DistanceWidthTimingInfoStruct sDistanceWidthTimingInfo;
	uint8_t sNumberOfBits = 32;

	sDistanceWidthTimingInfo = tIRDataToSend->DistanceWidthTimingInfo;
	sNumberOfBits = tIRDataToSend->numberOfBits;
	//*sDecodedRawDataArray = *(tIRDataToSend->decodedRawDataArray); // copy content here
	myMemCpy(sDecodedRawDataArray, tIRDataToSend->decodedRawDataArray, sizeof(sDecodedRawDataArray));

#ifdef DEBUG_IR_SEND
	NRF_LOG_INFO("Num bit: %d", sNumberOfBits);
	NRF_LOG_INFO("Raw: 0x%X", sDecodedRawDataArray[0]);
	NRF_LOG_INFO("Raw: 0x%X", sDecodedRawDataArray[1]);
	NRF_LOG_INFO("Raw: 0x%X", sDecodedRawDataArray[2]);
	NRF_LOG_INFO("Raw: 0x%X", sDecodedRawDataArray[3]);
#endif

	sendPulseDistanceWidthFromArrayV2(38, &sDistanceWidthTimingInfo, &sDecodedRawDataArray[0], sNumberOfBits,
#if defined(USE_MSB_DECODING_FOR_DISTANCE_DECODER)
				PROTOCOL_IS_MSB_FIRST
#else
				PROTOCOL_IS_LSB_FIRST
#endif
				, 100, 0);
}
