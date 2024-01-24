#include "main.h"

PulseDistanceWidthProtocolConstants WhynterProtocolConstants = { WHYNTER, 38, {WHYNTER_HEADER_MARK, WHYNTER_HEADER_SPACE,
WHYNTER_BIT_MARK, WHYNTER_ONE_SPACE, WHYNTER_BIT_MARK, WHYNTER_ZERO_SPACE}, PROTOCOL_IS_MSB_FIRST, 110, NULL };

void sendWhynter(uint32_t aData, uint8_t aNumberOfBitsToSend)
{
	sendPulseDistanceWidthV1(&WhynterProtocolConstants, aData, NEC_BITS, aNumberOfBitsToSend);
}

bool decodeWhynter(void)
{
    // Check we have the right amount of data (68). The +4 is for initial gap, start bit mark and space + stop bit mark.
    if (decodedIRData.rawDataPtr->rawlen != (2 * WHYNTER_BITS) + 4) {
        return false;
    }
    if (!checkHeader(&WhynterProtocolConstants)) {
        return false;
    }
    if (!decodePulseDistanceWidthDatav3(&WhynterProtocolConstants, WHYNTER_BITS)) {
        return false;
    }
    // Success
    decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;
    decodedIRData.numberOfBits = WHYNTER_BITS;
    decodedIRData.protocol = WHYNTER;
    return true;
}
