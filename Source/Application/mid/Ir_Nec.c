#include "main.h"
#include "Mid_IrRemote.h"

PulseDistanceWidthProtocolConstants NECProtocolConstants =
        {NEC, NEC_KHZ, {NEC_HEADER_MARK, NEC_HEADER_SPACE, NEC_BIT_MARK,
        NEC_ONE_SPACE, NEC_BIT_MARK, NEC_ZERO_SPACE}, PROTOCOL_IS_LSB_FIRST, (NEC_REPEAT_PERIOD / MICROS_IN_ONE_MILLI),
                &sendNECSpecialRepeat };

// Like NEC but repeats are full frames instead of special NEC repeats
PulseDistanceWidthProtocolConstants NEC2ProtocolConstants = {NEC2, NEC_KHZ, {NEC_HEADER_MARK, NEC_HEADER_SPACE, NEC_BIT_MARK,
NEC_ONE_SPACE, NEC_BIT_MARK, NEC_ZERO_SPACE}, PROTOCOL_IS_LSB_FIRST, (NEC_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), NULL };

/************************************
 * Start of send and decode functions
 ************************************/

/**
 * Send special NEC repeat frame
 * Repeat commands should be sent in a 110 ms raster.
 */
void sendNECRepeat(void)
{
    enableIROut (NEC_KHZ);           // 38 kHz
    mark(NEC_HEADER_MARK);          // + 9000
    space(NEC_REPEAT_HEADER_SPACE); // - 2250
    mark(NEC_BIT_MARK);             // + 560
}

/**
 * Static function for sending special repeat frame.
 * For use in ProtocolConstants. Saves up to 250 bytes compared to a member function.
 */
void sendNECSpecialRepeat(void)
{
    enableIROut(NEC_KHZ);           // 38 kHz
    mark(NEC_HEADER_MARK);          // + 9000
    space(NEC_REPEAT_HEADER_SPACE); // - 2250
    mark(NEC_BIT_MARK);             // + 560
}

uint32_t computeNECRawDataAndChecksum(uint16_t aAddress, uint16_t aCommand) {
    union LongUnion tRawData;

    // Address 16 bit LSB first
    if ((aAddress & 0xFF00) == 0) {
        // assume 8 bit address -> send 8 address bits and then 8 inverted address bits LSB first
        tRawData.UByte.LowByte = aAddress;
        tRawData.UByte.MidLowByte = ~tRawData.UByte.LowByte;
    } else {
        tRawData.UWord.LowWord = aAddress;
    }

    // send 8 command bits and then 8 inverted command bits LSB first
    tRawData.UByte.MidHighByte = aCommand;
    tRawData.UByte.HighByte = ~aCommand;
    return tRawData.ULong;
}

/**
 * NEC Send frame and special repeats
 * There is NO delay after the last sent repeat!
 * @param aNumberOfRepeats  If < 0 then only a special repeat frame without leading and trailing space
 *                          will be sent by calling NECProtocolConstants.SpecialSendRepeatFunction().
 */
void sendNEC(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats) {
    sendPulseDistanceWidthV1(&NECProtocolConstants, computeNECRawDataAndChecksum(aAddress, aCommand), NEC_BITS, aNumberOfRepeats);
}

/*
 * NEC2 Send frame and repeat the frame for each requested repeat
 * There is NO delay after the last sent repeat!
 * @param aNumberOfRepeats  If < 0 then nothing is sent.
 */
void sendNEC2(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats) {
    sendPulseDistanceWidthV1(&NEC2ProtocolConstants, computeNECRawDataAndChecksum(aAddress, aCommand), NEC_BITS, aNumberOfRepeats);
}

/*
 * Repeat commands should be sent in a 110 ms raster.
 * There is NO delay after the last sent repeat!
 * @param aNumberOfRepeats  If < 0 then only a special repeat frame without leading and trailing space
 *                          will be sent by calling NECProtocolConstants.SpecialSendRepeatFunction().
 */
void sendOnkyo(uint16_t aAddress, uint16_t aCommand, uint8_t aNumberOfRepeats) {
    sendPulseDistanceWidthV1(&NECProtocolConstants, (uint32_t) aCommand << 16 | aAddress, NEC_BITS, aNumberOfRepeats);
}

/*
 * Repeat commands should be sent in a 110 ms raster.
 * There is NO delay after the last sent repeat!
 * https://en.wikipedia.org/wiki/Apple_Remote
 * https://gist.github.com/darconeous/4437f79a34e3b6441628
 * @param aAddress is the DeviceId*
 * @param aNumberOfRepeats  If < 0 then only a special repeat frame without leading and trailing space
 *                          will be sent by calling NECProtocolConstants.SpecialSendRepeatFunction().
 */
void sendApple(uint8_t aDeviceId, uint8_t aCommand, uint8_t aNumberOfRepeats) {
    union LongUnion tRawData;

    // Address 16 bit LSB first
    tRawData.UWord.LowWord = APPLE_ADDRESS;

    // send Apple code and then 8 command bits LSB first
    tRawData.UByte.MidHighByte = aCommand;
    tRawData.UByte.HighByte = aDeviceId; // e.g. 0xD7

    sendPulseDistanceWidthV1(&NECProtocolConstants, tRawData.ULong, NEC_BITS, aNumberOfRepeats);
}

/*
 * Sends NEC1 protocol
 * @param aNumberOfRepeats  If < 0 then only a special repeat frame without leading and trailing space
 *                          will be sent by calling NECProtocolConstants.SpecialSendRepeatFunction().
 */
void sendNECRaw(uint32_t aRawData, uint8_t aNumberOfRepeats) {
    sendPulseDistanceWidthV1(&NECProtocolConstants, aRawData, NEC_BITS, aNumberOfRepeats);
}

/**
 * Decodes also Onkyo and Apple
 */

bool decodeNec(void)
{
	/*
	 * First check for right data length
	 * Next check start bit
	 * Next try the decode
	 */
	// Check we have the right amount of data (68). The +4 is for initial gap, start bit mark and space + stop bit mark.
	if (decodedIRData.rawDataPtr->rawlen != ((2 * NEC_BITS) + 4) && (decodedIRData.rawDataPtr->rawlen != 4)) {
//		IR_DEBUG_PRINT(F("NEC: "));
//		IR_DEBUG_PRINT(F("Data length="));
//		IR_DEBUG_PRINT(decodedIRData.rawDataPtr->rawlen);
//		IR_DEBUG_PRINTLN(F(" is not 68 or 4"));
		return false;
	}

	// Check header "mark" this must be done for repeat and data
	if (!matchMark(decodedIRData.rawDataPtr->rawbuf[1], NEC_HEADER_MARK)) {
		return false;
	}

	// Check for repeat - here we have another header space length
	if (decodedIRData.rawDataPtr->rawlen == 4) {
		if (matchSpace(decodedIRData.rawDataPtr->rawbuf[2], NEC_REPEAT_HEADER_SPACE)
				&& matchMark(decodedIRData.rawDataPtr->rawbuf[3], NEC_BIT_MARK)) {
			decodedIRData.flags = IRDATA_FLAGS_IS_REPEAT | IRDATA_FLAGS_IS_LSB_FIRST;
			decodedIRData.address = lastDecodedAddress;
			decodedIRData.command = lastDecodedCommand;
			decodedIRData.protocol = lastDecodedProtocol;
			return true;
		}
		return false;
	}

	// Check command header space
	if (!matchSpace(decodedIRData.rawDataPtr->rawbuf[2], NEC_HEADER_SPACE)) {
#if defined(LOCAL_DEBUG)
		Serial.print(F("NEC: "));
		Serial.println(F("Header space length is wrong"));
#endif
		return false;
	}

	// Try to decode as NEC protocol
	if (!decodePulseDistanceWidthDatav3(&NECProtocolConstants, NEC_BITS)) {
#if defined(LOCAL_DEBUG)
		Serial.print(F("NEC: "));
		Serial.println(F("Decode failed"));
#endif
		return false;
	}

	// Success
//    decodedIRData.flags = IRDATA_FLAGS_IS_LSB_FIRST; // Not required, since this is the start value
	union LongUnion tValue;
	tValue.ULong = decodedIRData.decodedRawData;
	decodedIRData.command = tValue.UByte.MidHighByte; // 8 bit

#if defined(DECODE_ONKYO)
	// Here only Onkyo protocol is supported -> force 16 bit address and command decoding
	decodedIRData.address = tValue.UWord.LowWord; // first 16 bit
	decodedIRData.protocol = ONKYO;
	decodedIRData.command = tValue.UWord.HighWord; // 16 bit command
#else
	// Address
	if (tValue.UWord.LowWord == APPLE_ADDRESS) {
		/*
		 * Apple
		 */
		decodedIRData.protocol = APPLE;
		decodedIRData.address = tValue.UByte.HighByte;

	} else {
		/*
		 * NEC LSB first, so first sent bit is also LSB of decodedIRData.decodedRawData
		 */
		if (tValue.UByte.LowByte == (uint8_t)(~tValue.UByte.MidLowByte)) {
			// standard 8 bit address NEC protocol
			decodedIRData.address = tValue.UByte.LowByte; // first 8 bit
		} else {
			// extended NEC protocol
			decodedIRData.address = tValue.UWord.LowWord; // first 16 bit
		}
		// Check for command if it is 8 bit NEC or 16 bit ONKYO
		if (tValue.UByte.MidHighByte == (uint8_t)(~tValue.UByte.HighByte)) {
			decodedIRData.protocol = NEC;
		} else {
			decodedIRData.protocol = ONKYO;
			decodedIRData.command = tValue.UWord.HighWord; // 16 bit command
		}
	}
#endif

	decodedIRData.numberOfBits = NEC_BITS;

	// check for NEC2 repeat, do not check for same content ;-)
	checkForRepeatSpaceTicksAndSetFlag(NEC_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);
	if (decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
		decodedIRData.protocol = NEC2;
	}
	return true;
}




