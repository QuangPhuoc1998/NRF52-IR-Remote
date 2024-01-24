#include "main.h"

//#define DEBUG_SAMSUNG

PulseDistanceWidthProtocolConstants SamsungProtocolConstants = { SAMSUNG, SAMSUNG_KHZ, {SAMSUNG_HEADER_MARK,
SAMSUNG_HEADER_SPACE, SAMSUNG_BIT_MARK, SAMSUNG_ONE_SPACE, SAMSUNG_BIT_MARK, SAMSUNG_ZERO_SPACE}, PROTOCOL_IS_LSB_FIRST,
        (SAMSUNG_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), &sendSamsungLGSpecialRepeat };

/************************************
 * Start of send and decode functions
 ************************************/

/**
 * Send repeat
 * Repeat commands should be sent in a 110 ms raster.
 * This repeat was sent by an LG 6711R1P071A remote
 */
void sendSamsungLGRepeat()
{
    enableIROut (SAMSUNG_KHZ);       // 38 kHz
    mark(SAMSUNG_HEADER_MARK);      // + 4500
    space(SAMSUNG_HEADER_SPACE);    // - 4500
    mark(SAMSUNG_BIT_MARK);         // + 560
    space(SAMSUNG_ZERO_SPACE);      // - 560
    mark(SAMSUNG_BIT_MARK);         // + 560
}

/**
 * Static function for sending special repeat frame.
 * For use in ProtocolConstants. Saves up to 250 bytes compared to a member function.
 */
void sendSamsungLGSpecialRepeat()
{
    enableIROut(SAMSUNG_KHZ);      // 38 kHz
    mark(SAMSUNG_HEADER_MARK);     // + 4500
    space(SAMSUNG_HEADER_SPACE);   // - 4500
    mark(SAMSUNG_BIT_MARK);        // + 560
    space(SAMSUNG_ZERO_SPACE);     // - 560
    mark(SAMSUNG_BIT_MARK);        // + 560
}

/*
 * Sent e.g. by an LG 6711R1P071A remote
 * @param aNumberOfRepeats If < 0 then only a special repeat frame will be sent
 */
void sendSamsungLG(uint16_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats)
{
    if (aNumberOfRepeats < 0) {
        sendSamsungLGRepeat();
        return;
    }

    // send 16 bit address and  8 command bits and then 8 inverted command bits LSB first
    union LongUnion tRawData;
    tRawData.UWord.LowWord = aAddress;
    tRawData.UByte.MidHighByte = aCommand;
    tRawData.UByte.HighByte = ~aCommand;

    sendPulseDistanceWidthV1(&SamsungProtocolConstants, tRawData.ULong, SAMSUNG_BITS, aNumberOfRepeats);
}

/**
 * Here we send Samsung32
 * If we get a command < 0x100, we send command and then ~command
 * !!! Be aware, that this is flexible, but makes it impossible to send e.g. 0x0042 as 16 bit value!!!
 * @param aNumberOfRepeats If < 0 then only a special repeat frame will be sent
 */
void sendSamsung(uint16_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats)
{
    // send 16 bit address
    union LongUnion tSendValue;
    tSendValue.UWords[0] = aAddress;
    if (aCommand < 0x100) {
        // Send 8 command bits and then 8 inverted command bits LSB first
        tSendValue.UBytes[2] = aCommand;
        tSendValue.UBytes[3] = ~aCommand;
    } else {
        // Send 16 command bits
        tSendValue.UWords[1] = aCommand;
    }

    sendPulseDistanceWidthV1(&SamsungProtocolConstants, tSendValue.ULong, SAMSUNG_BITS, aNumberOfRepeats);
}

/**
 * Here we send Samsung48
 * We send 2 x (8 bit command and then ~command)
 */
void sendSamsung48(uint16_t aAddress, uint32_t aCommand, uint8_t aNumberOfRepeats)
{
    // send 16 bit address and 2 x ( 8 command bits and then 8 inverted command bits) LSB first

    uint32_t tRawSamsungData[2]; // prepare 2 long for Samsung48

    union LongUnion tSendValue;
    tSendValue.UWords[0] = aAddress;
    tSendValue.UBytes[2] = aCommand;
    tSendValue.UBytes[3] = ~aCommand;
    uint8_t tUpper8BitsOfCommand = aCommand >> 8;
    tRawSamsungData[1] = tUpper8BitsOfCommand | (~tUpper8BitsOfCommand) << 8;
    tRawSamsungData[0] = tSendValue.ULong;

    sendPulseDistanceWidthFromArray(&SamsungProtocolConstants, &tRawSamsungData[0], SAMSUNG48_BITS, aNumberOfRepeats);
#if 0
    LongLongUnion tSendValue;
    tSendValue.UWords[0] = aAddress;
    if (aCommand < 0x10000) {
        tSendValue.UBytes[2] = aCommand;
        tSendValue.UBytes[3] = ~aCommand;
        uint8_t tUpper8BitsOfCommand = aCommand >> 8;
        tSendValue.UBytes[4] = tUpper8BitsOfCommand;
        tSendValue.UBytes[5] = ~tUpper8BitsOfCommand;
    } else {
        tSendValue.ULongLong = aAddress | aCommand << 16;
    }
    sendPulseDistanceWidth(&SamsungProtocolConstants, tSendValue.ULongLong, SAMSUNG48_BITS, aNumberOfRepeats);
#endif
}

bool decodeSamsung(void)
{
    // Check we have enough data (68). The +4 is for initial gap, start bit mark and space + stop bit mark
    if (decodedIRData.rawDataPtr->rawlen != ((2 * SAMSUNG_BITS) + 4)
            && decodedIRData.rawDataPtr->rawlen != ((2 * SAMSUNG48_BITS) + 4) && (decodedIRData.rawDataPtr->rawlen != 6)) {
#ifdef DEBUG_SAMSUNG
        NRF_LOG_INFO("Samsung: Data length = %d is not 6 or 68 or 100", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    if (!checkHeader(&SamsungProtocolConstants)) {
        return false;
    }

    // Check for SansungLG style repeat
    if (decodedIRData.rawDataPtr->rawlen == 6) {
        decodedIRData.flags = IRDATA_FLAGS_IS_REPEAT | IRDATA_FLAGS_IS_LSB_FIRST;
        decodedIRData.address = lastDecodedAddress;
        decodedIRData.command = lastDecodedCommand;
        decodedIRData.protocol = SAMSUNG_LG;
        return true;
    }

    /*
     * Decode first 32 bits
     */
    if (!decodePulseDistanceWidthDatav2(&SamsungProtocolConstants, SAMSUNG_BITS, 3))
    {
#ifdef DEBUG_SAMSUNG
        NRF_LOG_INFO("Samsung: Decode failed");
#endif
        return false;
    }
    union LongUnion tValue;
    tValue.ULong = decodedIRData.decodedRawData;
    decodedIRData.address = tValue.UWord.LowWord;

    if (decodedIRData.rawDataPtr->rawlen == (2 * SAMSUNG48_BITS) + 4)
    {
        /*
         * Samsung48
         */
        // decode additional 16 bit
        if (!decodePulseDistanceWidthDatav2(&SamsungProtocolConstants, (SAMSUNG_COMMAND32_BITS - SAMSUNG_COMMAND16_BITS),
                3 + (2 * SAMSUNG_BITS)))
        {
#ifdef DEBUG_SAMSUNG
        	NRF_LOG_INFO("Samsung: Decode failed");
#endif
            return false;
        }

        /*
         * LSB data is already in tValue.UWord.HighWord!
         * Put latest (MSB) bits in LowWord, LSB first would have them expect in HighWord so keep this in mind for decoding below
         */
        tValue.UWord.LowWord = decodedIRData.decodedRawData; // We have only 16 bit in decodedRawData here
//#if __INT_WIDTH__ >= 32
//        // workaround until complete refactoring for 64 bit
//        decodedIRData.decodedRawData = (decodedIRData.decodedRawData << 32)| tValue.UWord.HighWord << 16 | decodedIRData.address; // store all 48 bits in decodedRawData
//#endif

        /*
         * Check parity of 32 bit command
         */
        // receive 2 * (8 bits then 8 inverted bits) LSB first
        if (tValue.UByte.MidHighByte != (uint8_t)(~tValue.UByte.HighByte)
                && tValue.UByte.LowByte != (uint8_t)(~tValue.UByte.MidLowByte)) {
            decodedIRData.flags = IRDATA_FLAGS_PARITY_FAILED | IRDATA_FLAGS_IS_LSB_FIRST;
        }

        decodedIRData.command = tValue.UByte.LowByte << 8 | tValue.UByte.MidHighByte; // low and high word are swapped here, so fetch it this way
        decodedIRData.numberOfBits = SAMSUNG48_BITS;
        decodedIRData.protocol = SAMSUNG48;

    } else {
        /*
         * Samsung32
         */
        if (tValue.UByte.MidHighByte == (uint8_t)(~tValue.UByte.HighByte)) {
            // 8 bit command protocol
            decodedIRData.command = tValue.UByte.MidHighByte; // first 8 bit
        } else {
            // 16 bit command protocol
            decodedIRData.command = tValue.UWord.HighWord; // first 16 bit
        }
        decodedIRData.numberOfBits = SAMSUNG_BITS;
        decodedIRData.protocol = SAMSUNG;
    }

    // check for repeat
    checkForRepeatSpaceTicksAndSetFlag(SAMSUNG_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);

    return true;
}
