#include "main.h"

//#define DEBUG_BOSEWAVE

PulseDistanceWidthProtocolConstants BoseWaveProtocolConstants = { BOSEWAVE, BOSEWAVE_KHZ, {BOSEWAVE_HEADER_MARK,
BOSEWAVE_HEADER_SPACE, BOSEWAVE_BIT_MARK, BOSEWAVE_ONE_SPACE, BOSEWAVE_BIT_MARK, BOSEWAVE_ZERO_SPACE}, PROTOCOL_IS_LSB_FIRST
       , (BOSEWAVE_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), NULL };

/************************************
 * Start of send and decode functions
 ************************************/

void sendBoseWave(uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    // send 8 command bits and then 8 inverted command bits LSB first
    uint16_t tData = ((~aCommand) << 8) | aCommand;
    sendPulseDistanceWidthV1(&BoseWaveProtocolConstants, tData, BOSEWAVE_BITS, aNumberOfRepeats);
}

bool decodeBoseWave(void)
{
    if (!checkHeader(&BoseWaveProtocolConstants)) {
        return false;
    }

    // Check we have enough data +4 for initial gap, start bit mark and space + stop bit mark
    if (decodedIRData.rawDataPtr->rawlen != (2 * BOSEWAVE_BITS) + 4)
    {
#ifdef DEBUG_BOSEWAVE
    	NRF_LOG_INFO("Bose: Data length = %d is not 36", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    if (!decodePulseDistanceWidthDatav3(&BoseWaveProtocolConstants, BOSEWAVE_BITS))
    {
#ifdef DEBUG_BOSEWAVE
    	NRF_LOG_INFO("Bose: Decode failed");
#endif
        return false;
    }

    // Stop bit
    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[3 + (2 * BOSEWAVE_BITS)], BOSEWAVE_BIT_MARK))
    {
#ifdef DEBUG_BOSEWAVE
    	NRF_LOG_INFO("Bose: Stop bit mark length is wrong");
#endif
        return false;
    }

    // Success
//    decodedIRData.flags = IRDATA_FLAGS_IS_LSB_FIRST; // Not required, since this is the start value
    uint16_t tDecodedValue = decodedIRData.decodedRawData;
    uint8_t tCommandNotInverted = tDecodedValue & 0xFF; // comes first and is in the lower bits (LSB first :-))
    uint8_t tCommandInverted = tDecodedValue >> 8;
    // parity check for command. Use this variant to avoid compiler warning "comparison of promoted ~unsigned with unsigned [-Wsign-compare]"
    if ((tCommandNotInverted ^ tCommandInverted) != 0xFF) {
#ifdef DEBUG_BOSEWAVE
    	NRF_LOG_INFO("Bose: Command and inverted command check failed");
#endif
        return false;
    }
    decodedIRData.command = tCommandNotInverted;
    decodedIRData.numberOfBits = BOSEWAVE_BITS;
    decodedIRData.protocol = BOSEWAVE;

    // check for repeat
    checkForRepeatSpaceTicksAndSetFlag(BOSEWAVE_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);

    return true;
}
