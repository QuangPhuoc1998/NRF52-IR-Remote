#include "main.h"

#define DEBUG_SONY

PulseDistanceWidthProtocolConstants SonyProtocolConstants = { SONY, SONY_KHZ, {SONY_HEADER_MARK, SONY_SPACE, SONY_ONE_MARK,
SONY_SPACE, SONY_ZERO_MARK, SONY_SPACE }, PROTOCOL_IS_LSB_FIRST, (SONY_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), NULL };

/************************************
 * Start of send and decode functions
 ************************************/

/**
 * @param numberOfBits should be one of SIRCS_12_PROTOCOL, SIRCS_15_PROTOCOL, SIRCS_20_PROTOCOL. Not checked! 20 -> send 13 address bits
 */
void sendSony(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, uint8_t numberOfBits)
{
    uint32_t tData = (uint32_t) aAddress << 7 | (aCommand & 0x7F);
    // send 5, 8, 13 address bits LSB first
    sendPulseDistanceWidthV1(&SonyProtocolConstants, tData, numberOfBits, aNumberOfRepeats);
}

bool decodeSony(void)
{

    if (!checkHeader(&SonyProtocolConstants)) {
        return false;
    }

    // Check we have enough data. +2 for initial gap and start bit mark and space minus the last/MSB space. NO stop bit! 26, 32, 42
    if (decodedIRData.rawDataPtr->rawlen != (2 * SONY_BITS_MIN) + 2 && decodedIRData.rawDataPtr->rawlen != (2 * SONY_BITS_MAX) + 2
            && decodedIRData.rawDataPtr->rawlen != (2 * SONY_BITS_15) + 2)
    {
#ifdef DEBUG_SONY
        NRF_LOG_INFO("Sony: Data length = %d is not 12, 15 or 20", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    if (!decodePulseDistanceWidthDatav2(&SonyProtocolConstants, (decodedIRData.rawDataPtr->rawlen - 1) / 2, 3)) {
#ifdef DEBUG_SONY
    	NRF_LOG_INFO("Sony: ");
    	NRF_LOG_INFO("Decode failed");
#endif
        return false;
    }

    // Success
//    decodedIRData.flags = IRDATA_FLAGS_IS_LSB_FIRST; // Not required, since this is the start value
    decodedIRData.command = decodedIRData.decodedRawData & 0x7F;  // first 7 bits
    decodedIRData.address = decodedIRData.decodedRawData >> 7;    // next 5 or 8 or 13 bits
    decodedIRData.numberOfBits = (decodedIRData.rawDataPtr->rawlen - 1) / 2;
    decodedIRData.protocol = SONY;

    //Check for repeat
    checkForRepeatSpaceTicksAndSetFlag(SONY_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);

    return true;
}
