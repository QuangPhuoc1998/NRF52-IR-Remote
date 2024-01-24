#include "main.h"

#define DEBUG_IR_SEND

void Mid_IrSendCommon(IRData * tIrDataToSend, uint8_t aNumberOfRepeats)
{
	bool tIsRepeat = (tIrDataToSend->flags & IRDATA_FLAGS_IS_REPEAT);

	switch (tIrDataToSend->protocol)
	{
		case NEC:
		{
			sendNEC(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case SAMSUNG:
		{
			sendSamsung(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case SAMSUNG48:
		{
			sendSamsung48(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case SAMSUNG_LG:
		{
			sendSamsungLG(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case SONY:
		{
			sendSony(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats, tIrDataToSend->numberOfBits);
			break;
		}
		case PANASONIC:
		{
			Mid_IrSendPanasonic(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case DENON:
		{
			sendDenon(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats, false);
			break;
		}
		case SHARP:
		{
			sendSharp(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case LG:
		{
			sendLG((uint8_t) tIrDataToSend->address, (uint8_t) tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case JVC:
		{
			sendJVC((uint8_t) tIrDataToSend->address, (uint8_t) tIrDataToSend->command, aNumberOfRepeats); // casts are required to specify the right function
			break;
		}
		case RC5:
		{
			sendRC5(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats, !tIsRepeat); // No toggle for repeats
			break;
		}
		case RC6:
		{
			sendRC6(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats, !tIsRepeat); // No toggle for repeats
			break;
		}
		case KASEIKYO_JVC:
		{
			Mid_IrSendKaseikyo_JVC(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case KASEIKYO_DENON:
		{
			Mid_IrSendKaseikyo_Denon(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case KASEIKYO_SHARP:
		{
			sendKaseikyo_Sharp(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case KASEIKYO_MITSUBISHI:
		{
			Mid_IrSendKaseikyo_Mitsubishi(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case NEC2:
		{
			sendNEC2(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case ONKYO:
		{
			sendOnkyo(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case APPLE:
		{
			sendApple(tIrDataToSend->address, tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case BOSEWAVE:
		{
			sendBoseWave(tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case MAGIQUEST:
		{
			// we have a 32 bit ID/address
			sendMagiQuest(tIrDataToSend->decodedRawData, tIrDataToSend->command);
			break;
		}
		case FAST:
		{
			sendFAST(tIrDataToSend->command, aNumberOfRepeats);
			break;
		}
		case LEGO_PF:
		{
			sendLegoPowerFunctions(tIrDataToSend->address, tIrDataToSend->command, tIrDataToSend->command >> 4, tIsRepeat); // send 5 autorepeats
			break;
		}
		case PULSE_DISTANCE:
		{
			Mid_IrSendPulseDistance(tIrDataToSend);
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
