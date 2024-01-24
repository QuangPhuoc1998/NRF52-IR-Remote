#include "main.h"

//#define DEBUG_FAST

PulseDistanceWidthProtocolConstants FASTProtocolConstants = { FAST, FAST_KHZ, {FAST_HEADER_MARK, FAST_HEADER_SPACE,
FAST_BIT_MARK, FAST_ONE_SPACE, FAST_BIT_MARK, FAST_ZERO_SPACE}, PROTOCOL_IS_LSB_FIRST, (FAST_REPEAT_PERIOD / MICROS_IN_ONE_MILLI),
NULL };

/************************************
 * Start of send and decode functions
 ************************************/

/**
 * The FAST protocol repeats by skipping the header mark and space -> this leads to a poor repeat detection for JVC protocol.
 */
void sendFAST(uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    // Set IR carrier frequency
    enableIROut(FAST_KHZ); // 38 kHz

    uint_fast8_t tNumberOfCommands = aNumberOfRepeats + 1;
    while (tNumberOfCommands > 0) {

        mark(FAST_HEADER_MARK);
        space(FAST_HEADER_SPACE);

        sendPulseDistanceWidthDataV3(&FASTProtocolConstants, aCommand | (((uint8_t)(~aCommand)) << 8), FAST_BITS);

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            // send repeated command in a fixed raster
            nrf_delay_ms(FAST_REPEAT_DISTANCE / MICROS_IN_ONE_MILLI);
        }
    }
}

bool decodeFAST(void)
{

//    uint_fast8_t tRawlen = decodedIRData.rawDataPtr->rawlen; // Using a local variable does not improve code size

    // Check we have the right amount of data (36). The +4 is for initial gap, start bit mark and space + stop bit mark.
    if (decodedIRData.rawDataPtr->rawlen != ((2 * FAST_BITS) + 4))
    {
#ifdef DEBUG_FAST
        NRF_LOG_INFO("FAST: Data length = %d is not 36", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    if (!checkHeader(&FASTProtocolConstants)) {
        return false;
    }

    if (!decodePulseDistanceWidthDatav3(&FASTProtocolConstants, FAST_BITS)) {
#ifdef DEBUG_FAST
    	NRF_LOG_INFO("FAST: Decode failed");
#endif
        return false;
    }

    union WordUnion tValue;
    tValue.UWord = decodedIRData.decodedRawData;

    if (tValue.UByte.LowByte != (uint8_t)~(tValue.UByte.HighByte)) {
#ifdef DEBUG_FAST
    	NRF_LOG_INFO("FAST: 8 bit parity is not correct. Expected=0x%X", (uint8_t)~(tValue.UByte.LowByte));
    	NRF_LOG_INFO("Received=0x%X data = 0x%X", tValue.UByte.HighByte, tValue.UWord);
#endif
        decodedIRData.flags = IRDATA_FLAGS_PARITY_FAILED;
    }

    checkForRepeatSpaceTicksAndSetFlag(FAST_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);

    // Success
//    decodedIRData.flags = IRDATA_FLAGS_IS_LSB_FIRST; // Not required, since this is the start value
    decodedIRData.command = tValue.UByte.LowByte;
    decodedIRData.address = 0; // No address for this protocol
    decodedIRData.numberOfBits = FAST_BITS;
    decodedIRData.protocol = FAST;

    return true;
}
