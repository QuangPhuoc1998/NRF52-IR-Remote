#include "main.h"

uint8_t repeatCount;        // Used e.g. for Denon decode for autorepeat decoding.

PulseDistanceWidthProtocolConstants DenonProtocolConstants = { DENON, DENON_KHZ, {DENON_HEADER_MARK, DENON_HEADER_SPACE,
DENON_BIT_MARK, DENON_ONE_SPACE, DENON_BIT_MARK, DENON_ZERO_SPACE }, PROTOCOL_IS_LSB_FIRST,
        (DENON_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), NULL };

/************************************
 * Start of send and decode functions
 ************************************/

void sendSharp(uint8_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    sendDenon(aAddress, aCommand, aNumberOfRepeats, true);
}

void sendDenon(uint8_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, bool aSendSharp)
{
    // Set IR carrier frequency
    enableIROut (DENON_KHZ); // 38 kHz

    // Add frame marker for sharp
    uint16_t tCommand = aCommand;
    if (aSendSharp) {
        tCommand |= 0x0200; // the 2 upper bits are 00 for Denon and 10 for Sharp
    }
    uint16_t tData = aAddress | ((uint16_t) tCommand << DENON_ADDRESS_BITS);
    uint16_t tInvertedData = (tData ^ 0x7FE0); // Command and frame (upper 10 bits) are inverted

    uint8_t tNumberOfCommands = aNumberOfRepeats + 1;
    while (tNumberOfCommands > 0) {

        // Data
        sendPulseDistanceWidthDataV3(&DenonProtocolConstants, tData, DENON_BITS);

        // Inverted autorepeat frame
        nrf_delay_ms(DENON_AUTO_REPEAT_DISTANCE / MICROS_IN_ONE_MILLI);
        sendPulseDistanceWidthDataV3(&DenonProtocolConstants, tInvertedData, DENON_BITS);

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            // send repeated command with a fixed space gap
            nrf_delay_ms( DENON_AUTO_REPEAT_DISTANCE / MICROS_IN_ONE_MILLI);
        }
    }
}

bool decodeSharp(void)
{
    return decodeDenon();
}

bool decodeDenon(void)
{

    // we have no start bit, so check for the exact amount of data bits
    // Check we have the right amount of data (32). The + 2 is for initial gap + stop bit mark
    if (decodedIRData.rawDataPtr->rawlen != (2 * DENON_BITS) + 2) {
//        IR_DEBUG_PRINT(F("Denon: "));
//        IR_DEBUG_PRINT(F("Data length="));
//        IR_DEBUG_PRINT(decodedIRData.rawDataPtr->rawlen);
//        IR_DEBUG_PRINTLN(F(" is not 32"));
        return false;
    }

    // Try to decode as Denon protocol
    if (!decodePulseDistanceWidthDatav2(&DenonProtocolConstants, DENON_BITS, 1)) {
#if defined(LOCAL_DEBUG)
        Serial.print(F("Denon: "));
        Serial.println(F("Decode failed"));
#endif
        return false;
    }

    // Check for stop mark
    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[(2 * DENON_BITS) + 1], DENON_HEADER_MARK)) {
#if defined(LOCAL_DEBUG)
        Serial.print(F("Denon: "));
        Serial.println(F("Stop bit mark length is wrong"));
#endif
        return false;
    }

    // Success
    decodedIRData.address = decodedIRData.decodedRawData & 0x1F;
    decodedIRData.command = decodedIRData.decodedRawData >> DENON_ADDRESS_BITS;
    uint8_t tFrameBits = (decodedIRData.command >> 8) & 0x03;
    decodedIRData.command &= 0xFF;

    // Check for (auto) repeat
    if (decodedIRData.rawDataPtr->rawbuf[0] < ((DENON_AUTO_REPEAT_DISTANCE + (DENON_AUTO_REPEAT_DISTANCE / 4)) / MICROS_PER_TICK)) {
        repeatCount++;
        if (repeatCount > 1) { // skip first auto repeat
            decodedIRData.flags = IRDATA_FLAGS_IS_REPEAT;
        }

        if (tFrameBits & 0x01) {
            /*
             * Here we are in the auto repeated frame with the inverted command
             */
#if defined(LOCAL_DEBUG)
                Serial.print(F("Denon: "));
                Serial.println(F("Autorepeat received="));
#endif
            decodedIRData.flags |= IRDATA_FLAGS_IS_AUTO_REPEAT;
            // Check parity of consecutive received commands. There is no parity in one data set.
            if ((uint8_t) lastDecodedCommand != (uint8_t)(~decodedIRData.command)) {
                decodedIRData.flags |= IRDATA_FLAGS_PARITY_FAILED;
#if defined(LOCAL_DEBUG)
                Serial.print(F("Denon: "));
                Serial.print(F("Parity check for repeat failed. Last command="));
                Serial.print(lastDecodedCommand, HEX);
                Serial.print(F(" current="));
                Serial.println(~decodedIRData.command, HEX);
#endif
            }
            // always take non inverted command
            decodedIRData.command = lastDecodedCommand;
        }

        // repeated command here
        if (tFrameBits == 1 || tFrameBits == 2) {
            decodedIRData.protocol = SHARP;
        } else {
            decodedIRData.protocol = DENON;
        }
    } else {
        repeatCount = 0;
        // first command here
        if (tFrameBits == 2) {
            decodedIRData.protocol = SHARP;
        } else {
            decodedIRData.protocol = DENON;
        }
    }

    decodedIRData.numberOfBits = DENON_BITS;

    return true;
}
