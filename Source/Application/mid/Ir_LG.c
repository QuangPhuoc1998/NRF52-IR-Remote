#include "main.h"

PulseDistanceWidthProtocolConstants LGProtocolConstants = { LG, LG_KHZ, {LG_HEADER_MARK, LG_HEADER_SPACE, LG_BIT_MARK,
LG_ONE_SPACE, LG_BIT_MARK, LG_ZERO_SPACE}, PROTOCOL_IS_MSB_FIRST, (LG_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), &sendNECSpecialRepeat };

PulseDistanceWidthProtocolConstants LG2ProtocolConstants = { LG2, LG_KHZ, {LG2_HEADER_MARK, LG2_HEADER_SPACE, LG_BIT_MARK,
LG_ONE_SPACE, LG_BIT_MARK, LG_ZERO_SPACE}, PROTOCOL_IS_MSB_FIRST, (LG_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), &sendLG2SpecialRepeat };

/************************************
 * Start of send and decode functions
 ************************************/
/*
 * Send special LG2 repeat not used yet
 */
void sendLG2Repeat(void)
{
    enableIROut (LG_KHZ);            // 38 kHz
    mark(LG2_HEADER_MARK);          // + 3000
    space(LG_REPEAT_HEADER_SPACE);  // - 2250
    mark(LG_BIT_MARK);              // + 500
}

/**
 * Static function for sending special repeat frame.
 * For use in ProtocolConstants. Saves up to 250 bytes compared to a member function.
 */
void sendLG2SpecialRepeat(void)
{
    enableIROut(LG_KHZ);            // 38 kHz
    mark(LG2_HEADER_MARK);          // + 3000
    space(LG_REPEAT_HEADER_SPACE);  // - 2250
    mark(LG_BIT_MARK);              // + 500
}

uint32_t computeLGRawDataAndChecksum(uint8_t aAddress, uint16_t aCommand)
{
    uint32_t tRawData = ((uint32_t) aAddress << (LG_COMMAND_BITS + LG_CHECKSUM_BITS)) | ((uint32_t) aCommand << LG_CHECKSUM_BITS);
    /*
     * My guess of the 4 bit checksum
     * Addition of all 4 nibbles of the 16 bit command
     */
    uint8_t tChecksum = 0;
    uint16_t tTempForChecksum = aCommand;
    for (int i = 0; i < 4; ++i) {
        tChecksum += tTempForChecksum & 0xF; // add low nibble
        tTempForChecksum >>= 4; // shift by a nibble
    }
    return (tRawData | (tChecksum & 0xF));
}

/**
 * LG uses the NEC repeat.
 */
void sendLG(uint8_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats)
{
	sendPulseDistanceWidthV1(&LGProtocolConstants, computeLGRawDataAndChecksum(aAddress, aCommand), LG_BITS, aNumberOfRepeats);
}

/**
 * LG2 uses a special repeat.
 */
void sendLG2(uint8_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats)
{
	sendPulseDistanceWidthV1(&LG2ProtocolConstants, computeLGRawDataAndChecksum(aAddress, aCommand), LG_BITS, aNumberOfRepeats);
}

bool decodeLG(void)
{
    decode_type_t tProtocol = LG;
    uint16_t tHeaderSpace = LG_HEADER_SPACE;

    /*
     * First check for right data length
     * Next check start bit
     * Next try the decode
     */

// Check we have the right amount of data (60). The +4 is for initial gap, start bit mark and space + stop bit mark.
    if (decodedIRData.rawDataPtr->rawlen != ((2 * LG_BITS) + 4) && (decodedIRData.rawDataPtr->rawlen != 4)) {
//        IR_DEBUG_PRINT(F("LG: "));
//        IR_DEBUG_PRINT(F("Data length="));
//        IR_DEBUG_PRINT(decodedIRData.rawDataPtr->rawlen);
//        IR_DEBUG_PRINTLN(F(" is not 60 or 4"));
        return false;
    }

// Check header "mark" this must be done for repeat and data
    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[1], LG_HEADER_MARK)) {
        if (!matchMark(decodedIRData.rawDataPtr->rawbuf[1], LG2_HEADER_MARK)) {
#if defined(LOCAL_DEBUG)
            Serial.print(F("LG: "));
            Serial.println(F("Header mark is wrong"));
#endif
            return false;
        } else {
            tProtocol = LG2;
            tHeaderSpace = LG2_HEADER_SPACE;
        }
    }

// Check for repeat - here we have another header space length
    if (decodedIRData.rawDataPtr->rawlen == 4) {
        if (matchSpace(decodedIRData.rawDataPtr->rawbuf[2], LG_REPEAT_HEADER_SPACE)
                && matchMark(decodedIRData.rawDataPtr->rawbuf[3], LG_BIT_MARK)) {
            decodedIRData.flags = IRDATA_FLAGS_IS_REPEAT | IRDATA_FLAGS_IS_MSB_FIRST;
            decodedIRData.address = lastDecodedAddress;
            decodedIRData.command = lastDecodedCommand;
            decodedIRData.protocol = lastDecodedProtocol;
            return true;
        }
#if defined(LOCAL_DEBUG)
        Serial.print(F("LG: "));
        Serial.print(F("Repeat header space is wrong"));
#endif
        return false;
    }

// Check command header space
    if (!matchSpace(decodedIRData.rawDataPtr->rawbuf[2], tHeaderSpace)) {
#if defined(LOCAL_DEBUG)
        Serial.print(F("LG: "));
        Serial.println(F("Header space length is wrong"));
#endif
        return false;
    }

    if (!decodePulseDistanceWidthDatav3(&LGProtocolConstants, LG_BITS)) {
#if defined(LOCAL_DEBUG)
        Serial.print(F("LG: "));
        Serial.println(F("Decode failed"));
#endif
        return false;
    }

// Success
    decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;
    decodedIRData.command = (decodedIRData.decodedRawData >> LG_CHECKSUM_BITS) & 0xFFFF;
    decodedIRData.address = decodedIRData.decodedRawData >> (LG_COMMAND_BITS + LG_CHECKSUM_BITS); // first 8 bit

    /*
     * My guess of the checksum
     */
    uint8_t tChecksum = 0;
    uint16_t tTempForChecksum = decodedIRData.command;
    for (int i = 0; i < 4; ++i) {
        tChecksum += tTempForChecksum & 0xF; // add low nibble
        tTempForChecksum >>= 4; // shift by a nibble
    }
// Checksum check
    if ((tChecksum & 0xF) != (decodedIRData.decodedRawData & 0xF)) {
#if defined(LOCAL_DEBUG)
        Serial.print(F("LG: "));
        Serial.print(F("4 bit checksum is not correct. expected=0x"));
        Serial.print(tChecksum, HEX);
        Serial.print(F(" received=0x"));
        Serial.print((decodedIRData.decodedRawData & 0xF), HEX);
        Serial.print(F(" data=0x"));
        Serial.println(decodedIRData.command, HEX);
#endif
        decodedIRData.flags |= IRDATA_FLAGS_PARITY_FAILED;
    }

    decodedIRData.protocol = tProtocol; // LG or LG2
    decodedIRData.numberOfBits = LG_BITS;

    return true;
}

