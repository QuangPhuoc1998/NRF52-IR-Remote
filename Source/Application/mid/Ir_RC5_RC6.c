#include "main.h"

uint8_t sLastSendToggleValue = 1; // To start first command with toggle 0

extern uint8_t sBiphaseDecodeRawbuffOffset; // Index into raw timing array
extern uint16_t sBiphaseCurrentTimingIntervals; // 1, 2 or 3. Number of aBiphaseTimeUnit intervals of the current rawbuf[sBiphaseDecodeRawbuffOffset] timing.
extern uint8_t sBiphaseUsedTimingIntervals;       // Number of already used intervals of sCurrentTimingIntervals.
extern uint16_t sBiphaseTimeUnit;

/************************************
 * Start of send and decode functions
 ************************************/

/**
 * @param aCommand If aCommand is >=0x40 then we switch automatically to RC5X.
 * @param aEnableAutomaticToggle Send toggle bit according to the state of the static sLastSendToggleValue variable.
 */
void sendRC5(uint8_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, bool aEnableAutomaticToggle)
{
    // Set IR carrier frequency
    enableIROut (RC5_RC6_KHZ);

    uint16_t tIRData = ((aAddress & 0x1F) << RC5_COMMAND_BITS);

    if (aCommand < 0x40) {
        // Auto discovery of RC5X, set field bit to 1
        tIRData |= 1 << (RC5_TOGGLE_BIT + RC5_ADDRESS_BITS + RC5_COMMAND_BITS);
    } else {
        // Mask bit 7 of command and let field bit 0
        aCommand &= 0x3F;
    }
    tIRData |= aCommand;

    if (aEnableAutomaticToggle) {
        if (sLastSendToggleValue == 0) {
            sLastSendToggleValue = 1;
            // set toggled bit
            tIRData |= 1 << (RC5_ADDRESS_BITS + RC5_COMMAND_BITS);
        } else {
            sLastSendToggleValue = 0;
        }
    }

    uint_fast8_t tNumberOfCommands = aNumberOfRepeats + 1;
    while (tNumberOfCommands > 0) {

        // start bit is sent by sendBiphaseData
        sendBiphaseData(RC5_UNIT, tIRData, RC5_BITS);

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            // send repeated command in a fixed raster
            nrf_delay_ms(RC5_REPEAT_DISTANCE / MICROS_IN_ONE_MILLI);
        }
    }
}

/**
 * Try to decode data as RC5 protocol
 *                             _   _   _   _   _   _   _   _   _   _   _   _   _
 * Clock                 _____| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |
 *                                ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^   ^    End of each data bit period
 *                               _   _     - Mark
 * 2 Start bits for RC5    _____| |_| ...  - Data starts with a space->mark bit
 *                                         - Space
 *                               _
 * 1 Start bit for RC5X    _____| ...
 *
 */
bool decodeRC5(void)
{
    uint8_t tBitIndex;
    uint32_t tDecodedRawData = 0;

    // Set Biphase decoding start values
    initBiphaselevel(1, RC5_UNIT); // Skip gap space

    // Check we have the right amount of data (11 to 26). The +2 is for initial gap and start bit mark.
    if (decodedIRData.rawDataPtr->rawlen < ((RC5_BITS + 1) / 2) + 2 && (RC5_BITS + 2) < decodedIRData.rawDataPtr->rawlen) {
        // no debug output, since this check is mainly to determine the received protocol
#ifdef DEBUG_RC5_RC6
        IR_DEBUG_PRINT(F("RC5: "));
        IR_DEBUG_PRINT(F("Data length="));
        IR_DEBUG_PRINT(decodedIRData.rawDataPtr->rawlen);
        IR_DEBUG_PRINTLN(F(" is not between 9 and 15"));
#endif
        return false;
    }

// Check start bit, the first space is included in the gap
    if (getBiphaselevel() != MARK)
    {
#ifdef DEBUG_RC5_RC6
        IR_DEBUG_PRINT(F("RC5: "));
        IR_DEBUG_PRINTLN(F("first getBiphaselevel() is not MARK"));
#endif
        return false;
    }

    /*
     * Get data bits - MSB first
     */
    for (tBitIndex = 0; sBiphaseDecodeRawbuffOffset < decodedIRData.rawDataPtr->rawlen; tBitIndex++) {
        // get next 2 levels and check for transition
        uint8_t tStartLevel = getBiphaselevel();
        uint8_t tEndLevel = getBiphaselevel();

        if ((tStartLevel == SPACE) && (tEndLevel == MARK)) {
            // we have a space to mark transition here
            tDecodedRawData = (tDecodedRawData << 1) | 1;
        } else if ((tStartLevel == MARK) && (tEndLevel == SPACE)) {
            // we have a mark to space transition here
            tDecodedRawData = (tDecodedRawData << 1) | 0;
        } else {
#if defined(LOCAL_DEBUG)
            Serial.print(F("RC5: "));
            Serial.println(F("no transition found, decode failed"));
#endif
            return false;
        }
    }

    // Success
    decodedIRData.numberOfBits = tBitIndex; // must be RC5_BITS

    union LongUnion tValue;
    tValue.ULong = tDecodedRawData;
    decodedIRData.decodedRawData = tDecodedRawData;
    decodedIRData.command = tValue.UByte.LowByte & 0x3F;
    decodedIRData.address = (tValue.UWord.LowWord >> RC5_COMMAND_BITS) & 0x1F;

    // Get the inverted 7. command bit for RC5X, the inverted value is always 1 for RC5 and serves as a second start bit.
    if ((tValue.UWord.LowWord & (1 << (RC5_TOGGLE_BIT + RC5_ADDRESS_BITS + RC5_COMMAND_BITS))) == 0) {
        decodedIRData.command += 0x40;
    }

    decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;
    if (tValue.UByte.MidLowByte & 0x8) {
        decodedIRData.flags = IRDATA_FLAGS_TOGGLE_BIT | IRDATA_FLAGS_IS_MSB_FIRST;
    }
    decodedIRData.protocol = RC5;

    // check for repeat
    checkForRepeatSpaceTicksAndSetFlag(RC5_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);

    return true;
}

/**
 * Main RC6 send function
 */
void sendRC6With32Bit(uint32_t aRawData, uint8_t aNumberOfBitsToSend)
{
	sendRC6RawWith32Bit(aRawData, aNumberOfBitsToSend);
}
void sendRC6RawWith32Bit(uint32_t aRawData, uint8_t aNumberOfBitsToSend)
{
// Set IR carrier frequency
    enableIROut (RC5_RC6_KHZ);

// Header
    mark(RC6_HEADER_MARK);
    space(RC6_HEADER_SPACE);

// Start bit
    mark(RC6_UNIT);
    space(RC6_UNIT);

// Data MSB first
    uint32_t mask = 1UL << (aNumberOfBitsToSend - 1);
    for (uint_fast8_t i = 1; mask; i++, mask >>= 1) {
        // The fourth bit we send is the "double width toggle bit"
        unsigned int t = (i == (RC6_TOGGLE_BIT_INDEX + 1)) ? (RC6_UNIT * 2) : (RC6_UNIT);
        if (aRawData & mask) {
            mark(t);
            space(t);
        } else {
            space(t);
            mark(t);
        }
    }
}

/**
 * Send RC6 64 bit raw data
 * Can be used to send RC6A with ?31? data bits
 */
void sendRC6With64Bit(uint64_t aRawData, uint8_t aNumberOfBitsToSend)
{
	sendRC6RawWith64Bit(aRawData, aNumberOfBitsToSend);
}
void sendRC6RawWith64Bit(uint64_t aRawData, uint8_t aNumberOfBitsToSend)
{
// Set IR carrier frequency
    enableIROut (RC5_RC6_KHZ);

// Header
    mark(RC6_HEADER_MARK);
    space(RC6_HEADER_SPACE);

// Start bit
    mark(RC6_UNIT);
    space(RC6_UNIT);

// Data MSB first
    uint64_t mask = 1ULL << (aNumberOfBitsToSend - 1);
    for (uint_fast8_t i = 1; mask; i++, mask >>= 1) {
        // The fourth bit we send is the "double width toggle bit"
        unsigned int t = (i == (RC6_TOGGLE_BIT_INDEX + 1)) ? (RC6_UNIT * 2) : (RC6_UNIT);
        if (aRawData & mask) {
            mark(t);
            space(t);
        } else {
            space(t);
            mark(t);
        }
    }
}

/**
 * Assemble raw data for RC6 from parameters and toggle state and send
 * We do not wait for the minimal trailing space of 2666 us
 * @param aEnableAutomaticToggle Send toggle bit according to the state of the static sLastSendToggleValue variable.
 */
void sendRC6(uint8_t aAddress, uint8_t aCommand, int_fast8_t aNumberOfRepeats, bool aEnableAutomaticToggle)
{

    union LongUnion tIRRawData;
    tIRRawData.UByte.LowByte = aCommand;
    tIRRawData.UByte.MidLowByte = aAddress;

    tIRRawData.UWord.HighWord = 0; // must clear high word
    if (aEnableAutomaticToggle) {
        if (sLastSendToggleValue == 0) {
            sLastSendToggleValue = 1;
            // set toggled bit
            tIRRawData.UByte.MidHighByte = 1; // 3 Mode bits are 0
        } else {
            sLastSendToggleValue = 0;
        }
    }

#if defined(LOCAL_DEBUG)
    Serial.print(F("RC6: "));
    Serial.print(F("sLastSendToggleValue="));
    Serial.print (sLastSendToggleValue);
    Serial.print(F(" RawData="));
    Serial.println(tIRRawData.ULong, HEX);
#endif

    uint_fast8_t tNumberOfCommands = aNumberOfRepeats + 1;
    while (tNumberOfCommands > 0) {

        // start and leading bits are sent by sendRC6
        sendRC6RawWith32Bit(tIRRawData.ULong, RC6_BITS - 1); // -1 since the leading bit is additionally sent by sendRC6

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            // send repeated command in a fixed raster
            nrf_delay_ms(RC6_REPEAT_DISTANCE / MICROS_IN_ONE_MILLI);
        }
    }
}

/**
 * Try to decode data as RC6 protocol
 */
bool decodeRC6(void)
{
    uint8_t tBitIndex;
    uint32_t tDecodedRawData = 0;

    // Check we have the right amount of data (). The +3 for initial gap, start bit mark and space
    if (decodedIRData.rawDataPtr->rawlen < MIN_RC6_MARKS + 3 && (RC6_BITS + 3) < decodedIRData.rawDataPtr->rawlen) {
#ifdef DEBUG_RC5_RC6
    	IR_DEBUG_PRINT(F("RC6: "));
        IR_DEBUG_PRINT(F("Data length="));
        IR_DEBUG_PRINT(decodedIRData.rawDataPtr->rawlen);
        IR_DEBUG_PRINTLN(F(" is not between 15 and 25"));
#endif
        return false;
    }

    // Check header "mark" and "space", this must be done for repeat and data
    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[1], RC6_HEADER_MARK)
            || !matchSpace(decodedIRData.rawDataPtr->rawbuf[2], RC6_HEADER_SPACE)) {
        // no debug output, since this check is mainly to determine the received protocol
#ifdef DEBUG_RC5_RC6
        IR_DEBUG_PRINT(F("RC6: "));
        IR_DEBUG_PRINTLN(F("Header mark or space length is wrong"));
#endif
        return false;
    }

    // Set Biphase decoding start values
    initBiphaselevel(3, RC6_UNIT); // Skip gap-space and start-bit mark + space

// Process first bit, which is known to be a 1 (mark->space)
    if (getBiphaselevel() != MARK)
    {
#ifdef DEBUG_RC5_RC6
        IR_DEBUG_PRINT(F("RC6: "));
        IR_DEBUG_PRINTLN(F("first getBiphaselevel() is not MARK"));
#endif
        return false;
    }
    if (getBiphaselevel() != SPACE)
    {
#ifdef DEBUG_RC5_RC6
        IR_DEBUG_PRINT(F("RC6: "));
        IR_DEBUG_PRINTLN(F("second getBiphaselevel() is not SPACE"));
#endif
        return false;
    }

    for (tBitIndex = 0; sBiphaseDecodeRawbuffOffset < decodedIRData.rawDataPtr->rawlen; tBitIndex++) {
        uint8_t tStartLevel; // start level of coded bit
        uint8_t tEndLevel;   // end level of coded bit

        tStartLevel = getBiphaselevel();
        if (tBitIndex == RC6_TOGGLE_BIT_INDEX) {
            // Toggle bit is double wide; make sure second half is equal first half
            if (tStartLevel != getBiphaselevel()) {
#ifdef DEBUG_RC5_RC6
                Serial.print(F("RC6: "));
                Serial.println(F("Toggle mark or space length is wrong"));
#endif
                return false;
            }
        }

        tEndLevel = getBiphaselevel();
        if (tBitIndex == RC6_TOGGLE_BIT_INDEX) {
            // Toggle bit is double wide; make sure second half matches
            if (tEndLevel != getBiphaselevel()) {
#ifdef DEBUG_RC5_RC6
                Serial.print(F("RC6: "));
                Serial.println(F("Toggle mark or space length is wrong"));
#endif
                return false;
            }
        }

        /*
         * Determine tDecodedRawData bit value by checking the transition type
         */
        if ((tStartLevel == MARK) && (tEndLevel == SPACE)) {
            // we have a mark to space transition here
            tDecodedRawData = (tDecodedRawData << 1) | 1;  // inverted compared to RC5
        } else if ((tStartLevel == SPACE) && (tEndLevel == MARK)) {
            // we have a space to mark transition here
            tDecodedRawData = (tDecodedRawData << 1) | 0;
        } else {
#ifdef DEBUG_RC5_RC6
            Serial.print(F("RC6: "));
            Serial.println(F("Decode failed"));
#endif
            // we have no transition here or one level is -1 -> error
            return false;            // Error
        }
    }

// Success
    decodedIRData.numberOfBits = tBitIndex;

    union LongUnion tValue;
    tValue.ULong = tDecodedRawData;
    decodedIRData.decodedRawData = tDecodedRawData;

    if (tBitIndex < 36) {
        // RC6 8 address bits, 8 command bits
        decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;
        decodedIRData.command = tValue.UByte.LowByte;
        decodedIRData.address = tValue.UByte.MidLowByte;
        // Check for toggle flag
        if ((tValue.UByte.MidHighByte & 1) != 0) {
            decodedIRData.flags = IRDATA_FLAGS_TOGGLE_BIT | IRDATA_FLAGS_IS_MSB_FIRST;
        }
        if (tBitIndex > 20) {
            decodedIRData.flags |= IRDATA_FLAGS_EXTRA_INFO;
        }
    } else {
        // RC6A - 32 bits
        decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;
        if ((tValue.UByte.MidLowByte & 0x80) != 0) {
            decodedIRData.flags = IRDATA_FLAGS_TOGGLE_BIT | IRDATA_FLAGS_IS_MSB_FIRST;
        }
        tValue.UByte.MidLowByte &= 0x87F; // mask toggle bit
        decodedIRData.command = tValue.UByte.LowByte;
        decodedIRData.address = tValue.UByte.MidLowByte;
    }

    // check for repeat, do not check toggle bit yet
    checkForRepeatSpaceTicksAndSetFlag(RC6_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);

    decodedIRData.protocol = RC6;
    return true;
}
