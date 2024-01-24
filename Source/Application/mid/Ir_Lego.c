#include "main.h"

//#define DEBUG_LEGO

PulseDistanceWidthProtocolConstants LegoProtocolConstants = { LEGO_PF, 38, {LEGO_HEADER_MARK, LEGO_HEADER_SPACE, LEGO_BIT_MARK,
LEGO_ONE_SPACE, LEGO_BIT_MARK, LEGO_ZERO_SPACE}, PROTOCOL_IS_LSB_FIRST, (LEGO_AUTO_REPEAT_PERIOD_MIN
        / MICROS_IN_ONE_MILLI), NULL };

/************************************
 * Start of send and decode functions
 ************************************/
/*
 * Here we process the structured data, and call the send raw data function
 * @param aMode one of LEGO_MODE_EXTENDED, LEGO_MODE_COMBO, LEGO_MODE_SINGLE
 */
void sendLegoPowerFunctions(uint8_t aChannel, uint8_t aCommand, uint8_t aMode, bool aDoSend5Times)
{
    aChannel &= 0x0F; // allow toggle and escape bits too
    aCommand &= 0x0F;
    aMode &= 0x0F;
    uint8_t tParity = 0xF ^ aChannel ^ aMode ^ aCommand;
    // send 4 bit channel, 4 bit mode, 4 bit command, 4 bit parity
    uint16_t tRawData = (((aChannel << LEGO_MODE_BITS) | aMode) << (LEGO_COMMAND_BITS + LEGO_PARITY_BITS))
            | (aCommand << LEGO_PARITY_BITS) | tParity;
    sendLegoPowerFunctionsV2(tRawData, aChannel, aDoSend5Times);
}

void sendLegoPowerFunctionsV2(uint16_t aRawData, uint8_t aChannel, bool aDoSend5Times)
{
#ifdef DEBUG_LEGO
    NRF_LOG_INFO("sendLego aRawData = 0x%X", aRawData);
#endif

    aChannel &= 0x03; // we have 4 channels

    uint_fast8_t tNumberOfRepeats = 0;
    if (aDoSend5Times) {
        tNumberOfRepeats = 4;
    }
// required for repeat timing, see http://www.hackvandedam.nl/blog/?page_id=559
    uint8_t tRepeatPeriod = (LEGO_AUTO_REPEAT_PERIOD_MIN / MICROS_IN_ONE_MILLI) + (aChannel * 40); // from 110 to 230
    LegoProtocolConstants.RepeatPeriodMillis = tRepeatPeriod;
    sendPulseDistanceWidthV1(&LegoProtocolConstants, aRawData, LEGO_BITS, tNumberOfRepeats);
}

/*
 * Mode is stored in the upper nibble of command
 */
bool decodeLegoPowerFunctions(void)
{
    if (!checkHeader(&LegoProtocolConstants)) {
        return false;
    }

    // Check we have enough data - +4 for initial gap, start bit mark and space + stop bit mark
    if (decodedIRData.rawDataPtr->rawlen != (2 * LEGO_BITS) + 4)
    {
#ifdef DEBUG_LEGO
        NRF_LOG_INFO("LEGO: Data length = %d is not 36", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    if (!decodePulseDistanceWidthDatav3(&LegoProtocolConstants, LEGO_BITS))
    {
#ifdef DEBUG_LEGO
    	NRF_LOG_INFO("LEGO: Decode failed");
#endif
        return false;
    }

    // Stop bit
    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[3 + (2 * LEGO_BITS)], LEGO_BIT_MARK))
    {
#ifdef DEBUG_LEGO
    	NRF_LOG_INFO("LEGO: Stop bit mark length is wrong");
#endif
        return false;
    }

    // Success
    decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;
    uint16_t tDecodedValue = decodedIRData.decodedRawData;
    uint8_t tToggleEscapeChannel = tDecodedValue >> (LEGO_MODE_BITS + LEGO_COMMAND_BITS + LEGO_PARITY_BITS);
    uint8_t tMode = (tDecodedValue >> (LEGO_COMMAND_BITS + LEGO_PARITY_BITS)) & 0xF;
    uint8_t tData = (tDecodedValue >> LEGO_PARITY_BITS) & 0xF; // lego calls this field "data"
    uint8_t tParityReceived = tDecodedValue & 0xF;

    // This is parity as defined in the specifications
    // But in some scans I saw 0x9 ^ .. as parity formula
    uint8_t tParityComputed = 0xF ^ tToggleEscapeChannel ^ tMode ^ tData;

    // parity check
    if (tParityReceived != tParityComputed)
    {
#ifdef DEBUG_LEGO
    	NRF_LOG_INFO("LEGO: Parity is not correct. expected=0x%X", tParityComputed);
    	NRF_LOG_INFO("Received=0x%X", tParityReceived);
    	NRF_LOG_INFO("raw=0x%X", tDecodedValue);
    	NRF_LOG_INFO("3 nibbles are 0x%X, 0x%X, 0x%X", tToggleEscapeChannel, tMode, tData);
#endif
        // might not be an error, so just continue
        decodedIRData.flags = IRDATA_FLAGS_PARITY_FAILED | IRDATA_FLAGS_IS_MSB_FIRST;
    }

    /*
     * Check for autorepeat (should happen 4 times for one press)
     */
    if (decodedIRData.rawDataPtr->rawbuf[0] < (LEGO_AUTO_REPEAT_PERIOD_MAX / MICROS_PER_TICK)) {
        decodedIRData.flags |= IRDATA_FLAGS_IS_AUTO_REPEAT;
    }
    decodedIRData.address = tToggleEscapeChannel;
    decodedIRData.command = tData | tMode << LEGO_COMMAND_BITS;
    decodedIRData.numberOfBits = LEGO_BITS;
    decodedIRData.protocol = LEGO_PF;

    return true;
}
